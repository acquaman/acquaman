#include "AMGitHubIssueFamily.h"

AMGitHubIssueFamily::AMGitHubIssueFamily(AMGitHubIssue *originatingIssue, AMGitHubIssue *pullRequestIssue, QObject *parent) :
	QObject(parent)
{
	originatingIssue_ = originatingIssue;
	pullRequestIssue_ = pullRequestIssue;
}

bool AMGitHubIssueFamily::fullySpecifiedFamily() const
{
	if(!originatingIssue_)
		return false;
	if(originatingIssue_->isOpen())
		return originatingIssue_->issueFullySpecifiedForState();
	if(originatingIssue_->isClosed() && !pullRequestIssue_)
		return false;
	if(!originatingIssue_->issueFullySpecifiedForState() || !pullRequestIssue_->issueFullySpecifiedForState())
		return false;
	return true;
}

bool AMGitHubIssueFamily::completedFamily() const
{
	if(!originatingIssue_ || !pullRequestIssue_)
		return false;
	if(originatingIssue_->isOpen() || pullRequestIssue_->isOpen())
		return false;
	return (originatingIssue_->issueFullySpecifiedForState() && pullRequestIssue_->issueFullySpecifiedForState());
}

double AMGitHubIssueFamily::normalizedTimeEstimateForFamily() const
{
	if(!originatingIssue_)
		return -1;
	if(!pullRequestIssue_)
		return originatingIssue_->normalizedTimeEstimate();
	if(originatingIssue_ == pullRequestIssue_)
		return originatingIssue_->normalizedTimeEstimate();

	double normalizedForOriginating = originatingIssue_->normalizedTimeEstimate();
	double normalizedForPullRequest = pullRequestIssue_->normalizedTimeEstimate();
	if(normalizedForOriginating < 0)
		return normalizedForPullRequest;
	if(normalizedForPullRequest < 0)
		return normalizedForOriginating;
	return normalizedForOriginating + normalizedForPullRequest;
}
