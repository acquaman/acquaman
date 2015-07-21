#ifndef AMGITHUBGETCOMMENTSACTION_H
#define AMGITHUBGETCOMMENTSACTION_H

#include "actions3/actions/AMGitHubStandardAction.h"
#include "actions3/actions/AMGitHubGetCommentsActionInfo.h"

class AMGitHubIssue;

#define AMGITHUBGETCOMMENTSACTION_CANNOT_START_WITH_EMPTY_OWNER 358001
#define AMGITHUBGETCOMMENTSACTION_CANNOT_START_WITH_EMPTY_REPO 358002
#define AMGITHUBGETCOMMENTSACTION_CANNOT_START_WITHOUT_ALL_ISSUES_MAP 358003

class AMGitHubGetCommentsAction : public AMGitHubStandardAction
{
Q_OBJECT
public:
	/// Constructor.
	Q_INVOKABLE AMGitHubGetCommentsAction(AMGitHubGetCommentsActionInfo *info, QNetworkAccessManager *networkAccessManager, const QString &authorizationHeader, QMap<int, AMGitHubIssue*> *allIssues, QObject *parent = 0);
	/// Copy constructor.
	AMGitHubGetCommentsAction(const AMGitHubGetCommentsAction &other);
	/// Destructor.
	virtual ~AMGitHubGetCommentsAction() {}

	/// Virtual copy constructor.
	virtual AMAction3 *createCopy() const;

	/// We can always access our info object via info_ or info(), but it will come back as a AMGitHubGetCommentsActionInfo* pointer that we would need to cast to AMRestActionInfo. This makes it easier to access.
	const AMGitHubGetCommentsActionInfo *githubGetCommentsActionInfo() const { return qobject_cast<const AMGitHubGetCommentsActionInfo *>(info()); }
	/// We can always access our info object via info_ or info(), but it will come back as a AMGitHubGetCommentsActionInfo* pointer that we would need to cast to AMRestActionInfo. This makes it easier to access.
	AMGitHubGetCommentsActionInfo* githubGetCommentsActionInfo() { return qobject_cast<AMGitHubGetCommentsActionInfo *>(info()); }

public slots:
	/// Sets the map for storing all of the issues
	void setAllIssuesMap(QMap<int, AMGitHubIssue*> *allIssues);

protected:
	/// This function is called from the Starting state when the implementation should initiate the action.
	virtual void startImplementation();

	/// Sets up the REST action with the correct pagination for the right call to the comments REST API
	virtual AMRestAction* setupNextRestAction();

	/// Does nothing in this class
	virtual void restResponsePreImplementation(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs);
	/// Actually parses the comments response and edits the issues in the map to add the comments
	virtual void restResponseImplementation(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs);

protected:
	/// Pointer to the map of all issues we will edit for comments
	QMap<int, AMGitHubIssue*> *allIssues_;

	QStringList commentURLsCopy_;
};

#endif // AMGITHUBGETCOMMENTSACTION_H
