#include "AMRestAction.h"

#include "util/AMErrorMonitor.h"

#include "qjson/serializer.h"
#include "qjson/parser.h"

#include <QDebug>

AMRestAction::AMRestAction(AMRestActionInfo *info, QNetworkAccessManager *networkAccessManager, QObject *parent) :
	AMAction3(info, parent)
{
	restReply_ = 0;
	networkAccessManager_ = networkAccessManager;
}

AMRestAction::AMRestAction(const AMRestAction &other) :
	AMAction3(other)
{
	restReply_ = 0;
	networkAccessManager_ = other.networkAccessManager();
}

AMAction3* AMRestAction::createCopy() const
{
	return new AMRestAction(*this);
}

void AMRestAction::startImplementation()
{
	AMRestActionInfo *actionInfo = restActionInfo();

	if(actionInfo->urlString().isEmpty()){
		QString fundamentalFailureMessage = QString("There was an error starting the request because the URL was empty.");
		AMErrorMon::alert(this, AMRESTACTION_CANNOT_START_WITH_EMPTY_URL, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}
	if(!networkAccessManager_){
		QString fundamentalFailureMessage = QString("There was an error starting the request to %1, no network access manager was available.").arg(actionInfo->urlString());
		AMErrorMon::alert(this, AMRESTACTION_CANNOT_START_WITHOUT_NETWORKACCESSMANAGER, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}

	QNetworkRequest request;
	request.setUrl(actionInfo->urlString());

	AMRestActionRawHeaderMap::const_iterator i = actionInfo->rawHeaders().constBegin();
	while (i != actionInfo->rawHeaders().constEnd()) {
		request.setRawHeader(i.key(), i.value());
		++i;
	}

	QVariantMap jdata;
	jdata["body"] = "FIXME";
	QJson::Serializer jserializer;
	QByteArray jsonData = jserializer.serialize(jdata);

	QBuffer *buffer = new QBuffer;
	buffer->setData(jsonData);
	buffer->open(QIODevice::ReadOnly);

	switch(actionInfo->verbRequestType()){
	case AMRestActionInfo::GetRequest:
//		qDebug() << "Sending the get request: " << request.url() << request.rawHeader("Authorization");
		restReply_ = networkAccessManager_->get(request);
		break;
	case AMRestActionInfo::PostRequest:
		restReply_ = networkAccessManager_->post(request, buffer);
		break;
	case AMRestActionInfo::PatchRequest:
		restReply_ = networkAccessManager_->sendCustomRequest(request, "PATCH", buffer);
		break;
	default:
		break;
	}

	restReply_->ignoreSslErrors();

	connect(restReply_, SIGNAL(readyRead()), this, SLOT(onNetworkRequestReturned()));
	connect(restReply_, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onNetworkRequestError(QNetworkReply::NetworkError)));

	setStarted();
}

void AMRestAction::pauseImplementation()
{
}

void AMRestAction::resumeImplementation()
{
}

void AMRestAction::cancelImplementation()
{
	setCancelled();
}

void AMRestAction::skipImplementation(const QString &command)
{
	Q_UNUSED(command)
}

QString AMRestAction::jsonSensiblePrint(const QVariantMap &jsonMap, int indentLevel) const{
	QString retVal;
	QString tabLevel;
	for(int x = 0; x < indentLevel; x++)
		tabLevel.append("\t");
	QMap<QString, QVariant>::const_iterator i = jsonMap.constBegin();
	while (i != jsonMap.constEnd()) {
		if(i.value().type() == QVariant::ULongLong)
			retVal.append(QString("%1\"%2\": \"%3\"\n").arg(tabLevel).arg(i.key()).arg(i.value().toULongLong()));
		else if(i.value().type() == QVariant::String)
			retVal.append(QString("%1\"%2\": \"%3\"\n").arg(tabLevel).arg(i.key()).arg(i.value().toString()));
		else if(i.value().canConvert(QVariant::Map))
			retVal.append(QString("%1\"%2\":\n%3").arg(tabLevel).arg(i.key()).arg(jsonSensiblePrint(i.value().toMap(), indentLevel+1)));
		++i;
	}
	return retVal;
}

int AMRestAction::pageNumberFromURLString(const QString &urlString){
	QRegExp pageNumberRegExp = QRegExp("page=(\\d+)");
	pageNumberRegExp.indexIn(urlString);
	bool conversionOk = false;
	int retVal = pageNumberRegExp.cap(1).toInt(&conversionOk);
	if(!conversionOk)
		retVal = -1;
	return retVal;
}

void AMRestAction::setNetworkAccessManager(QNetworkAccessManager *networkAccessManager)
{
	networkAccessManager_ = networkAccessManager;
}

void AMRestAction::onNetworkRequestReturned()
{
//	qDebug() << "Network Request Returned";
	QJson::Parser parser;
	QVariant githubFullReply = parser.parse(restReply_->readAll());

//	QList<QByteArray> headerList = restReply_->rawHeaderList();
	QList<QNetworkReply::RawHeaderPair> headerPairs = restReply_->rawHeaderPairs();

//	for(int x = 0; x < headerList.count(); x++){
//		if(headerList.at(x) == "Link"){
//			QString linkHeader = restReply_->rawHeader(headerList.at(x));
//			qDebug() << "Full link header: " << linkHeader;
//			int lastPageNumber = -1;
//			int nextPageNumber = -1;
//			QStringList linkHeaderItems = linkHeader.split(',');
//			for(int y = 0; y < linkHeaderItems.count(); y++){
//				if(linkHeaderItems.at(y).contains("; rel=\"last\""))
//					lastPageNumber = pageNumberFromURLString(linkHeaderItems.at(y));
//				if(linkHeaderItems.at(y).contains("; rel=\"next\""))
//					nextPageNumber = pageNumberFromURLString(linkHeaderItems.at(y));
//			}

//			//lastPageNumber_ = lastPageNumber;
//			qDebug() << "Last page " << lastPageNumber;
//			qDebug() << "Next page " << nextPageNumber;
//		}
//	}

	fullResponse_ = githubFullReply;
	emit fullResponseReady(fullResponse_, headerPairs);

	setSucceeded();
}

void AMRestAction::onNetworkRequestError(QNetworkReply::NetworkError networkError)
{
	qDebug() << "Network Request Error";
	QString fundamentalFailureMessage = QString("A network error occurred: %1").arg(networkError);
	AMErrorMon::alert(this, AMRESTACTION_NETWORK_ERROR_FAILURE, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
	setFailed(fundamentalFailureMessage);
}

//AMGitHubIssue::AMGitHubIssue(int issueNumber, AMGitHubIssue::ComplexityValue complexityValue, const QString &title, bool isPullRequest, AMGitHubIssue *originatingIssue, int originatingIssueNumber, int commentCount, const QString &commentsURL, const QString &timeEstimateString, const QString &assignee, bool projectTrackingDisabled, bool inlineIssue, AMGitHubIssue::IssueState issueState, QObject *parent) :
//	QObject(parent)
//{
//	issueNumber_ = issueNumber;
//	complexityValue_ = complexityValue;
//	title_ = title;
//	isPullRequest_ = isPullRequest;
//	originatingIssue_ = originatingIssue;
//	originatingIssueNumber_ = originatingIssueNumber;
//	commentCount_ = commentCount;
//	commentsURL_ = commentsURL;
//	timeEstimateString_ = timeEstimateString;
//	assignee_ = assignee;
//	projectTrackingDisabled_ = projectTrackingDisabled;
//	inlineIssue_ = inlineIssue;
//	issueState_ = issueState;
//}

AMGitHubIssue::AMGitHubIssue(QVariantMap jsonMap, QObject *parent) :
	QObject(parent)
{
	issueNumber_ = -1;
	complexityValue_ = AMGitHubIssue::InvalidComplexity;
	title_ = "Invalid Title";
	isPullRequest_ = false;
	originatingIssue_ = 0;
	originatingIssueNumber_ = -1;
	commentCount_ = 0;
	commentsURL_ = "InvalidURL";
//	timeEstimateString_ = "Invalid Time Estimate";
	timeEstimateString_ = QString();
	assignee_ = "Invalid Assignee";
	projectTrackingDisabled_ = false;
	inlineIssue_ = false;
	issueState_ = AMGitHubIssue::InvalidState;

	if(jsonMap.value("number").canConvert<int>())
		issueNumber_ = jsonMap.value("number").toInt();
	if(jsonMap.value("title").canConvert<QString>()){
		title_ = jsonMap.value("title").toString();
		int startPoint = title_.indexOf("Issue") + 5;
		int endPoint = title_.indexOf(":");
		QString originatingIssue = title_.mid(startPoint, endPoint-startPoint);
		if(!originatingIssue.isEmpty())
			originatingIssueNumber_ = originatingIssue.toInt();
	}
	if(jsonMap.value("comments").canConvert<int>())
		commentCount_ = jsonMap.value("comments").toInt();
	if(jsonMap.value("comments_url").canConvert<QString>())
		commentsURL_ = jsonMap.value("comments_url").toString();
	if(jsonMap.contains("assignee") && jsonMap.value("assignee").toMap().value("login").canConvert<QString>())
		assignee_ = jsonMap.value("assignee").toMap().value("login").toString();
	if(jsonMap.contains("labels") && jsonMap.value("labels").canConvert<QVariantList>()){
		QVariantList labelsList = jsonMap.value("labels").toList();
		for(int x = 0, size = labelsList.count(); x < size; x++){
			QVariantMap labelMap = labelsList.at(x).toMap();
//			qDebug() << "One label: " << labelMap;
			if(labelMap.contains("name") && labelMap.value("name").toString().contains("Complexity"))
				complexityValue_ = complexityFromString(labelMap.value("name").toString());
			if(labelMap.contains("name") && labelMap.value("name").toString().contains("Project Tracking Disabled"))
				projectTrackingDisabled_ = true;
			if(labelMap.contains("name") && labelMap.value("name").toString().contains("Project Tracking Inline Issue"))
				inlineIssue_ = true;
		}
	}

	if(jsonMap.contains("pull_request") && !jsonMap.value("pull_request").isNull())
		isPullRequest_ = true;

	if(jsonMap.contains("state") && jsonMap.value("state").canConvert<QString>()){
		QString stateAsString = jsonMap.value("state").toString();
		if(stateAsString == "open")
			issueState_ = AMGitHubIssue::OpenState;
		else if(stateAsString == "closed")
			issueState_ = AMGitHubIssue::ClosedState;
	}

	if(jsonMap.contains("created_at") && jsonMap.value("created_at").canConvert<QString>()){
		QString createdDateString = jsonMap.value("created_at").toString();
		createdDate_ = QDateTime::fromString(createdDateString, "yyyy-MM-ddThh:mm:ssZ");
		if(!createdDate_.isValid()){
			qDebug() << "Not a valid created at date";
			createdDate_ = QDateTime();
		}
	}
	if(jsonMap.contains("closed_at") && jsonMap.value("closed_at").canConvert<QString>()){
		QString closedDateString = jsonMap.value("closed_at").toString();
		closedDate_ = QDateTime::fromString(closedDateString, "yyyy-MM-ddThh:mm:ssZ");
		if(!closedDate_.isValid()){
			qDebug() << "Not a valid closed at date";
			closedDate_ = QDateTime();
		}
	}
}

AMGitHubIssue::~AMGitHubIssue()
{
}

AMGitHubIssue::ComplexityValue AMGitHubIssue::complexityFromString(const QString &complexityString)
{
	if(complexityString == "Complexity1")
		return AMGitHubIssue::Complexity1;
	else if(complexityString == "Complexity2")
		return AMGitHubIssue::Complexity2;
	else if(complexityString == "Complexity3")
		return AMGitHubIssue::Complexity3;
	else if(complexityString == "Complexity5")
		return AMGitHubIssue::Complexity5;
	else if(complexityString == "Complexity8")
		return AMGitHubIssue::Complexity8;
	else if(complexityString == "ComplexityK")
		return AMGitHubIssue::ComplexityK;
	else
		return AMGitHubIssue::InvalidComplexity;
}

QString AMGitHubIssue::stringFromComplexity(AMGitHubIssue::ComplexityValue complexityValue){
	switch(complexityValue){
	case AMGitHubIssue::Complexity1:
		return "Complexity1";
	case AMGitHubIssue::Complexity2:
		return "Complexity2";
	case AMGitHubIssue::Complexity3:
		return "Complexity3";
	case AMGitHubIssue::Complexity5:
		return "Complexity5";
	case AMGitHubIssue::Complexity8:
		return "Complexity8";
	case AMGitHubIssue::ComplexityK:
		return "ComplexityK";
	case AMGitHubIssue::InvalidComplexity:
		return "Invalid Complexity";
	default:
		return "Invalid Complexity";
	}
}

QString AMGitHubIssue::stringFromState(AMGitHubIssue::IssueState issueState){
	switch(issueState){
	case AMGitHubIssue::OpenState:
		return "Open";
	case AMGitHubIssue::ClosedState:
		return "Closed";
	case AMGitHubIssue::InvalidState:
		return "Invalid State";
	}
}

QString AMGitHubIssue::oneLineDebugInfo() const
{
	QString retVal;
	if(!isPullRequest_)
		retVal = QString("[%1] %2: %3").arg(issueNumber_).arg(title_).arg(AMGitHubIssue::stringFromComplexity(complexityValue_));
	else if(originatingIssue_)
		retVal = QString("[%1 --> %2] %3: %4").arg(issueNumber_).arg(originatingIssue_->issueNumber()).arg(title_).arg(AMGitHubIssue::stringFromComplexity(complexityValue_));
	else
		retVal = QString("[%1 ==> %2] %3: %4").arg(issueNumber_).arg(originatingIssueNumber_).arg(title_).arg(AMGitHubIssue::stringFromComplexity(complexityValue_));

	if(!timeEstimateString_.isEmpty() || timeEstimateString_ == "Invalid Time Estimate")
		retVal.append(QString("{%1}").arg(timeEstimateString_));

//	if(!commentsURL_.isEmpty())
//		retVal.append(QString(" [@%1]").arg(commentsURL_));

	return retVal;
}

QString AMGitHubIssue::multiLineDebugInfo() const
{
	QString retVal;
	if(!isPullRequest_)
		retVal = QString("[%1]\n").arg(issueNumber_);
	else if(originatingIssue_)
		retVal = QString("[%1 --> %2]\n").arg(issueNumber_).arg(originatingIssue_->issueNumber());
	else
		retVal = QString("[%1 ==> %2]\n").arg(issueNumber_).arg(originatingIssueNumber_);

	retVal.append(QString("Title:\t\t\t%1\n").arg(title_));
	retVal.append(QString("Complexity:\t\t\t%1\n").arg(AMGitHubIssue::stringFromComplexity(complexityValue_)));
	if(!timeEstimateString_.isEmpty() || timeEstimateString_ == "Invalid Time Estimate")
		retVal.append(QString("Time Estimate:\t\t%1\n").arg(timeEstimateString_));

	return retVal;
}

AMGitHubIssueFamily::AMGitHubIssueFamily(AMGitHubIssue *originatingIssue, AMGitHubIssue *pullRequestIssue, QObject *parent) :
	QObject(parent)
{
	originatingIssue_ = originatingIssue;
	pullRequestIssue_ = pullRequestIssue;
}

AMGitHubIssueFamily::~AMGitHubIssueFamily()
{
}

int AMGitHubIssueFamily::originatingIssueNumber() const
{
	if(originatingIssue_)
		return originatingIssue_->issueNumber();
	return -1;
}

int AMGitHubIssueFamily::pullRequestIssueNumber() const
{
	if(pullRequestIssue_)
		return pullRequestIssue_->issueNumber();
	return -1;
}

AMGitHubIssue::ComplexityValue AMGitHubIssueFamily::estimatedComplexity() const
{
	if(originatingIssue_)
		return originatingIssue_->complexityValue();
	return AMGitHubIssue::InvalidComplexity;
}

AMGitHubIssue::ComplexityValue AMGitHubIssueFamily::actualComplexity() const
{
	if(pullRequestIssue_)
		return pullRequestIssue_->complexityValue();
	return AMGitHubIssue::InvalidComplexity;
}

QString AMGitHubIssueFamily::timeEstimate() const
{
	if(originatingIssue_)
		return originatingIssue_->timeEstimateString();
	return "Invalid Time Estimate";
}

QString AMGitHubIssueFamily::multiLineDebugInfo() const
{
	QString retVal;

	if(originatingIssue_)
		retVal.append(QString("Original:\t\t%1\n").arg(originatingIssueNumber()));
	else
		retVal.append(QString("Original:\t\tMISSING\n"));

	if(pullRequestIssue_)
		retVal.append(QString("Pull Request:\t%1\n").arg(pullRequestIssueNumber()));
	else
		retVal.append(QString("Pull Request:\tMISSING\n"));

	if(originatingIssue_ && estimatedComplexity() != AMGitHubIssue::InvalidComplexity)
		retVal.append(QString("Estimated Complexity:\t%1\n").arg(AMGitHubIssue::stringFromComplexity(estimatedComplexity())));
	else
		retVal.append(QString("Estimated Complexity:\tMISSING\n"));

	if(pullRequestIssue_ && actualComplexity() != AMGitHubIssue::InvalidComplexity)
		retVal.append(QString("Actual Complexity:\t%1\n").arg(AMGitHubIssue::stringFromComplexity(actualComplexity())));
	else
		retVal.append(QString("Actual Complexity:\tMISSING\n"));

	if(originatingIssue_ && timeEstimate() != "Invalid Time Estimated" && !timeEstimate().isEmpty())
		retVal.append(QString("Time Estimate:\t%1\n").arg(timeEstimate()));
	else
		retVal.append(QString("Time Estimate:\tMISSING\n"));

	return retVal;

}

bool AMGitHubIssueFamily::completeInfo() const
{
	if(originatingIssue_ && originatingIssue()->projectTrackingDisabled())
		return false;

	if(!originatingIssue_ || !pullRequestIssue_)
		return false;

	if(originatingIssue_ && estimatedComplexity() == AMGitHubIssue::InvalidComplexity)
		return false;

	if(pullRequestIssue_ && actualComplexity() == AMGitHubIssue::InvalidComplexity)
		return false;

	if(originatingIssue_ && (timeEstimate() == "Invalid Time Estimated" || timeEstimate().isEmpty()) )
		return false;

	return true;
}

bool AMGitHubIssueFamily::totallyIncomplete() const{
	if(originatingIssue_ && originatingIssue()->projectTrackingDisabled())
		return false;

	if(!pullRequestIssue_ && estimatedComplexity() == AMGitHubIssue::InvalidComplexity && (timeEstimate() == "Invalid Time Estimated" || timeEstimate().isEmpty()))
		return true;

	return false;
}

bool AMGitHubIssueFamily::onlyMissingActualComplexity() const{
	if(originatingIssue_ && originatingIssue()->projectTrackingDisabled())
		return false;

	if(originatingIssue_ && pullRequestIssue_ && (estimatedComplexity() != AMGitHubIssue::InvalidComplexity) && (timeEstimate() != "Invalid Time Estimated") && !timeEstimate().isEmpty() && actualComplexity() == AMGitHubIssue::InvalidComplexity)
		return true;
	return false;
}

bool AMGitHubIssueFamily::onlyMissingTimeEstimate() const{
	if(originatingIssue_ && originatingIssue()->projectTrackingDisabled())
		return false;

	if(originatingIssue_ && pullRequestIssue_ && (estimatedComplexity() != AMGitHubIssue::InvalidComplexity) && (timeEstimate() == "Invalid Time Estimated" || timeEstimate().isEmpty()) && (actualComplexity() != AMGitHubIssue::InvalidComplexity) )
		return true;
	return false;
}

AMGitHubIssueFamily::ComplexityMapping AMGitHubIssueFamily::complexityMapping() const{
	AMGitHubIssue::ComplexityValue localEstimatedComplexity = estimatedComplexity();
	AMGitHubIssue::ComplexityValue localActualComplexity = actualComplexity();

	switch(localEstimatedComplexity){
	case AMGitHubIssue::Complexity1:{
		switch(localActualComplexity){
		case AMGitHubIssue::Complexity1:
			return AMGitHubIssueFamily::Complexity1ToComplexity1;
		case AMGitHubIssue::Complexity2:
			return AMGitHubIssueFamily::Complexity1ToComplexity2;
		case AMGitHubIssue::Complexity3:
			return AMGitHubIssueFamily::Complexity1ToComplexity3;
		case AMGitHubIssue::Complexity5:
			return AMGitHubIssueFamily::Complexity1ToComplexity5;
		case AMGitHubIssue::Complexity8:
			return AMGitHubIssueFamily::Complexity1ToComplexity8;
		case AMGitHubIssue::ComplexityK:
			return AMGitHubIssueFamily::InvalidComplexityMapping;
		default:
			return AMGitHubIssueFamily::InvalidComplexityMapping;
		}
		break;}
	case AMGitHubIssue::Complexity2:{
		switch(localActualComplexity){
		case AMGitHubIssue::Complexity1:
			return AMGitHubIssueFamily::Complexity2ToComplexity1;
		case AMGitHubIssue::Complexity2:
			return AMGitHubIssueFamily::Complexity2ToComplexity2;
		case AMGitHubIssue::Complexity3:
			return AMGitHubIssueFamily::Complexity2ToComplexity3;
		case AMGitHubIssue::Complexity5:
			return AMGitHubIssueFamily::Complexity2ToComplexity5;
		case AMGitHubIssue::Complexity8:
			return AMGitHubIssueFamily::Complexity2ToComplexity8;
		case AMGitHubIssue::ComplexityK:
			return AMGitHubIssueFamily::InvalidComplexityMapping;
		default:
			return AMGitHubIssueFamily::InvalidComplexityMapping;
		}
		break;}
	case AMGitHubIssue::Complexity3:{
		switch(localActualComplexity){
		case AMGitHubIssue::Complexity1:
			return AMGitHubIssueFamily::Complexity3ToComplexity1;
		case AMGitHubIssue::Complexity2:
			return AMGitHubIssueFamily::Complexity3ToComplexity2;
		case AMGitHubIssue::Complexity3:
			return AMGitHubIssueFamily::Complexity3ToComplexity3;
		case AMGitHubIssue::Complexity5:
			return AMGitHubIssueFamily::Complexity3ToComplexity5;
		case AMGitHubIssue::Complexity8:
			return AMGitHubIssueFamily::Complexity3ToComplexity8;
		case AMGitHubIssue::ComplexityK:
			return AMGitHubIssueFamily::InvalidComplexityMapping;
		default:
			return AMGitHubIssueFamily::InvalidComplexityMapping;
		}
		break;}
	case AMGitHubIssue::Complexity5:{
		switch(localActualComplexity){
		case AMGitHubIssue::Complexity1:
			return AMGitHubIssueFamily::Complexity5ToComplexity1;
		case AMGitHubIssue::Complexity2:
			return AMGitHubIssueFamily::Complexity5ToComplexity2;
		case AMGitHubIssue::Complexity3:
			return AMGitHubIssueFamily::Complexity5ToComplexity3;
		case AMGitHubIssue::Complexity5:
			return AMGitHubIssueFamily::Complexity5ToComplexity5;
		case AMGitHubIssue::Complexity8:
			return AMGitHubIssueFamily::Complexity5ToComplexity8;
		case AMGitHubIssue::ComplexityK:
			return AMGitHubIssueFamily::InvalidComplexityMapping;
		default:
			return AMGitHubIssueFamily::InvalidComplexityMapping;
		}
		break;}
	case AMGitHubIssue::Complexity8:{
		switch(localActualComplexity){
		case AMGitHubIssue::Complexity1:
			return AMGitHubIssueFamily::Complexity8ToComplexity1;
		case AMGitHubIssue::Complexity2:
			return AMGitHubIssueFamily::Complexity8ToComplexity2;
		case AMGitHubIssue::Complexity3:
			return AMGitHubIssueFamily::Complexity8ToComplexity3;
		case AMGitHubIssue::Complexity5:
			return AMGitHubIssueFamily::Complexity8ToComplexity5;
		case AMGitHubIssue::Complexity8:
			return AMGitHubIssueFamily::Complexity8ToComplexity8;
		case AMGitHubIssue::ComplexityK:
			return AMGitHubIssueFamily::InvalidComplexityMapping;
		default:
			return AMGitHubIssueFamily::InvalidComplexityMapping;
		}
		break;}
	case AMGitHubIssue::ComplexityK:{
		switch(localActualComplexity){
		case AMGitHubIssue::Complexity1:
			return AMGitHubIssueFamily::ComplexityKToComplexity1;
		case AMGitHubIssue::Complexity2:
			return AMGitHubIssueFamily::ComplexityKToComplexity2;
		case AMGitHubIssue::Complexity3:
			return AMGitHubIssueFamily::ComplexityKToComplexity3;
		case AMGitHubIssue::Complexity5:
			return AMGitHubIssueFamily::ComplexityKToComplexity5;
		case AMGitHubIssue::Complexity8:
			return AMGitHubIssueFamily::ComplexityKToComplexity8;
		case AMGitHubIssue::ComplexityK:
			return AMGitHubIssueFamily::InvalidComplexityMapping;
		default:
			return AMGitHubIssueFamily::InvalidComplexityMapping;
		}
		break;}
	case AMGitHubIssue::InvalidComplexity:
		return AMGitHubIssueFamily::InvalidComplexityMapping;
	default:
		return AMGitHubIssueFamily::InvalidComplexityMapping;
	}
}

bool AMGitHubIssueFamily::symmetricComplexityMapping() const{
	if(complexityMapping() == AMGitHubIssueFamily::Complexity1ToComplexity1 || complexityMapping() == AMGitHubIssueFamily::Complexity2ToComplexity2 || complexityMapping() == AMGitHubIssueFamily::Complexity3ToComplexity3 || complexityMapping() == AMGitHubIssueFamily::Complexity5ToComplexity5 || complexityMapping() == AMGitHubIssueFamily::Complexity8ToComplexity8)
		return true;
	return false;
}

double AMGitHubIssueFamily::normalizedTimeEstiamte() const{
	QString timeEstimateFirstHalf = timeEstimate().section(' ', 0, 0);
	QString timeEstimateSecondHalf = timeEstimate().section(' ', 1, 1);

	bool convertsToDouble = false;
	double timeValue = timeEstimateFirstHalf.toDouble(&convertsToDouble);
	if(!convertsToDouble)
		return -1;

	double multiplier;
	QRegExp secondRX("[Ss]econd");
	QRegExp minuteRX("[Mm]inute");
	QRegExp hourRX("[Hh]our");
	QRegExp dayRX("[Dd]ay");
	QRegExp weekRX("[Ww]eek");
	QRegExp monthRX("[Mm]onth");
	if(timeEstimateSecondHalf.contains(secondRX))
		multiplier = 1.0/60.0/60.0;
	else if(timeEstimateSecondHalf.contains(minuteRX))
		multiplier = 1.0/60.0;
	else if(timeEstimateSecondHalf.contains(hourRX))
		multiplier = 1.0;
	else if(timeEstimateSecondHalf.contains(dayRX))
		multiplier = 1.0*8.0;
	else if(timeEstimateSecondHalf.contains(weekRX))
		multiplier = 1.0*8.0*5.0;
	else if(timeEstimateSecondHalf.contains(monthRX))
		multiplier = 1.0*8.0*5.0*4.0;
	else
		return -1;

	double retVal = timeValue*multiplier;
	if(retVal < 1.0)
		retVal = 1.0;
	return retVal;
}

#include <QFormLayout>
#include <QLabel>

AMGitHubIssueFamilyView::AMGitHubIssueFamilyView(AMGitHubIssueFamily *issueFamily, QWidget *parent) :
	QWidget(parent)
{
	issueFamily_ = issueFamily;
	if(issueFamily){
		QFormLayout *fl = new QFormLayout();

		if(issueFamily_->originatingIssue())
			fl->addRow("Original:", new QLabel(QString("%1").arg(issueFamily_->originatingIssueNumber())));
		else
			fl->addRow("Original:", new QLabel("MISSING"));

		if(issueFamily_->pullRequestIssue())
			fl->addRow("Pull Request:", new QLabel(QString("%1").arg(issueFamily_->pullRequestIssueNumber())));
		else
			fl->addRow("Pull Request:", new QLabel("MISSING"));

		if(issueFamily_->originatingIssue() && issueFamily_->estimatedComplexity() != AMGitHubIssue::InvalidComplexity)
			fl->addRow("Estimated Complexity:", new QLabel(AMGitHubIssue::stringFromComplexity(issueFamily_->estimatedComplexity())));
		else
			fl->addRow("Estimated Complexity:", new QLabel("MISSING"));

		if(issueFamily_->pullRequestIssue() && issueFamily_->actualComplexity() != AMGitHubIssue::InvalidComplexity)
			fl->addRow("Actual Complexity:", new QLabel(AMGitHubIssue::stringFromComplexity(issueFamily_->actualComplexity())));
		else
			fl->addRow("Actual Complexity:", new QLabel("MISSING"));

		if(issueFamily_->originatingIssue() && issueFamily_->timeEstimate() != "Invalid Time Estimated" && !issueFamily_->timeEstimate().isEmpty())
			fl->addRow("Time Estimate:", new QLabel(issueFamily_->timeEstimate()));
		else
			fl->addRow("Time Estimate:", new QLabel("MISSING"));

		if(issueFamily_->originatingIssue() && issueFamily_->originatingIssue()->issueState() == AMGitHubIssue::ClosedState)
			fl->addRow("State:", new QLabel("Closed"));
		else
			fl->addRow("State:", new QLabel("Open"));

		setLayout(fl);
	}
}

AMGitHubMilestone::AMGitHubMilestone(int number, const QString &title, AMGitHubMilestone::MilestoneState state, QObject *parent) :
	QObject(parent)
{
	number_ = number;
	title_ = title;
	state_ = state;
}

AMGitHubMilestone::AMGitHubMilestone(QVariantMap jsonMap, QObject *parent) :
	QObject(parent)
{
	if(jsonMap.value("number").canConvert<int>())
		number_ = jsonMap.value("number").toInt();
	else
		number_ = -1;

	if(jsonMap.value("title").canConvert<QString>())
		title_ = jsonMap.value("title").toString();
	else
		title_ = QString();

	if(jsonMap.value("state").canConvert<QString>()){
		QString stateAsString = jsonMap.value("state").toString();
		if(stateAsString == "open")
			state_ = AMGitHubMilestone::OpenState;
		else if(stateAsString == "closed")
			state_ = AMGitHubMilestone::ClosedState;
		else
			state_ = AMGitHubMilestone::InvalidState;
	}else
		state_ = AMGitHubMilestone::InvalidState;
}

QString AMGitHubMilestone::stateAsString() const{
	switch(state_){
	case AMGitHubMilestone::OpenState:
		return "Open";
	case AMGitHubMilestone::ClosedState:
		return "Closed";
	case AMGitHubMilestone::InvalidState:
		return "Invalid State";
	default:
		return "Invalid State";
	}
}

void AMGitHubMilestone::appendAssociatedIssue(AMGitHubIssue *associatedIssue){
	if(!associatedIssues_.contains(associatedIssue))
		associatedIssues_.append(associatedIssue);
}

void AMGitHubMilestone::appendAssociatedFamily(AMGitHubIssueFamily *associatedFamily){
	if(!associatedFamilies_.contains(associatedFamily))
		associatedFamilies_.append(associatedFamily);
}


AMGitHubComplexityManager::AMGitHubComplexityManager(QObject *parent) :
	QObject(parent)
{
	complexityMappingMatrix_ = QVector<int>(31, 0);
}

double AMGitHubComplexityManager::averageTimeForEstimatedComplexity(AMGitHubIssue::ComplexityValue estimatedComplexityValue){
	QList<double> localEstimatedComplexityTime;
	switch(estimatedComplexityValue){
	case AMGitHubIssue::Complexity1:
		localEstimatedComplexityTime = estimatedComplexity1Times_;
		break;
	case AMGitHubIssue::Complexity2:
		localEstimatedComplexityTime = estimatedComplexity2Times_;
		break;
	case AMGitHubIssue::Complexity3:
		localEstimatedComplexityTime = estimatedComplexity3Times_;
		break;
	case AMGitHubIssue::Complexity5:
		localEstimatedComplexityTime = estimatedComplexity5Times_;
		break;
	case AMGitHubIssue::Complexity8:
		localEstimatedComplexityTime = estimatedComplexity8Times_;
		break;
	case AMGitHubIssue::ComplexityK:
		localEstimatedComplexityTime = estimatedComplexityKTimes_;
		break;
	case AMGitHubIssue::InvalidComplexity:
		break;
	}

	if(localEstimatedComplexityTime.count() == 0)
		return 0;

	double retVal = 0;
	for(int x = 0, size = localEstimatedComplexityTime.count(); x < size; x++)
		retVal += localEstimatedComplexityTime.at(x);
	retVal = retVal/localEstimatedComplexityTime.count();
	return retVal;
}

double AMGitHubComplexityManager::averageTimeForActualComplexity(AMGitHubIssue::ComplexityValue actualComplexityValue){
	QList<double> localActualComplexityTime;
	switch(actualComplexityValue){
	case AMGitHubIssue::Complexity1:
		localActualComplexityTime = actualComplexity1Times_;
		break;
	case AMGitHubIssue::Complexity2:
		localActualComplexityTime = actualComplexity2Times_;
		break;
	case AMGitHubIssue::Complexity3:
		localActualComplexityTime = actualComplexity3Times_;
		break;
	case AMGitHubIssue::Complexity5:
		localActualComplexityTime = actualComplexity5Times_;
		break;
	case AMGitHubIssue::Complexity8:
		localActualComplexityTime = actualComplexity8Times_;
		break;
	case AMGitHubIssue::ComplexityK:
		return 0;
//		localActualComplexityTime = actualComplexityKTimes_;
//		break;
	case AMGitHubIssue::InvalidComplexity:
		return 0;
	}

	if(localActualComplexityTime.count() == 0)
		return 0;

	double retVal = 0;
	for(int x = 0, size = localActualComplexityTime.count(); x < size; x++)
		retVal += localActualComplexityTime.at(x);
	retVal = retVal/localActualComplexityTime.count();
	return retVal;
}

double AMGitHubComplexityManager::probableTimeForEstimatedComplexity(AMGitHubIssue::ComplexityValue estimatedComplexityValue){
	int localTotalEstimatedComplexityIssues = 0;
	int indexStartValue;

	switch(estimatedComplexityValue){
	case AMGitHubIssue::Complexity1:
		indexStartValue = 1;
		break;
	case AMGitHubIssue::Complexity2:
		indexStartValue = 6;
		break;
	case AMGitHubIssue::Complexity3:
		indexStartValue = 11;
		break;
	case AMGitHubIssue::Complexity5:
		indexStartValue = 16;
		break;
	case AMGitHubIssue::Complexity8:
		indexStartValue = 21;
		break;
	case AMGitHubIssue::ComplexityK:
		return -1;
//		indexStartValue = 26;
//		break;
	case AMGitHubIssue::InvalidComplexity:
		return 0;
	}

	for(int x = indexStartValue, size = indexStartValue+5; x < size; x++)
		localTotalEstimatedComplexityIssues += complexityMappingMatrix_.at(x);

	double retVal = 0;
	retVal = averageTimeForActualComplexity(AMGitHubIssue::Complexity1)*complexityMappingMatrix_.at(indexStartValue)/localTotalEstimatedComplexityIssues
			+ averageTimeForActualComplexity(AMGitHubIssue::Complexity2)*complexityMappingMatrix_.at(indexStartValue+1)/localTotalEstimatedComplexityIssues
			+ averageTimeForActualComplexity(AMGitHubIssue::Complexity3)*complexityMappingMatrix_.at(indexStartValue+2)/localTotalEstimatedComplexityIssues
			+ averageTimeForActualComplexity(AMGitHubIssue::Complexity5)*complexityMappingMatrix_.at(indexStartValue+3)/localTotalEstimatedComplexityIssues
			+ averageTimeForActualComplexity(AMGitHubIssue::Complexity8)*complexityMappingMatrix_.at(indexStartValue+4)/localTotalEstimatedComplexityIssues;

	return retVal;
}

double AMGitHubComplexityManager::outstandingEstimateAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date){
	if(!issueFamily || !issueFamily->originatingIssue() || !issueFamily->originatingIssue()->createdDate().isValid())
		return 0;

	AMGitHubIssue *originatingIssue = issueFamily->originatingIssue();
	if(date < originatingIssue->createdDate())
		return 0;

	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) )
		return 0;

	return probableTimeForEstimatedComplexity(issueFamily->estimatedComplexity());
}

