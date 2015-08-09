#ifndef AMGITHUBCOMPLEXITYMANAGER_H
#define AMGITHUBCOMPLEXITYMANAGER_H

#include "AMGitHubIssueFamily.h"

#include <QVector>

class AMGitHubComplexityManager : public QObject
{
Q_OBJECT
public:
	AMGitHubComplexityManager(QObject *parent = 0);

	int analyzedIssues() const { return mappedIssues_.count(); }

	double probabilityOfMapping(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue, AMGitHubIssue::ActualComplexityValue actualComplexityValue) const;
	double probabilityOfMapping(AMGitHubComplexityMapping *complexityMapping) const;

	double probabilityOfMappingInColumn(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue, AMGitHubIssue::ActualComplexityValue actualComplexityValue) const;
	double probabilityOfMappingInColumn(AMGitHubComplexityMapping *complexityMapping) const;

	double averageTimeForEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue) const;
	double averageTimeForActualComplexity(AMGitHubIssue::ActualComplexityValue actualComplexityValue) const;
	double probableTimeForEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue) const;

	QString probableTimeStringForEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue) const;
	QString fullMatrixString() const;

//	double averageTimeForEstimatedComplexity(AMGitHubIssue::ActualComplexityValue estimatedComplexityValue);
//	double averageTimeForActualComplexity(AMGitHubIssue::ActualComplexityValue actualComplexityValue);
//	double probableTimeForEstimatedComplexity(AMGitHubIssue::ActualComplexityValue estimatedComplexityValue);

//	double outstandingEstimateAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date);
//	double completedEstimateAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date);
//	double withdrawnEstimateAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date);

//	double complexityMappedCompletedWorkAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date);
//	double reportedCompletedWorkAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date);

public slots:
	bool addMapping(AMGitHubIssue *oneIssue);
	void clearComplexityMappings();

//	void incrementComplexityMapping(AMGitHubIssueFamily::ComplexityMapping complexityMapping);
//	void clearComplexityMappings();

//	void addTimeEstimateForEstimatedComplexity(double normalizedTimeEstimate, AMGitHubIssue::ActualComplexityValue complexityValue);
//	void addTimeEstimateForActualComplexity(double normalizedTimeEstimate, AMGitHubIssue::ActualComplexityValue complexityValue);

protected:
	QMap<int, AMGitHubIssue*> mappedIssues_;
	QVector<QList<AMGitHubIssue*> > complexityMappingMatrix_;
//	QVector<int> complexityMappingMatrix_;

//	QList<double> estimatedComplexity1Times_;
//	QList<double> estimatedComplexity2Times_;
//	QList<double> estimatedComplexity3Times_;
//	QList<double> estimatedComplexity5Times_;
//	QList<double> estimatedComplexity8Times_;
//	QList<double> estimatedComplexityKTimes_;
//	QList<double> actualComplexity1Times_;
//	QList<double> actualComplexity2Times_;
//	QList<double> actualComplexity3Times_;
//	QList<double> actualComplexity5Times_;
//	QList<double> actualComplexity8Times_;
};

#endif // AMGITHUBCOMPLEXITYMANAGER_H
