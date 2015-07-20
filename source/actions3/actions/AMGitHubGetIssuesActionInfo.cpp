#include "AMGitHubGetIssuesActionInfo.h"

AMGitHubGetIssuesActionInfo::AMGitHubGetIssuesActionInfo(const QString &owner, const QString &repo, AMGitHubGetIssuesActionInfo::IssueState issueState, QObject *parent)
	: AMActionInfo3("REST Github Issues Call", "REST Github Issues Call", ":/user-away.png", parent)
{
	owner_ = owner;
	repo_ = repo;
	issueState_ = issueState;

	setShortDescription("REST Github Issues Call");
	setLongDescription("REST Github Issues Call");
}

AMGitHubGetIssuesActionInfo::AMGitHubGetIssuesActionInfo(const AMGitHubGetIssuesActionInfo &other)
	: AMActionInfo3(other)
{
	owner_ = other.owner();
	repo_ = other.repo();
	issueState_ = other.issueState();
}

AMActionInfo3 *AMGitHubGetIssuesActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMGitHubGetIssuesActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

QString AMGitHubGetIssuesActionInfo::issueStateString() const
{
	switch(issueState_){
	case AMGitHubGetIssuesActionInfo::OpenIssues:
		return "open";
	case AMGitHubGetIssuesActionInfo::ClosedIssues:
		return "closed";
	case AMGitHubGetIssuesActionInfo::AllIssues:
		return "all";
	}
}

void AMGitHubGetIssuesActionInfo::setOwner(const QString &owner)
{
	owner_ = owner;
}

void AMGitHubGetIssuesActionInfo::setRepo(const QString &repo)
{
	repo_ = repo;
}

void AMGitHubGetIssuesActionInfo::setIssueState(AMGitHubGetIssuesActionInfo::IssueState issueState)
{
	issueState_ = issueState;
}
