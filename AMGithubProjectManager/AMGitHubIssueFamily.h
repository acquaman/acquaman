#ifndef AMGITHUBISSUEFAMILY_H
#define AMGITHUBISSUEFAMILY_H

#include "AMGitHubIssue.h"

class AMGitHubIssueFamily : public QObject
{
Q_OBJECT
public:
	AMGitHubIssueFamily(AMGitHubIssue *originatingIssue, AMGitHubIssue *pullRequestIssue, QObject *parent = 0);

	const AMGitHubIssue* originatingIssue() const { return originatingIssue_; }
	const AMGitHubIssue* pullRequestIssue() const { return pullRequestIssue_; }

	AMGitHubIssue* originatingIssue() { return originatingIssue_; }
	AMGitHubIssue* pullRequestIssue() { return pullRequestIssue_; }

	/// The family is fully specified for its current combination of open/closed/pull request state
	bool fullySpecifiedFamily() const;
	/// The family is fully closed and fully specified
	bool completedFamily() const;

	double normalizedTimeEstimateForFamily() const;

public slots:
	void setOriginatingIssue(AMGitHubIssue *originatingIssue) { originatingIssue_ = originatingIssue; }
	void setPullRequestIssue(AMGitHubIssue *pullRequestIssue) { pullRequestIssue_ = pullRequestIssue; }

protected:
	AMGitHubIssue *originatingIssue_;
	AMGitHubIssue *pullRequestIssue_;
};

#endif // AMGITHUBISSUEFAMILY_H