double AMGitHubComplexityManager::completedEstimateAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date){
	if(!issueFamily || !issueFamily->originatingIssue() || !issueFamily->originatingIssue()->createdDate().isValid())
		return 0;

	AMGitHubIssue *originatingIssue = issueFamily->originatingIssue();
	if(date < originatingIssue->createdDate())
		return 0;

//	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) && (issueFamily->actualComplexity() != AMGitHubIssue::InvalidComplexity) )
//		return averageTimeForActualComplexity(issueFamily->actualComplexity());

	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) )
		return probableTimeForEstimatedComplexity(issueFamily->estimatedComplexity());

	return 0;
}

double AMGitHubComplexityManager::withdrawnEstimateAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date){
	if(!issueFamily || !issueFamily->originatingIssue() || !issueFamily->originatingIssue()->createdDate().isValid())
		return 0;

	AMGitHubIssue *originatingIssue = issueFamily->originatingIssue();
	if(date < originatingIssue->createdDate())
		return 0;

	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) && !issueFamily->pullRequestIssue() && !originatingIssue->inlineIssue() )
		return probableTimeForEstimatedComplexity(issueFamily->estimatedComplexity());

	return 0;
}

double AMGitHubComplexityManager::complexityMappedCompletedWorkAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date){
	if(!issueFamily || !issueFamily->originatingIssue() || !issueFamily->originatingIssue()->createdDate().isValid())
		return 0;

	AMGitHubIssue *originatingIssue = issueFamily->originatingIssue();
	if(date < originatingIssue->createdDate())
		return 0;

	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) && (issueFamily->actualComplexity() != AMGitHubIssue::InvalidComplexity) )
		return averageTimeForActualComplexity(issueFamily->actualComplexity());

	return 0;
}

