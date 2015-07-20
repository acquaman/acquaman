#ifndef AMGITHUBCOMPLEXITYMANAGER_H
#define AMGITHUBCOMPLEXITYMANAGER_H

#include "AMGitHubIssueFamily.h"

#include <QVector>

class AMGitHubComplexityManager : public QObject
{
Q_OBJECT
public:
	AMGitHubComplexityManager(QObject *parent = 0);

	double averageTimeForEstimatedComplexity(AMGitHubIssue::ComplexityValue estimatedComplexityValue);
	double averageTimeForActualComplexity(AMGitHubIssue::ComplexityValue actualComplexityValue);
	double probableTimeForEstimatedComplexity(AMGitHubIssue::ComplexityValue estimatedComplexityValue);

	double outstandingEstimateAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date);
	double completedEstimateAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date);
	double withdrawnEstimateAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date);

	double complexityMappedCompletedWorkAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date);
	double reportedCompletedWorkAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date);

public slots:
	void incrementComplexityMapping(AMGitHubIssueFamily::ComplexityMapping complexityMapping);
	void clearComplexityMappings();

	void addTimeEstimateForEstimatedComplexity(double normalizedTimeEstimate, AMGitHubIssue::ComplexityValue complexityValue);
	void addTimeEstimateForActualComplexity(double normalizedTimeEstimate, AMGitHubIssue::ComplexityValue complexityValue);

protected:
	QVector<int> complexityMappingMatrix_;

	QList<double> estimatedComplexity1Times_;
	QList<double> estimatedComplexity2Times_;
	QList<double> estimatedComplexity3Times_;
	QList<double> estimatedComplexity5Times_;
	QList<double> estimatedComplexity8Times_;
	QList<double> estimatedComplexityKTimes_;
	QList<double> actualComplexity1Times_;
	QList<double> actualComplexity2Times_;
	QList<double> actualComplexity3Times_;
	QList<double> actualComplexity5Times_;
	QList<double> actualComplexity8Times_;
};

#endif // AMGITHUBCOMPLEXITYMANAGER_H
