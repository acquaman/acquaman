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

#define AMGITHUBMANAGER_NETWORK_ERROR_OCCURRED 605001

class AMGithubManager : public QObject
{
	Q_OBJECT

public:
	/// Enum for the issues filter type.
	enum IssuesFilter { IssuesFilterAssigned, IssuesFilterCreated, IssuesFilterMentioned, IssuesFilterSubscribed};
	/// Enum for the issues state filter type.
	enum IssuesState { IssuesStateOpen, IssuesStateClosed};
	/// Enum for the issues sort type filter.
	enum IssuesSort{ IssuesSortCreated, IssuesSortUpdated, IssuesSortComments};
	/// Enum for the issues sorting direction filter.
	enum IssuesDirection{ IssuesDirectionAscending, IssuesDirectionDescending};

	/// Constructor.  Builds a default Github manager.
 	virtual ~AMGithubManager();
	AMGithubManager(QObject *parent = 0);
	/// Constructor.  Builds a default Github manager and attempts to connect to Github based on the given username, password, and respository.
	AMGithubManager(const QString &userName, const QString &password, const QString &repository, QObject *parent = 0);

	/// Returns the current user name.
	QString userName() const;
	/// Returns whether we are currently authenticated with the Github server.
	bool isAuthenticated() const;
	/// Returns which repository we are connecting to.
	QString repository() const;
	/// Returns a string in a readable format based on the returned values when requesting the issues from the respository.
	QString jsonSensiblePrint(const QVariantMap &jsonMap, int indentLevel = 0) const;

public slots:
	/// Sets the user name used to connect to the repository.
	void setUserName(const QString &userName);
	/// Sets the password used to authenticate to the repository.
	void setPassword(const QString &password);
	/// Attempts to authenticate with Github with the provided user name and password.
	void authenticate();
	/// Sets which repository this should be connected to.
	void setRepository(const QString &repository);
	/// Does the work to get the issues from the connected Github repository based on the \param filter, \param state, \param sort, and \param direction flags.
	void getIssues(AMGithubManager::IssuesFilter filter = AMGithubManager::IssuesFilterAssigned,
			   AMGithubManager::IssuesState state = AMGithubManager::IssuesStateOpen,
			   AMGithubManager::IssuesSort sort = AMGithubManager::IssuesSortCreated,
			   AMGithubManager::IssuesDirection direction = AMGithubManager::IssuesDirectionAscending);
	/// Slot that creates a new issue with the given \param title, \param body, and, optionally, an \param assignee.
	void createNewIssue(const QString &title, const QString &body, const QString &assignee = QString());

signals:
	/// Notifier that indicates whether we successfully authenticated with Github or not.
	void authenticated(bool isAuthenticated);
	/// Notifier that contains all issues when they have been requested.
	void issuesReturned(QVariantMap issues);
	/// Notifier whether or not an issue was successfully created.
	void issueCreated(bool issueCreated);

protected slots:
	/// Slot handling the authentication response.
	void onAuthenicatedRequestReturned();
	/// Slot handling the issues request response.
	void onIssuesReturned();
	/// Slot handling the response when creating a new issue.
	void onCreateNewIssueReturned();

	void onSomeErrorOccured(QNetworkReply::NetworkError nError);

protected:
	/// Helper method that intializes all of the classes member variables.
	void initialize();

protected:
	/// Pointer that handles the network access.
	QNetworkAccessManager *manager_;
	/// Pointer specifically focusing on the authenticatation network reply.
	QNetworkReply *authenticateReply_;
	/// Pointer specifically focusing on the get issues network reply.
	QNetworkReply *getIssuesReply_;
	/// Pointer specifically focusing on the create new issue network reply.
	QNetworkReply *createNewIssueReply_;

	/// Holds the user name used for connecting to Github.
	QString userName_;
	/// Holds the password that goes with the user name.
	QString password_;
	/// Flag holding whether or not this manager has successfully been authenticated.
	bool authenticated_;
	/// Holds the name of the repository of interest.
	QString repository_;
};

#endif // AMGITHUBMANAGER_H