double AMGitHubComplexityManager::reportedCompletedWorkAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date){
	if(!issueFamily || !issueFamily->originatingIssue() || !issueFamily->originatingIssue()->createdDate().isValid())
		return 0;

	AMGitHubIssue *originatingIssue = issueFamily->originatingIssue();
	if(date < originatingIssue->createdDate())
		return 0;

	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) && (issueFamily->actualComplexity() != AMGitHubIssue::InvalidComplexity) ){
		if(issueFamily->normalizedTimeEstiamte() < 0)
			return 0;
		return issueFamily->normalizedTimeEstiamte();
	}

	return 0;
}

void AMGitHubComplexityManager::incrementComplexityMapping(AMGitHubIssueFamily::ComplexityMapping complexityMapping){
	complexityMappingMatrix_[complexityMapping] = complexityMappingMatrix_.at(complexityMapping)+1;
}

void AMGitHubComplexityManager::clearComplexityMappings(){
	complexityMappingMatrix_ = QVector<int>(31, 0);;
}

void AMGitHubComplexityManager::addTimeEstimateForEstimatedComplexity(double normalizedTimeEstimate, AMGitHubIssue::ComplexityValue complexityValue){
	if(normalizedTimeEstimate <= 0)
		return;

	switch(complexityValue){
	case AMGitHubIssue::Complexity1:
		estimatedComplexity1Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::Complexity2:
		estimatedComplexity2Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::Complexity3:
		estimatedComplexity3Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::Complexity5:
		estimatedComplexity5Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::Complexity8:
		estimatedComplexity8Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::ComplexityK:
		estimatedComplexityKTimes_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::InvalidComplexity:
		break;
	}
}

void AMGitHubComplexityManager::addTimeEstimateForActualComplexity(double normalizedTimeEstimate, AMGitHubIssue::ComplexityValue complexityValue){
	if(normalizedTimeEstimate <= 0)
		return;

	switch(complexityValue){
	case AMGitHubIssue::Complexity1:
		actualComplexity1Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::Complexity2:
		actualComplexity2Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::Complexity3:
		actualComplexity3Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::Complexity5:
		actualComplexity5Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::Complexity8:
		actualComplexity8Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::ComplexityK:
//		actualComplexityKTimes_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::InvalidComplexity:
		break;
	}
}
