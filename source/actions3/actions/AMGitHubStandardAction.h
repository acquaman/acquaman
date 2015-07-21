#ifndef AMGITHUBSTANDARDACTION_H
#define AMGITHUBSTANDARDACTION_H

#include <QNetworkReply>

#include "actions3/AMAction3.h"

class QNetworkAccessManager;

class AMRestAction;

#define AMGITHUBSTANDARDACTION_CANNOT_START_WITHOUT_NETWORKACCESSMANAGER 359001
#define AMGITHUBSTANDARDACTION_CANNOT_START_WITHOUT_RESTACTION 359002

class AMGitHubStandardAction : public AMAction3
{
Q_OBJECT
public:
	/// Constructor.
	AMGitHubStandardAction(AMActionInfo3 *info, QNetworkAccessManager *networkAccessManager, const QString &authorizationHeader, QObject *parent = 0);

	/// Copy constructor.
	AMGitHubStandardAction(const AMGitHubStandardAction &other);
	/// Destructor.
	virtual ~AMGitHubStandardAction() {}

//	/// Virtual copy constructor.
//	virtual AMAction3 *createCopy() const;

	/// Specify that we can pause.
	virtual bool canPause() const { return false; }
	/// Specify that we can skip.
	virtual bool canSkip() const { return false; }

	/// Virtual function that denotes that this action has no children.
	virtual bool hasChildren() const { return false; }
	/// Virtual function that returns the number of children for this action.
	virtual int numberOfChildren() const { return 0; }

	/// Returns the long-lived network access manager we're using
	QNetworkAccessManager* networkAccessManager() const { return networkAccessManager_; }

	/// Returns the string to use in the authorization header field
	QString authorizationHeader() const { return authorizationHeader_; }

public slots:
	/// Sets the pointer to a long-lived network access manager. This action will not clean up the memory or take ownership of this.
	void setNetworkAccessManager(QNetworkAccessManager *networkAccessManager);

	/// Sets the string to set in the authorization header field
	void setAuthorizationHeader(const QString &authorizationHeader);

protected slots:
	void onRestActionFullResponseReady(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs);
	void onRestActionFailed();

protected:
	/// This function is called from the Starting state when the implementation should initiate the action.
	virtual void startImplementation();
	/// Pauses the action by stopping the timer.
	virtual void pauseImplementation();
	/// Resumes the action by resuming the timer.
	virtual void resumeImplementation();
	/// Handles cancelling the scan.
	virtual void cancelImplementation();
	/// Handles skipping the action.
	virtual void skipImplementation(const QString &command);

	/// This function is implemented by subclasses to allow them to return a REST action based on their specifications. It will be called during startImplementation and after each page returns
	virtual AMRestAction* setupNextRestAction() = 0;

	/// This function offers subclasses an opporunity to operate on the response before the headers are check for pagination and status
	virtual void restResponsePreImplementation(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs) = 0;
	/// This function is implemented by subclasses to actually do the work of parsing and using the response
	virtual void restResponseImplementation(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs) = 0;

protected:
	/// Pointer to a long-lived network access manager
	QNetworkAccessManager *networkAccessManager_;

	/// Header string to use for authorization
	QString authorizationHeader_;

	int headerLastPageNumber_;
	int headerNextPageNumber_;
	QString headerNextURL_;

	/// Holds the current page we're searching
//	int currentIssuesPage_;
	/// Holds whether this is the last page or not
	bool lastPage_;

	/// A pointer to the REST action that will do the work. Subclasses fill in the details in setupRestAction
	AMRestAction *restAction_;
};

#endif // AMGITHUBSTANDARDACTION_H
