#ifndef AMGITHUBREPOSITORYVIEW_H
#define AMGITHUBREPOSITORYVIEW_H

#include <QWidget>
#include <QGroupBox>

#include "AMGitHubRepository.h"

class QLabel;
class QPushButton;

class AMGitHubRepositoryIssueMapTypeView;
class AMGitHubIssueMapView;

class AMGitHubRepositoryView : public QWidget
{
Q_OBJECT
public:
	AMGitHubRepositoryView(AMGitHubRepository *repository, QWidget *parent = 0);

protected slots:
	void onIssuesForReviewButtonClicked();
	void onIssuesNeedingEstimatesReported();

protected:
	AMGitHubRepository *repository_;

	AMGitHubRepositoryIssueMapTypeView *allIssuesMapTypeView_;
	AMGitHubRepositoryIssueMapTypeView *openIssuesMapTypeView_;
	AMGitHubRepositoryIssueMapTypeView *closedIssuesMapTypeView_;

	AMGitHubRepositoryIssueMapTypeView *openSpecifiedMapTypeView_;
	AMGitHubRepositoryIssueMapTypeView *openMissingEstimateMapTypeView_;

	AMGitHubRepositoryIssueMapTypeView *closedUntrackedMapTypeView_;
	AMGitHubRepositoryIssueMapTypeView *closedPartiallyTrackedMapTypeView_;
	AMGitHubRepositoryIssueMapTypeView *closedFullyTrackedMapTypeView_;

	AMGitHubRepositoryIssueMapTypeView *closedCompleteMapTypeView_;
	AMGitHubRepositoryIssueMapTypeView *closedMissingEstimateMapTypeView_;
	AMGitHubRepositoryIssueMapTypeView *closedMissingActualMapTypeView_;
	AMGitHubRepositoryIssueMapTypeView *closedMissingTimeReportMapTypeView_;
	AMGitHubRepositoryIssueMapTypeView *closedSpecifiedMapTypeView_;

	QList<AMGitHubRepositoryIssueMapTypeView*> allIssueMapTypeViews_;

	QPushButton *issuesForReviewButton_;
	QPushButton *issuesNeedingEstimateReported_;
};

class AMGitHubIssueMapView : public QGroupBox
{
Q_OBJECT
public:
	AMGitHubIssueMapView(const QMap<int, AMGitHubIssue*> *issueMap, QWidget *parent = 0);

protected:
	const QMap<int, AMGitHubIssue*> *issueMap_;
};

class AMGitHubRepositoryIssueMapTypeView : public QWidget
{
Q_OBJECT
public:
	AMGitHubRepositoryIssueMapTypeView(AMGitHubRepository *repository, const QString &issuesMapTypeString, AMGitHubRepository::IssueMapType issuesMapType, QWidget *parent = 0);

	QString issuesMapTypeString() const { return issuesMapTypeString_; }

protected slots:
	void onMoreInfoButtonClicked();

protected:
	AMGitHubRepository *repository_;
	QString issuesMapTypeString_;
	AMGitHubRepository::IssueMapType issuesMapType_;

	QLabel *countLabel_;
	QPushButton *moreInfoButton_;
	AMGitHubIssueMapView *mapView_;
};

#endif // AMGITHUBREPOSITORYVIEW_H
