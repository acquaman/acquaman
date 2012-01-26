#ifndef AMGITHUBMANAGER_H
#define AMGITHUBMANAGER_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QAuthenticator>
#include <QSslError>
#include <QBuffer>

#include "source/qjson/serializer.h"
#include "source/qjson/parser.h"

class AMGithubManager : public QObject{
Q_OBJECT
public:
	enum IssuesFilter { IssuesFilterAssigned, IssuesFilterCreated, IssuesFilterMentioned, IssuesFilterSubscribed};
	enum IssuesState { IssuesStateOpen, IssuesStateClosed};
	enum IssuesSort{ IssuesSortCreated, IssuesSortUpdated, IssuesSortComments};
	enum IssuesDirection{ IssuesDirectionAscending, IssuesDirectionDescending};

	AMGithubManager(QObject *parent = 0);
	AMGithubManager(const QString &userName, const QString &password, const QString &repository, QObject *parent = 0);

	QString userName() const;
	bool isAuthenticated() const;

	QString repository() const;

	QString jsonSensiblePrint(const QVariantMap &jsonMap, int indentLevel = 0) const;

public slots:
	void setUserName(const QString &userName);
	void setPassword(const QString &password);
	void authenticate();

	void setRepository(const QString &repository);

	void getIssues(AMGithubManager::IssuesFilter filter = AMGithubManager::IssuesFilterAssigned,
		       AMGithubManager::IssuesState state = AMGithubManager::IssuesStateOpen,
		       AMGithubManager::IssuesSort sort = AMGithubManager::IssuesSortCreated,
		       AMGithubManager::IssuesDirection direction = AMGithubManager::IssuesDirectionAscending);

	void createNewIssue(const QString &title, const QString &body, const QString &assignee = QString());

signals:
	void authenticated(bool isAuthenticated);
	void issuesReturned(QVariantMap issues);
	void issueCreated(bool issueCreated);

protected slots:
	void onAuthenicatedRequestReturned();
	void onIssuesReturned();
	void onCreateNewIssueReturned();

protected:
	void initialize();

protected:
	QNetworkAccessManager *manager_;
	QNetworkReply *authenticateReply_;
	QNetworkReply *getIssuesReply_;
	QNetworkReply *createNewIssueReply_;

	QString userName_;
	QString password_;
	bool authenticated_;

	QString repository_;
};

#endif // AMGITHUBMANAGER_H
