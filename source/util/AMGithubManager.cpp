#include "AMGithubManager.h"

#include <QDebug>

AMGithubManager::AMGithubManager(QObject *parent) :
	QObject(parent)
{
	initialize();
}

AMGithubManager::AMGithubManager(const QString &userName, const QString &password, const QString &repository, QObject *parent) :
	QObject(parent)
{
	initialize();
	userName_ = userName;
	password_ = password;
	repository_ = repository;
	authenticate();
}

QString AMGithubManager::userName() const{
	return userName_;
}

bool AMGithubManager::isAuthenticated() const{
	return authenticated_;
}

QString AMGithubManager::repository() const{
	return repository_;
}

void AMGithubManager::setUserName(const QString &userName){
	userName_ = userName;
}

void AMGithubManager::setPassword(const QString &password){
	password_ = password;
}

void AMGithubManager::setRepository(const QString &repository){
	repository_ = repository;
}

void AMGithubManager::authenticate(){
	if(authenticateReply_)
		return;
	QNetworkRequest request;
	QString authenticateURL = "https://api.github.com/user";

	request.setUrl(QUrl(authenticateURL));

	QString userInfo = userName_+":"+password_;
	QByteArray userData = userInfo.toLocal8Bit().toBase64();
	QString headerData = "Basic " + userData;
	request.setRawHeader("Authorization", headerData.toLocal8Bit());

	authenticateReply_ = manager_->get(request);
	connect(authenticateReply_, SIGNAL(readyRead()), this, SLOT(onAuthenicatedRequestReturned()));
}

void AMGithubManager::getIssues(AMGithubManager::IssuesFilter filter, AMGithubManager::IssuesState state, AMGithubManager::IssuesSort sort, AMGithubManager::IssuesDirection direction){
	if(!isAuthenticated() || repository_.isEmpty() || getIssuesReply_)
		return;
	QNetworkRequest request;

	QString issuesURL = "https://api.github.com/repos/"+userName_+"/"+repository_+"/issues";
	QString issuesOptions = "?";
	switch(filter){
	issuesOptions.append("filter=");
	case AMGithubManager::IssuesFilterAssigned:
		issuesOptions.append("assigned&");
		break;
	case AMGithubManager::IssuesFilterCreated:
		issuesOptions.append("created&");
		break;
	case AMGithubManager::IssuesFilterMentioned:
		issuesOptions.append("mentioned&");
		break;
	case AMGithubManager::IssuesFilterSubscribed:
		issuesOptions.append("subscribed&");
		break;
	}
	switch(state){
	issuesOptions.append("state=");
	case AMGithubManager::IssuesStateOpen:
		issuesOptions.append("open&");
		break;
	case AMGithubManager::IssuesStateClosed:
		issuesOptions.append("closed&");
		break;
	}
	switch(sort){
	issuesOptions.append("sort=");
	case AMGithubManager::IssuesSortCreated:
		issuesOptions.append("created&");
		break;
	case AMGithubManager::IssuesSortUpdated:
		issuesOptions.append("updated&");
		break;
	case AMGithubManager::IssuesSortComments:
		issuesOptions.append("comments&");
		break;
	}
	switch(direction){
	issuesOptions.append("direction=");
	case AMGithubManager::IssuesDirectionAscending:
		issuesOptions.append("asc");
		break;
	case AMGithubManager::IssuesDirectionDescending:
		issuesOptions.append("desc");
		break;
	}
	request.setUrl(QUrl(issuesURL+issuesOptions));

	QString userInfo = userName_+":"+password_;
	QByteArray userData = userInfo.toLocal8Bit().toBase64();
	QString headerData = "Basic " + userData;
	request.setRawHeader("Authorization", headerData.toLocal8Bit());

	getIssuesReply_ = manager_->get(request);
	connect(getIssuesReply_, SIGNAL(readyRead()), this, SLOT(onIssuesReturned()));
}

void AMGithubManager::createNewIssue(const QString &title, const QString &body, const QString &assignee){
	if(!isAuthenticated() || repository_.isEmpty() || createNewIssueReply_)
		return;
	QNetworkRequest request;

	QString issuesURL = "https://api.github.com/repos/AcquamanIssues/AcquamanIssuesTest/issues";
	request.setUrl(QUrl(issuesURL));

	QString userInfo = userName_+":"+password_;
	QByteArray userData = userInfo.toLocal8Bit().toBase64();
	QString headerData = "Basic " + userData;
	request.setRawHeader("Authorization", headerData.toLocal8Bit());

	QVariantMap jdata;
	jdata["title"] = title;
	jdata["body"] = body;
	if(!assignee.isEmpty())
		jdata["assignee"] = assignee;
	QJson::Serializer jserializer;
	QByteArray jsonData = jserializer.serialize(jdata);
	qDebug() << jsonData;

	createNewIssueReply_ = manager_->post(request, jsonData);
	connect(createNewIssueReply_, SIGNAL(readyRead()), this, SLOT(onCreateNewIssueReturned()));
}

void AMGithubManager::onAuthenicatedRequestReturned(){
	QJson::Parser parser;
	QVariant githubFullReply = parser.parse(authenticateReply_->readAll());
	authenticated_ = false;
	if(githubFullReply.canConvert(QVariant::Map)){
		QVariantMap jsonMap = githubFullReply.toMap();
		QMap<QString, QVariant>::const_iterator i = jsonMap.constBegin();
		if(i.key() == "message" && i.value().toString() == "Bad credentials")
			authenticated_ = false;
		else
			authenticated_ = true;
	}
	disconnect(authenticateReply_, 0);
	authenticateReply_->deleteLater();
	authenticateReply_ = 0;
	emit authenticated(authenticated_);
}

void AMGithubManager::onIssuesReturned(){
	QJson::Parser parser;
	QVariant githubFullReply = parser.parse(getIssuesReply_->readAll());
	qDebug() << githubFullReply;
	bool doEmit = false;
	QVariantMap retVal;
	if(githubFullReply.canConvert(QVariant::List)){
		QVariantList githubListReply = githubFullReply.toList();
		if(githubListReply.at(0).canConvert(QVariant::Map)){
			doEmit = true;
			retVal = githubListReply.at(0).toMap();
		}
	}
	disconnect(getIssuesReply_, 0);
	getIssuesReply_->deleteLater();
	getIssuesReply_ = 0;
	if(doEmit)
		emit issuesReturned(retVal);
}

void AMGithubManager::onCreateNewIssueReturned(){
	disconnect(createNewIssueReply_, 0);
	createNewIssueReply_->deleteLater();
	createNewIssueReply_ = 0;
	emit createNewIssueReturned();
}

void AMGithubManager::initialize(){
	manager_ = new QNetworkAccessManager(this);
	userName_ = "";
	password_ = "";
	repository_ = "";
	authenticateReply_ = 0;
	getIssuesReply_ = 0;
	createNewIssueReply_ = 0;
	authenticated_ = false;
}
