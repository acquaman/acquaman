#include "AMGitHubIssue.h"

#include <QRegExp>
#include <QDebug>

#include "qjson/serializer.h"
#include "qjson/parser.h"

AMGitHubIssue::AMGitHubIssue(QVariantMap jsonMap, QObject *parent) :
	QObject(parent)
{
	issueNumber_ = -1;
	actualComplexityValue_ = AMGitHubIssue::ActualComplexityInvalid;
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
	complexityMapping_ = 0;
	estimatedComplexityValue_ = AMGitHubIssue::EstimatedComplexityInvalid;

	normalizedTimeEstimate_ = -2;

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
				actualComplexityValue_ = actualComplexityFromString(labelMap.value("name").toString());
			if(labelMap.contains("name") && labelMap.value("name").toString().contains("Project Tracking Disabled"))
				projectTrackingDisabled_ = true;
			if(labelMap.contains("name") && labelMap.value("name").toString().contains("Project Tracking Inline Issue"))
				inlineIssue_ = true;

			if(labelMap.contains("name"))
				rawLabels_.append(labelMap.value("name").toString());
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

	complexityMapping_ = new AMGitHubComplexityMapping(this);
}

AMGitHubIssue::~AMGitHubIssue()
{
}

QVariantMap AMGitHubIssue::toMap() const
{
	QVariantMap retVal;

	retVal.insert("issueNumber", issueNumber_);
	retVal.insert("title", title_);
	retVal.insert("assignee", assignee_);
	retVal.insert("actualComplexity", actualComplexityValue_);
	retVal.insert("estimatedComplexity", estimatedComplexityValue_);
	retVal.insert("timeEstimateString", timeEstimateString_);
	retVal.insert("isPullRequest", isPullRequest_);
	retVal.insert("originatingIssueNumber", originatingIssueNumber_);
	retVal.insert("projectTrackingDisabled", projectTrackingDisabled_);
	retVal.insert("inlineIssue", inlineIssue_);
	retVal.insert("issueState", issueState_);
	retVal.insert("createdDate", createdDate_);
	retVal.insert("closedDate", closedDate_);

	QVariantList rawLabelsVariantList;
	for(int x = 0, size = rawLabels_.count(); x < size; x++)
		rawLabelsVariantList.append(rawLabels_.at(x));
	if(rawLabelsVariantList.count() > 0)
		retVal.insert("rawLabels", rawLabelsVariantList);

	return retVal;
}

QByteArray AMGitHubIssue::toJSON() const
{
	QVariantMap asMap = toMap();

	QJson::Serializer jserializer;
	QByteArray retVal = jserializer.serialize(asMap);
	return retVal;
}

bool AMGitHubIssue::validActualComplexity() const
{
	if(actualComplexityValue_ != AMGitHubIssue::ActualComplexityInvalid)
		return true;
	return false;
}

bool AMGitHubIssue::validEstimatedComplexity() const
{
	if(estimatedComplexityValue_ != AMGitHubIssue::EstimatedComplexityInvalid)
		return true;
	return false;
}

bool AMGitHubIssue::issueCompletelyUntracked() const
{
	if(!isPullRequest_ && projectTrackingDisabled())
		return true;
	return false;
}

bool AMGitHubIssue::issueTrackedWithoutEstimates() const
{
	if(isPullRequest_ && projectTrackingDisabled())
		return true;
	return false;
}

bool AMGitHubIssue::fullyTrackedIssue() const
{
	return !issueCompletelyUntracked() && !issueTrackedWithoutEstimates();
}

bool AMGitHubIssue::completeIssue() const
{
	if(!complexityMapping_->validMapping())
		return false;
	if(normalizedTimeEstimate() < 0)
		return false;
	return true;
}

bool AMGitHubIssue::issueMissingEstimateComplexity() const
{
	if(isOpen() && !validEstimatedComplexity())
		return true;
	if(isClosed() && !validEstimatedComplexity() && !issueCompletelyUntracked() && !issueTrackedWithoutEstimates())
		return true;
	return false;
}

