#include "AMGoogleAuthTokenRequest.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringList>

#include "source/qjson/parser.h"
AMGoogleAuthTokenRequest::AMGoogleAuthTokenRequest(const QString& clientId,
												   const QString& clientSecret,
												   int redirectPort,
												   const QString& scope,
												   const QString& endpoint,
												   QObject *parent ) :
    QObject(parent)
{

	endPoint_ = endpoint;
	clientId_ = clientId;
	clientSecret_ = clientSecret;
	redirectPort_ = redirectPort;
	scope_ = scope;
	manager_ = new QNetworkAccessManager();
	connect(manager_, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetworkReplyReceived(QNetworkReply*)));
	codeListenSocket_ = 0;

	codeListenServer_ = new QTcpServer();
	codeListenServer_->listen(QHostAddress::Any, redirectPort);
	connect(codeListenServer_, SIGNAL(newConnection()), this, SLOT(onNewCodeServerConnection()));
}

AMGoogleAuthTokenRequest::~AMGoogleAuthTokenRequest()
{
	if(codeListenSocket_) {
		codeListenSocket_->close();
		codeListenSocket_->deleteLater();
	}

	manager_->deleteLater();
	codeListenServer_->close();
	codeListenServer_->deleteLater();
}

void AMGoogleAuthTokenRequest::sendRequest()
{
	QUrl codeRequestUrl = QUrl(QString("%1?response_type=code&client_id=%3&redirect_uri=http://localhost:%4&scope=%5")
						  .arg(endPoint_)
						  .arg(clientId_)
						  .arg(redirectPort_)
						  .arg(scope_));

	manager_->get(QNetworkRequest(codeRequestUrl));
}

void AMGoogleAuthTokenRequest::onNetworkReplyReceived(QNetworkReply *reply)
{
	QString replyString(reply->readAll());

	if(replyString.startsWith("<HTML>")) {
		emit authorizationPageToDisplay(replyString);
	} else {
		QJson::Parser replyParser;
		bool parseOkay = false;
		QVariant parsedReply = replyParser.parse(replyString.toAscii(), &parseOkay);
		if(parseOkay && parsedReply.canConvert(QVariant::Map)){
			QVariantMap jsonMap = parsedReply.toMap();
			QVariant token = jsonMap.value("access_token");
			QVariant secsToExpiration = jsonMap.value("expires_in");
			QVariant refreshToken = jsonMap.value("refresh_token");

			emit tokenReceived(token.toString(),
							   secsToExpiration.toInt(),
							   refreshToken.toString());
		}
	}
}

void AMGoogleAuthTokenRequest::onNewCodeServerConnection()
{
	if(codeListenSocket_ != 0) {
		codeListenSocket_->deleteLater();
	}

	codeListenSocket_ = codeListenServer_->nextPendingConnection();

	connect(codeListenSocket_, SIGNAL(readyRead()), this, SLOT(onCodeReadyToRead()));
}

void AMGoogleAuthTokenRequest::onCodeReadyToRead()
{
	// Obtain the provided code, and parse it:
	QString processedGetResponse = codeListenSocket_->readAll();
	int indexOfCode = processedGetResponse.indexOf("/?code=");
	processedGetResponse = processedGetResponse.mid(indexOfCode + 7).split("\n").at(0).split(" ").at(0);
	code_ = processedGetResponse;

	// Make a post request to exchange the code for a token
	QUrl postUrl = QUrl("https://accounts.google.com/o/oauth2/token");
	QUrl params;
	params.addQueryItem("client_id", clientId_);
	params.addQueryItem("client_secret", clientSecret_);
	params.addQueryItem("code", code_);
	params.addQueryItem("redirect_uri", QString("http://localhost:%1").arg(redirectPort_));
	params.addQueryItem("grant_type", "authorization_code");

	QNetworkRequest postRequest(postUrl);
	postRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

	manager_->post(postRequest, params.encodedQuery());
}
