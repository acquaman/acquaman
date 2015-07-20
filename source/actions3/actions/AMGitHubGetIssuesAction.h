#ifndef AMGITHUBGETISSUESACTION_H
#define AMGITHUBGETISSUESACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/AMGitHubGetIssuesActionInfo.h"

#include <QNetworkReply>

class QNetworkAccessManager;

class AMGitHubIssue;
class AMGitHubMilestone;

#define AMGITHUBGETISSUESACTION_CANNOT_START_WITH_EMPTY_OWNER 357001
#define AMGITHUBGETISSUESACTION_CANNOT_START_WITH_EMPTY_REPO 357002
#define AMGITHUBGETISSUESACTION_CANNOT_START_WITHOUT_NETWORKACCESSMANAGER 355003
#define AMGITHUBGETISSUESACTION_CANNOT_START_WITHOUT_ALL_ISSUES_MAP 355004
#define AMGITHUBGETISSUESACTION_CANNOT_START_WITHOUT_ALL_MILESTONES_MAP 355005

class AMGitHubGetIssuesAction : public AMAction3
{
Q_OBJECT
public:
	/// Constructor.
	Q_INVOKABLE AMGitHubGetIssuesAction(AMGitHubGetIssuesActionInfo *info, QNetworkAccessManager *networkAccessManager, const QString &authorizationHeader, QMap<int, AMGitHubIssue*> *allIssues, QMap<int, AMGitHubMilestone*> *allMilestones, QObject *parent = 0);
	/// Copy constructor.
	AMGitHubGetIssuesAction(const AMGitHubGetIssuesAction &other);
	/// Destructor.
	virtual ~AMGitHubGetIssuesAction() {}

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

	/// We can always access our info object via info_ or info(), but it will come back as a AMGitHubGetIssuesActionInfo* pointer that we would need to cast to AMRestActionInfo. This makes it easier to access.
	const AMGitHubGetIssuesActionInfo *githubGetIssueActionInfo() const { return qobject_cast<const AMGitHubGetIssuesActionInfo *>(info()); }
	/// We can always access our info object via info_ or info(), but it will come back as a AMGitHubGetIssuesActionInfo* pointer that we would need to cast to AMRestActionInfo. This makes it easier to access.
	AMGitHubGetIssuesActionInfo* githubGetIssueActionInfo() { return qobject_cast<AMGitHubGetIssuesActionInfo *>(info()); }

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
	/// Sets the map for storing all of the milestones
	void setAllMilestonesMap(QMap<int, AMGitHubMilestone*> *allMilestones);

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


protected slots:
	void onGetAllIssuesFullResponseReady(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs);
	void onGetAllIssuesFailed();

protected:
	/// Pointer to a long-lived network access manager
	QNetworkAccessManager *networkAccessManager_;

	/// Header string to use for authorization
	QString authorizationHeader_;

	/// Holds the current page we're searching
	int currentIssuesPage_;
	/// Holds whether this is the last page or not
	bool lastPage_;

	QMap<int, AMGitHubIssue*> *allIssues_;
	QMap<int, AMGitHubMilestone*> *allMilestones_;
};

#endif // AMGITHUBGETISSUESACTION_H