bool AMGitHubIssue::issueMissingActualComplexity() const
{
	if(isOpen())
		return false;
	if(isClosed() && !validActualComplexity() && !issueCompletelyUntracked() && !issueTrackedWithoutEstimates())
		return true;
	return false;
}

bool AMGitHubIssue::issueMissingTimeReporting() const
{
	if(isOpen())
		return false;
	if(isClosed() && (normalizedTimeEstimate() < 0)  && !issueCompletelyUntracked() && !issueTrackedWithoutEstimates())
		return true;
	return false;
}

bool AMGitHubIssue::issueFullySpecifiedForState() const
{
	if(isOpen() && validEstimatedComplexity())
		return true;
	if(isClosed() && (completeIssue() || issueTrackedWithoutEstimates()) )
		return true;
	return false;
}

AMGitHubIssue::ActualComplexityValue AMGitHubIssue::actualComplexityFromString(const QString &actualComplexityString)
{
	if(actualComplexityString == "Complexity1")
		return AMGitHubIssue::ActualComplexity1;
	else if(actualComplexityString == "Complexity2")
		return AMGitHubIssue::ActualComplexity2;
	else if(actualComplexityString == "Complexity3")
		return AMGitHubIssue::ActualComplexity3;
	else if(actualComplexityString == "Complexity5")
		return AMGitHubIssue::ActualComplexity5;
	else if(actualComplexityString == "Complexity8")
		return AMGitHubIssue::ActualComplexity8;
	else if(actualComplexityString == "Complexity13")
		return AMGitHubIssue::ActualComplexity13;
	else if(actualComplexityString == "Complexity21")
		return AMGitHubIssue::ActualComplexity21;
	else if(actualComplexityString == "Complexity40")
		return AMGitHubIssue::ActualComplexity40;
	else
		return AMGitHubIssue::ActualComplexityInvalid;
}

QString AMGitHubIssue::stringFromActualComplexity(AMGitHubIssue::ActualComplexityValue complexityValue){
	switch(complexityValue){
	case AMGitHubIssue::ActualComplexity1:
		return "Complexity1";
	case AMGitHubIssue::ActualComplexity2:
		return "Complexity2";
	case AMGitHubIssue::ActualComplexity3:
		return "Complexity3";
	case AMGitHubIssue::ActualComplexity5:
		return "Complexity5";
	case AMGitHubIssue::ActualComplexity8:
		return "Complexity8";
	case AMGitHubIssue::ActualComplexity13:
		return "Complexity13";
	case AMGitHubIssue::ActualComplexity21:
		return "Complexity21";
	case AMGitHubIssue::ActualComplexity40:
		return "Complexity40";
	case AMGitHubIssue::ActualComplexityInvalid:
		return "Invalid Complexity";
	default:
		return "Invalid Complexity";
	}
}

