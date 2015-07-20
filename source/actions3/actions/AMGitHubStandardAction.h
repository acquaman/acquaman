#ifndef AMGITHUBSTANDARDACTION_H
#define AMGITHUBSTANDARDACTION_H

#include <QNetworkReply>

#include "actions3/AMAction3.h"

class QNetworkAccessManager;

class AMGitHubStandardAction : public AMAction3
{
Q_OBJECT
public:
	/// Constructor.
	Q_INVOKABLE AMGitHubStandardAction(AMActionInfo3 *info, QNetworkAccessManager *networkAccessManager, const QString &authorizationHeader, QObject *parent = 0);

	/// Copy constructor.
	AMGitHubStandardAction(const AMGitHubStandardAction &other);
	/// Destructor.
	virtual ~AMGitHubStandardAction() {}

	/// Virtual copy constructor.
	virtual AMAction3 *createCopy() const;

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

protected:
	/// Pointer to a long-lived network access manager
	QNetworkAccessManager *networkAccessManager_;

	/// Header string to use for authorization
	QString authorizationHeader_;

	/// Holds the current page we're searching
	int currentIssuesPage_;
	/// Holds whether this is the last page or not
	bool lastPage_;
};

#endif // AMGITHUBSTANDARDACTION_H
