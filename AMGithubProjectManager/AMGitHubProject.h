#ifndef AMGITHUBPROJECT_H
#define AMGITHUBPROJECT_H

#include "AMGitHubIssueFamily.h"
#include "AMGitHubMilestone.h"

class AMGitHubIssueValueMap : public QObject
{
Q_OBJECT
public:
	AMGitHubIssueValueMap(QObject *parent = 0);

	bool containsIssue(const AMGitHubIssue *issue) const;
	double valueForIssue(const AMGitHubIssue *issue) const;
	QList<double> valueList() const;
	const QMap<const AMGitHubIssue*, double>* valueMap() const { return valueMap_; }

	double total() const;
	double average(double divisor = -1) const;

public slots:
	void insertMapping(const AMGitHubIssue *issue, double value);

protected:
	QMap<const AMGitHubIssue*, double> *valueMap_;
};

class AMGitHubIssueValueDateProgression : public QObject
{
Q_OBJECT
public:
	AMGitHubIssueValueDateProgression(QObject *parent = 0);

	bool containsDateTime(const QDateTime &dateTime) const;
	const AMGitHubIssueValueMap* valueMapForDateTime(const QDateTime &dateTime) const;
	QList<QDateTime> dateTimeList() const;
	QList<AMGitHubIssueValueMap*> valueMapList() const;
	QList<double> totalsList() const;
	QList<double> averageList(double divisor = -1) const;
	const QMap<QDateTime, AMGitHubIssueValueMap*>* dateProgression() const { return dateProgression_; }

	double totalForDate(const QDateTime &dateTime) const;
	double averageForDate(const QDateTime &dateTime, double divisor = -1) const;

public slots:
	void insertMapping(const QDateTime &dateTime, AMGitHubIssueValueMap *valueMap);

protected:
	QMap<QDateTime, AMGitHubIssueValueMap*> *dateProgression_;
};

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
