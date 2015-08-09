#ifndef AMGITHUBPROJECT_H
#define AMGITHUBPROJECT_H

#include "AMGitHubIssueFamily.h"
#include "AMGitHubMilestone.h"

/*
class AMGitHubProject : public QObject
{
Q_OBJECT
public:
	AMGitHubProject(QList<AMGitHubIssueFamily*> projectIssues, const QDateTime &endDate, int weeksToCompute = 1, QObject *parent = 0);
	AMGitHubProject(AMGitHubMilestone *projectMilestone, const QDateTime &endDate, int weeksToCompute = 1, QObject *parent = 0);

	void evaluate();

protected:
	QList<AMGitHubIssueFamily*> projectIssues_;

	QDateTime endDate_;
	int weeksToCompute_;

	QList<double> normalizedEstimatedList_;
	double normalizedEstimated_;
	QList<double> normalizedCurrentList_;
	double normalizedCurrent_;
	QList<double> normalizedCompletedList_;
	double normalizedCompleted_;
	QList<double> normalizedValueList_;
	double normalizedValue_;

	QList<double> normalizedOpenList_;
	double normalizedOpen_;
	QList<double> normalizedClosedList_;
	double normalizedClosed_;

	QList<QList<double> > allOutstandingEstimatesLists_;
	QList<double> allOutstandingEstimatesTotals_;
	QList<QList<double> > allCompletedEstimatesLists_;
	QList<double> allCompletedEstimatesTotals_;
	QList<QList<double> > allWithdrawnEstimatesLists_;
	QList<double> allWithdrawnEstimatesTotals_;

	QList<QList<double> > allComplexityMappedCompetedWorkLists_;
	QList<double> allComplexityMappedCompetedWorkTotals_;
	QList<QList<double> > allReportedCompetedWorkLists_;
	QList<double> allReportedCompetedWorkTotals_;

	QList<QDateTime> dateList_;

	QList<double> weeklyEstimatedVelocity_;
	QList<double> weeklyComplexityMappedVelocity_;
	QList<double> weeklyReportedVelocity_;


	double averageEstimatedVelocityTotal_;
	double averageComplexityMappedVelocityTotal_;
	double averageReportedVelocityTotal_;

	double averageCalendarEstimatedVelocity_;
	double averageCalendarComplexityMappedVelocity_;
	double averageCalendarReportedVelocity_;
	double availablePersonWeeks_;
	double averageAvailabilityEstimatedVelocity_;
	double averageAvailabilityComplexityMappedVelocity_;
	double averageAvailabilityReportedVelocity_;
};
*/

#endif // AMGITHUBPROJECT_H
