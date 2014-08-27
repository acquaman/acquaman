/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMGithubManager.h"

#include "util/AMErrorMonitor.h"

AMGithubManager::AMGithubManager(QObject *parent) :
	QObject(parent)
{
	initialize();
}

AMGithubManager::~AMGithubManager(){}

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

QString AMGithubManager::jsonSensiblePrint(const QVariantMap &jsonMap, int indentLevel) const{
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
	authenticateReply_->ignoreSslErrors();
	connect(authenticateReply_, SIGNAL(readyRead()), this, SLOT(onAuthenicatedRequestReturned()));
	connect(authenticateReply_, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onSomeErrorOccured(QNetworkReply::NetworkError)));
}

void AMGithubManager::getIssues(AMGithubManager::IssuesFilter filter, AMGithubManager::IssuesState state, AMGithubManager::IssuesSort sort, AMGithubManager::IssuesDirection direction){
	if(!isAuthenticated() || repository_.isEmpty() || getIssuesReply_)
		return;
	QNetworkRequest request;

	QString issuesURL = "https://api.github.com/repos/"+repository_+"/issues";
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

	QString issuesURL = "https://api.github.com/repos/"+repository_+"/issues";
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

	createNewIssueReply_ = manager_->post(request, jsonData);
	createNewIssueReply_->ignoreSslErrors();
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
	QJson::Parser parser;
	bool retVal = false;
	if(createNewIssueReply_->rawHeader("Status") == "201 Created")
		retVal = true;
	disconnect(createNewIssueReply_, 0);
	createNewIssueReply_->deleteLater();
	createNewIssueReply_ = 0;
	emit issueCreated(retVal);
}

void AMGithubManager::onSomeErrorOccured(QNetworkReply::NetworkError nError){
	AMErrorMon::alert(this, AMGITHUBMANAGER_NETWORK_ERROR_OCCURRED, QString("A network error (%1) occurred in AMGithubManager.").arg(nError) );
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