int AMGitHubIssue::integerFromActualComplexity(AMGitHubIssue::ActualComplexityValue complexityValue){
	switch(complexityValue){
	case AMGitHubIssue::ActualComplexity1:
		return 1;
	case AMGitHubIssue::ActualComplexity2:
		return 2;
	case AMGitHubIssue::ActualComplexity3:
		return 3;
	case AMGitHubIssue::ActualComplexity5:
		return 5;
	case AMGitHubIssue::ActualComplexity8:
		return 8;
	case AMGitHubIssue::ActualComplexity13:
		return 13;
	case AMGitHubIssue::ActualComplexity21:
		return 21;
	case AMGitHubIssue::ActualComplexity40:
		return 40;
	default:
		return -1;
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

AMGitHubIssue::EstimatedComplexityValue AMGitHubIssue::correspondingEstimatedComplexity(AMGitHubIssue::ActualComplexityValue actualComplexity)
{
	int actualAsInt = int(actualComplexity);
	AMGitHubIssue::EstimatedComplexityValue retVal = AMGitHubIssue::EstimatedComplexityValue(actualAsInt);
	return retVal;
}

AMGitHubIssue::ActualComplexityValue AMGitHubIssue::correspondingActualComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexity)
{
	int estimateAsInt = int(estimatedComplexity);
	AMGitHubIssue::ActualComplexityValue retVal = AMGitHubIssue::ActualComplexityValue(estimateAsInt);
	return retVal;
}

void AMGitHubIssue::setTimeEstimateString(const QString &timeEstimateString)
{
	timeEstimateString_ = timeEstimateString;
	normalizedTimeEstimate_ = -2;
}

double AMGitHubIssue::normalizedTimeEstimate() const{
	if(normalizedTimeEstimate_ != -2)
		return normalizedTimeEstimate_;

	QString timeEstimateFirstHalf = timeEstimateString().section(' ', 0, 0);
	QString timeEstimateSecondHalf = timeEstimateString().section(' ', 1, 1);

	bool convertsToDouble = false;
	double timeValue = timeEstimateFirstHalf.toDouble(&convertsToDouble);
	if(!convertsToDouble){
		normalizedTimeEstimate_ = -1;
		return normalizedTimeEstimate_;
	}

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
	else{
		normalizedTimeEstimate_ = -1;
		return normalizedTimeEstimate_;
	}

	normalizedTimeEstimate_ = timeValue*multiplier;
	if(normalizedTimeEstimate_ < 1.0/60.0)
		normalizedTimeEstimate_ = 1.0/60.0;
	return normalizedTimeEstimate_;
}

QString AMGitHubIssue::oneLineDebugInfo() const
{
	QString retVal;
	if(!isPullRequest_)
		retVal = QString("[%1] %2: %3").arg(issueNumber_).arg(title_).arg(AMGitHubIssue::stringFromActualComplexity(actualComplexityValue_));
	else if(originatingIssue_)
		retVal = QString("[%1 --> %2] %3: %4").arg(issueNumber_).arg(originatingIssue_->issueNumber()).arg(title_).arg(AMGitHubIssue::stringFromActualComplexity(actualComplexityValue_));
	else
		retVal = QString("[%1 ==> %2] %3: %4").arg(issueNumber_).arg(originatingIssueNumber_).arg(title_).arg(AMGitHubIssue::stringFromActualComplexity(actualComplexityValue_));

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
	retVal.append(QString("Complexity:\t\t\t%1\n").arg(AMGitHubIssue::stringFromActualComplexity(actualComplexityValue_)));
	if(!timeEstimateString_.isEmpty() || timeEstimateString_ == "Invalid Time Estimate")
		retVal.append(QString("Time Estimate:\t\t%1\n").arg(timeEstimateString_));

	return retVal;
}

AMGitHubIssue::EstimatedComplexityValue AMGitHubIssue::estimatedComplexityFromInteger(int estimatedComplexityAsInteger)
{
	switch(estimatedComplexityAsInteger){
	case 1:
		return AMGitHubIssue::EstimatedComplexity1;
	case 2:
		return AMGitHubIssue::EstimatedComplexity2;
	case 3:
		return AMGitHubIssue::EstimatedComplexity3;
	case 5:
		return AMGitHubIssue::EstimatedComplexity5;
	case 8:
		return AMGitHubIssue::EstimatedComplexity8;
	case 13:
		return AMGitHubIssue::EstimatedComplexity13;
	case 21:
		return AMGitHubIssue::EstimatedComplexity21;
	case 40:
		return AMGitHubIssue::EstimatedComplexity40;
	default:
		return AMGitHubIssue::EstimatedComplexityInvalid;
	}
}

int AMGitHubIssue::integerFromEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue zenhubComplexityValue)
{
	switch(zenhubComplexityValue){
	case AMGitHubIssue::EstimatedComplexity1:
		return 1;
	case AMGitHubIssue::EstimatedComplexity2:
		return 2;
	case AMGitHubIssue::EstimatedComplexity3:
		return 3;
	case AMGitHubIssue::EstimatedComplexity5:
		return 5;
	case AMGitHubIssue::EstimatedComplexity8:
		return 8;
	case AMGitHubIssue::EstimatedComplexity13:
		return 13;
	case AMGitHubIssue::EstimatedComplexity21:
		return 21;
	case AMGitHubIssue::EstimatedComplexity40:
		return 40;
	case AMGitHubIssue::EstimatedComplexityInvalid:
	default:
		return -1;
	}
}

