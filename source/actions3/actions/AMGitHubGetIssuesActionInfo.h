#ifndef AMGITHUBGETISSUESACTIONINFO_H
#define AMGITHUBGETISSUESACTIONINFO_H

#include "actions3/AMActionInfo3.h"

class AMGitHubGetIssuesActionInfo : public AMActionInfo3
{
Q_OBJECT

Q_PROPERTY(QString owner READ owner WRITE setOwner)
Q_PROPERTY(QString repo READ repo WRITE setRepo)
Q_PROPERTY(AMGitHubGetIssuesActionInfo::IssueState issueState READ issueState WRITE setIssueState)

public:
	/// Enum for State of the issue
	enum IssueState { OpenIssues, ClosedIssues, AllIssues };

	Q_INVOKABLE AMGitHubGetIssuesActionInfo(const QString &owner, const QString &repo, AMGitHubGetIssuesActionInfo::IssueState issueState = AMGitHubGetIssuesActionInfo::AllIssues, QObject *parent = 0);
	/// Copy Constructor.
	AMGitHubGetIssuesActionInfo(const AMGitHubGetIssuesActionInfo &other);
	/// Destructor.
	virtual ~AMGitHubGetIssuesActionInfo(){}

	/// This function is used as a virtual copy constructor.
	virtual AMActionInfo3 *createCopy() const;

	/// This should describe the type of the action.
	virtual QString typeDescription() const { return QString("REST call to retrieve issues from %1/%2").arg(owner_).arg(repo_); }

	/// Returns the owner of the github repo
	QString owner() const { return owner_; }

	/// Returns the name of the github repo
	QString repo() const { return repo_; }

	/// Returns the issue state to be filtered by
	AMGitHubGetIssuesActionInfo::IssueState issueState() const { return issueState_; }
	/// Returns the equivalent string
	QString issueStateString() const;

public slots:
	/// Sets the owner of the repo
	void setOwner(const QString &owner);

	/// Sets the repo to search
	void setRepo(const QString &repo);

	/// Sets the issue state to filter by
	void setIssueState(AMGitHubGetIssuesActionInfo::IssueState issueState);

protected:
	/// Name of the owner of the github repo
	QString owner_;

	/// Name of the github repo
	QString repo_;

	/// The type of HTTP Verb to be used
	AMGitHubGetIssuesActionInfo::IssueState issueState_;
};

Q_DECLARE_METATYPE(AMGitHubGetIssuesActionInfo::IssueState)

#endif // AMGITHUBGETISSUESACTIONINFO_H
