#ifndef AMZENHUBGETESTIMATESACTION_H
#define AMZENHUBGETESTIMATESACTION_H

#include <QNetworkReply>

#include "actions3/AMAction3.h"
#include "actions3/actions/AMZenHubGetEstimatesActionInfo.h"

class QNetworkAccessManager;

class AMGitHubIssue;
class AMRestAction;

#define AMZENHUBGETESTIMATESACTION_CANNOT_START_WITHOUT_NETWORKACCESSMANAGER 379001
#define AMZENHUBGETESTIMATESACTION_CANNOT_START_WITHOUT_RESTACTION 379002
#define AMZENHUBGETESTIMATESACTION_CANNOT_START_WITH_EMPTY_OWNER 379003
#define AMZENHUBGETESTIMATESACTION_CANNOT_START_WITH_EMPTY_REPO 379004
#define AMZENHUBGETESTIMATESACTION_CANNOT_START_WITHOUT_ALL_ISSUES_MAP 379005

class AMZenHubGetEstimatesAction : public AMAction3
{
Q_OBJECT
public:
	/// Constructor.
	Q_INVOKABLE AMZenHubGetEstimatesAction(AMZenHubGetEstimatesActionInfo *info, QNetworkAccessManager *networkAccessManager, const QString &authorizationHeader, QMap<int, AMGitHubIssue*> *allIssues, QObject *parent = 0);
	/// Copy constructor.
	AMZenHubGetEstimatesAction(const AMZenHubGetEstimatesAction &other);
	/// Destructor.
	virtual ~AMZenHubGetEstimatesAction() {}

	/// Virtual copy constructor.
	virtual AMAction3 *createCopy() const;

	/// We can always access our info object via info_ or info(), but it will come back as a AMGitHubGetCommentsActionInfo* pointer that we would need to cast to AMRestActionInfo. This makes it easier to access.
	const AMZenHubGetEstimatesActionInfo *zenhubGetEstimatesActionInfo() const { return qobject_cast<const AMZenHubGetEstimatesActionInfo *>(info()); }
	/// We can always access our info object via info_ or info(), but it will come back as a AMGitHubGetCommentsActionInfo* pointer that we would need to cast to AMRestActionInfo. This makes it easier to access.
	AMZenHubGetEstimatesActionInfo* zenhubGetEstimatesActionInfo() { return qobject_cast<AMZenHubGetEstimatesActionInfo *>(info()); }

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

	/// Sets the map for storing all of the issues
	void setAllIssuesMap(QMap<int, AMGitHubIssue*> *allIssues);

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

	AMRestAction* setupNextRestAction();

protected:
	/// Pointer to the map of all issues we will edit for comments
	QMap<int, AMGitHubIssue*> *allIssues_;

	QStringList estimatesURLsCopy_;

	/// Pointer to a long-lived network access manager
	QNetworkAccessManager *networkAccessManager_;

	/// Header string to use for authorization
	QString authorizationHeader_;

	/// A pointer to the REST action that will do the work. Subclasses fill in the details in setupRestAction
	AMRestAction *restAction_;

	QString lastURL_;
};

#endif // AMZENHUBGETESTIMATESACTION_H