void AMGitHubIssue::setActualComplexityValue(AMGitHubIssue::ActualComplexityValue actualComplexityValue)
{
	actualComplexityValue_ = actualComplexityValue;
	complexityMapping_->deleteLater();
	complexityMapping_ = new AMGitHubComplexityMapping(this);
}

void AMGitHubIssue::setEstimatedComplexityValue(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue)
{
	estimatedComplexityValue_ = estimatedComplexityValue;
	complexityMapping_->deleteLater();
	complexityMapping_ = new AMGitHubComplexityMapping(this);
}

void AMGitHubIssue::resetFromMap(const QVariantMap &map)
{
	if(map.contains("issueNumber"))
		issueNumber_ = map.value("issueNumber").toInt();
	if(map.contains("title"))
		title_ = map.value("title").toString();
	if(map.contains("assignee"))
		assignee_ = map.value("assignee").toString();
	if(map.contains("actualComplexity"))
		actualComplexityValue_ = AMGitHubIssue::ActualComplexityValue(map.value("actualComplexity").toLongLong());
	if(map.contains("estimatedComplexity"))
		estimatedComplexityValue_ = AMGitHubIssue::EstimatedComplexityValue(map.value("estimatedComplexity").toLongLong());

	if(map.contains("timeEstimateString")){
		timeEstimateString_ = map.value("timeEstimateString").toString();
		normalizedTimeEstimate_ = -2;
	}
	if(map.contains("isPullRequest"))
		isPullRequest_ = map.value("isPullRequest").toBool();
	if(map.contains("originatingIssueNumber"))
		originatingIssueNumber_ = map.value("originatingIssueNumber").toInt();
	if(map.contains("projectTrackingDisabled"))
		projectTrackingDisabled_ = map.value("projectTrackingDisabled").toBool();
	if(map.contains("inlineIssue"))
		inlineIssue_ = map.value("inlineIssue").toBool();
	if(map.contains("issueState"))
		issueState_ = AMGitHubIssue::IssueState(map.value("issueState").toLongLong());
	if(map.contains("createdDate"))
		createdDate_ = map.value("createdDate").toDateTime();
	if(map.contains("closedDate"))
		closedDate_ = map.value("closedDate").toDateTime();

	if(map.contains("rawLabels")){
		QVariantList rawLabelsList = map.value("rawLabels").toList();

		rawLabels_.clear();
		for(int x = 0, size = rawLabelsList.count(); x < size; x++)
			rawLabels_.append(rawLabelsList.at(x).toString());
	}

	complexityMapping_->deleteLater();
	complexityMapping_ = new AMGitHubComplexityMapping(this);
}

void AMGitHubIssue::resetFromJSON(const QByteArray &json)
{
	QJson::Parser jparser;
	bool successfulParsing = false;
	QVariant parseVariant = jparser.parse(json, &successfulParsing);
	if(successfulParsing && parseVariant.canConvert<QVariantMap>()){
		QVariantMap parseVariantMap = parseVariant.toMap();
		resetFromMap(parseVariantMap);
	}
	else
		qDebug() << "[AMGitHubIssue] Failed to parse JSON in resetFromJSON";
}

