#ifndef AMGITHUBCOMPLEXITYMANAGER_H
#define AMGITHUBCOMPLEXITYMANAGER_H

#include "AMGitHubIssueFamily.h"

#include <QVector>

namespace AMGitHubStats {
	double mean(const QVector<double> &input);
	double standardDeviation(const QVector<double> &input, double useMean = -1);
}

class AMGitHubComplexityManager : public QObject
{
Q_OBJECT
public:
	AMGitHubComplexityManager(QObject *parent = 0);

	int analyzedIssues() const { return mappedIssues_.count(); }

	double probabilityOfMapping(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue, AMGitHubIssue::ActualComplexityValue actualComplexityValue) const;
	double probabilityOfMapping(AMGitHubComplexityMapping *complexityMapping) const;

	double probabilityOfMappingInRow(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue, AMGitHubIssue::ActualComplexityValue actualComplexityValue) const;
	double probabilityOfMappingInRow(AMGitHubComplexityMapping *complexityMapping) const;

	double averageTimeForEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue) const;
	double standardDeviationTimeForEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue) const;

	double averageTimeForActualComplexity(AMGitHubIssue::ActualComplexityValue actualComplexityValue) const;
	double standardDeviationTimeForActualComplexity(AMGitHubIssue::ActualComplexityValue actualComplexityValue) const;

	double probableTimeForEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue) const;
	double probablestandardDeviationTimeForEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue) const;

	QString probableTimeStringForEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue) const;
	QString fullMatrixString() const;

	double outstandingEstimateAtDate(AMGitHubIssue *issue, const QDateTime &date);
	double completedEstimateAtDate(AMGitHubIssue *issue, const QDateTime &date);
	double withdrawnEstimateAtDate(AMGitHubIssue *issue, const QDateTime &date);

	double complexityMappedCompletedWorkAtDate(AMGitHubIssue *issue, const QDateTime &date);
	double reportedCompletedWorkAtDate(AMGitHubIssue *issue, const QDateTime &date);

public slots:
	bool addMapping(AMGitHubIssue *oneIssue);
	void clearComplexityMappings();

protected:
	QMap<int, AMGitHubIssue*> mappedIssues_;
	QVector<QList<AMGitHubIssue*> > complexityMappingMatrix_;
};

#endif // AMGITHUBCOMPLEXITYMANAGER_H
