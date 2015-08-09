#ifndef AMGITHUBISSUEFAMILY_H
#define AMGITHUBISSUEFAMILY_H

#include "AMGitHubIssue.h"

class AMGitHubIssueFamily : public QObject
{
Q_OBJECT
public:
	enum ComplexityMapping { InvalidComplexityMapping = 0,
				 Complexity1ToComplexity1 = 1, Complexity1ToComplexity2 = 2, Complexity1ToComplexity3 = 3, Complexity1ToComplexity5 = 4, Complexity1ToComplexity8 = 5,
				 Complexity2ToComplexity1 = 6, Complexity2ToComplexity2 = 7, Complexity2ToComplexity3= 7, Complexity2ToComplexity5 = 9, Complexity2ToComplexity8 = 10,
				 Complexity3ToComplexity1 = 11, Complexity3ToComplexity2 = 12, Complexity3ToComplexity3 = 13, Complexity3ToComplexity5 = 14, Complexity3ToComplexity8 = 15,
				 Complexity5ToComplexity1 = 16, Complexity5ToComplexity2 = 17, Complexity5ToComplexity3 = 18, Complexity5ToComplexity5 = 19, Complexity5ToComplexity8 = 20,
				 Complexity8ToComplexity1 = 21, Complexity8ToComplexity2 = 22, Complexity8ToComplexity3 = 23, Complexity8ToComplexity5 = 24, Complexity8ToComplexity8 = 25,
				 ComplexityKToComplexity1 = 26, ComplexityKToComplexity2 = 27, ComplexityKToComplexity3 = 28, ComplexityKToComplexity5 = 29, ComplexityKToComplexity8 = 30 };

	AMGitHubIssueFamily(AMGitHubIssue *originatingIssue = 0, AMGitHubIssue *pullRequestIssue = 0, QObject *parent = 0);
	virtual ~AMGitHubIssueFamily();

	int originatingIssueNumber() const;
	int pullRequestIssueNumber() const;

	AMGitHubIssue* originatingIssue() const { return originatingIssue_; }
	AMGitHubIssue* pullRequestIssue() const { return pullRequestIssue_; }

	AMGitHubIssue::ActualComplexityValue estimatedComplexity() const;
	AMGitHubIssue::ActualComplexityValue actualComplexity() const;

	QString timeEstimate() const;

	QString multiLineDebugInfo() const;

	bool completeInfo() const;
	bool totallyIncomplete() const;
	bool onlyMissingActualComplexity() const;
	bool onlyMissingTimeEstimate() const;

	AMGitHubIssueFamily::ComplexityMapping complexityMapping() const;
	bool symmetricComplexityMapping() const;

	double normalizedTimeEstiamte() const;

public slots:
	void setOriginatingIssue(AMGitHubIssue *originatingIssue) { originatingIssue_ = originatingIssue; }
	void setPullRequestIssue(AMGitHubIssue *pullRequestIssue) { pullRequestIssue_ = pullRequestIssue; }

protected:
	AMGitHubIssue *originatingIssue_;
	AMGitHubIssue *pullRequestIssue_;
};

#endif // AMGITHUBISSUEFAMILY_H