AMGitHubComplexityMapping::AMGitHubComplexityMapping(int mappingIndex, QObject *parent) :
	QObject(parent)
{
	if(validMappingIndex(mappingIndex)){
		validMapping_ = true;
		actualComplexityValue_ = AMGitHubComplexityMapping::actualComplexityValueFromMappingIndex(mappingIndex);
		estimatedComplexityValue_ = AMGitHubComplexityMapping::estimatedComplexityValueFromMappingIndex(mappingIndex);
	}
	else{
		validMapping_ = false;
		actualComplexityValue_ = AMGitHubIssue::ActualComplexityInvalid;
		estimatedComplexityValue_ = AMGitHubIssue::EstimatedComplexityInvalid;
	}
}

AMGitHubComplexityMapping::AMGitHubComplexityMapping(AMGitHubIssue *issue, QObject *parent) :
	QObject(parent)
{
	if(validComplexityValues(issue->actualComplexityValue(), issue->estimatedComplexityValue())){
		validMapping_ = true;
		actualComplexityValue_ = issue->actualComplexityValue();
		estimatedComplexityValue_ = issue->estimatedComplexityValue();
	}
	else{
		validMapping_ = false;
		actualComplexityValue_ = AMGitHubIssue::ActualComplexityInvalid;
		estimatedComplexityValue_ = AMGitHubIssue::EstimatedComplexityInvalid;
	}
}

AMGitHubComplexityMapping::AMGitHubComplexityMapping(AMGitHubIssue::ActualComplexityValue actualComplexityValue, AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue, QObject *parent) :
	QObject(parent)
{
	if(validComplexityValues(actualComplexityValue, estimatedComplexityValue)){
		validMapping_ = true;
		actualComplexityValue_ = actualComplexityValue;
		estimatedComplexityValue_ = estimatedComplexityValue;
	}
	else{
		validMapping_ = false;
		actualComplexityValue_ = AMGitHubIssue::ActualComplexityInvalid;
		estimatedComplexityValue_ = AMGitHubIssue::EstimatedComplexityInvalid;
	}
}

int AMGitHubComplexityMapping::mappingIndex() const
{
	if(validMapping_){
		int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid);
		int indexActual = int(actualComplexityValue_);
		int indexEstimate = int(estimatedComplexityValue_);

		return maxEstimate*indexEstimate + indexActual;
	}
	return -1;
}

bool AMGitHubComplexityMapping::symmetricComplexityMapping() const
{
	if(!validMapping())
		return false;
	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid);
	int columnValue = mappingIndex()%maxEstimate;
	int rowValue = mappingIndex()/maxEstimate;
	if(columnValue == rowValue)
		return true;
	return false;
}

AMGitHubIssue::ActualComplexityValue AMGitHubComplexityMapping::actualComplexityValueFromMappingIndex(int index)
{
	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid);
	int asInt = index%maxEstimate;
	return AMGitHubIssue::ActualComplexityValue(asInt);
}

AMGitHubIssue::EstimatedComplexityValue AMGitHubComplexityMapping::estimatedComplexityValueFromMappingIndex(int index)
{
	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid);
	int asInt = index/maxEstimate;
	return AMGitHubIssue::EstimatedComplexityValue(asInt);
}

bool AMGitHubComplexityMapping::validMappingIndex(int mappingIndex)
{
	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid);
	int maxActual = int(AMGitHubIssue::ActualComplexityInvalid);
	if(mappingIndex < 0 || (mappingIndex >= maxEstimate*maxActual) )
		return false;
	return true;
}

bool AMGitHubComplexityMapping::validComplexityValues(AMGitHubIssue::ActualComplexityValue actualComplexityValue, AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue)
{
	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid);
	int maxActual = int(AMGitHubIssue::ActualComplexityInvalid);

	int actualAsInt = int(actualComplexityValue);
	int estimateAsInt = int(estimatedComplexityValue);

	if(actualAsInt < 0 || actualAsInt >= maxActual)
		return false;
	if(estimateAsInt < 0 || estimateAsInt >= maxEstimate)
		return false;
	return true;
}

int AMGitHubComplexityMapping::maximumIndex()
{
	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid);
	int maxActual = int(AMGitHubIssue::ActualComplexityInvalid);

	return maxEstimate*maxActual-1;
}
