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

	switch(actionInfo->contentType()){
	case AMRestActionInfo::JSONContent:
		break;
	case AMRestActionInfo::FormURLEncoded:
		request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
		break;
	}

//	QVariantList jdata;
//	jdata.append(QString("repo=acquaman&issue_number=1462&estimate_value=3&organization=acquaman"));
//	QJson::Serializer jserializer;
////	QByteArray jsonData = jserializer.serialize(jdata);
//	QByteArray jsonData = QString("repo=acquaman&issue_number=1458&estimate_value=3&organization=acquaman").toAscii();

	QBuffer *buffer = new QBuffer;
	buffer->setData(actionInfo->requestData());
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
	case AMRestActionInfo::PutRequest:
		restReply_ = networkAccessManager_->put(request, buffer);
	case AMRestActionInfo::DeleteRequest:
		restReply_ = networkAccessManager_->sendCustomRequest(request, "DELETE");
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
