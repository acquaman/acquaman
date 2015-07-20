#ifndef AMGITHUBGETISSUESACTION_H
#define AMGITHUBGETISSUESACTION_H

#include "actions3/actions/AMGitHubStandardAction.h"
#include "actions3/actions/AMGitHubGetIssuesActionInfo.h"

class AMGitHubIssue;
class AMGitHubMilestone;

#define AMGITHUBGETISSUESACTION_CANNOT_START_WITH_EMPTY_OWNER 357001
#define AMGITHUBGETISSUESACTION_CANNOT_START_WITH_EMPTY_REPO 357002
#define AMGITHUBGETISSUESACTION_CANNOT_START_WITHOUT_ALL_ISSUES_MAP 355003
#define AMGITHUBGETISSUESACTION_CANNOT_START_WITHOUT_ALL_MILESTONES_MAP 355004

class AMGitHubGetIssuesAction : public AMGitHubStandardAction
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

	/// We can always access our info object via info_ or info(), but it will come back as a AMGitHubGetIssuesActionInfo* pointer that we would need to cast to AMRestActionInfo. This makes it easier to access.
	const AMGitHubGetIssuesActionInfo *githubGetIssueActionInfo() const { return qobject_cast<const AMGitHubGetIssuesActionInfo *>(info()); }
	/// We can always access our info object via info_ or info(), but it will come back as a AMGitHubGetIssuesActionInfo* pointer that we would need to cast to AMRestActionInfo. This makes it easier to access.
	AMGitHubGetIssuesActionInfo* githubGetIssueActionInfo() { return qobject_cast<AMGitHubGetIssuesActionInfo *>(info()); }

public slots:
	/// Sets the map for storing all of the issues
	void setAllIssuesMap(QMap<int, AMGitHubIssue*> *allIssues);
	/// Sets the map for storing all of the milestones
	void setAllMilestonesMap(QMap<int, AMGitHubMilestone*> *allMilestones);

protected:
	/// This function is called from the Starting state when the implementation should initiate the action.
	virtual void startImplementation();

	virtual void setupRestAction();

	virtual void restResponsePreImplementation(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs);
	virtual void restResponseImplementation(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs);

//protected slots:
//	void onGetAllIssuesFullResponseReady(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs);
//	void onGetAllIssuesFailed();

protected:
	QMap<int, AMGitHubIssue*> *allIssues_;
	QMap<int, AMGitHubMilestone*> *allMilestones_;
};

#endif // AMGITHUBGETISSUESACTION_H
