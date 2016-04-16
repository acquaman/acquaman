#include "AMGitHubRepositoryView.h"

#include <QLabel>
#include <QPushButton>

#include <QFormLayout>
#include <QBoxLayout>

#include "AMGitHubComplexityManager.h"

AMGitHubRepositoryView::AMGitHubRepositoryView(AMGitHubRepository *repository, QWidget *parent) :
	QWidget(parent)
{
	repository_ = repository;

	QFormLayout *mainFL_ = new QFormLayout();

	allIssuesMapTypeView_ = new AMGitHubRepositoryIssueMapTypeView(repository_, "All Issues", AMGitHubRepository::AllIssuesMap);
	openIssuesMapTypeView_ = new AMGitHubRepositoryIssueMapTypeView(repository_, "Open Issues", AMGitHubRepository::OpenIssuesMap);
	closedIssuesMapTypeView_ = new AMGitHubRepositoryIssueMapTypeView(repository_, "Closed Issues", AMGitHubRepository::ClosedIssuesMap);

	openSpecifiedMapTypeView_ = new AMGitHubRepositoryIssueMapTypeView(repository_, "Specified [Open]", AMGitHubRepository::FullySpecifiedOpenMap);
	openMissingEstimateMapTypeView_ = new AMGitHubRepositoryIssueMapTypeView(repository_, "Missing Estimate [Open]", AMGitHubRepository::MissingEstimateOpenMap);

	closedUntrackedMapTypeView_ = new AMGitHubRepositoryIssueMapTypeView(repository_, "Untracked [Closed]", AMGitHubRepository::UntrackedClosedMap);
	closedPartiallyTrackedMapTypeView_ = new AMGitHubRepositoryIssueMapTypeView(repository_, "Partially Tracked [Closed]", AMGitHubRepository::PartiallyTrackedClosedMap);
	closedFullyTrackedMapTypeView_ = new AMGitHubRepositoryIssueMapTypeView(repository_, "Fully Tracked [Closed]", AMGitHubRepository::FullyTrackedClosedMap);

	closedCompleteMapTypeView_ = new AMGitHubRepositoryIssueMapTypeView(repository_, "Completed [Closed]", AMGitHubRepository::CompleteClosedMap);
	closedMissingEstimateMapTypeView_ = new AMGitHubRepositoryIssueMapTypeView(repository_, "Missing Estimates [Closed]", AMGitHubRepository::MissingEstimateClosedMap);
	closedMissingActualMapTypeView_ = new AMGitHubRepositoryIssueMapTypeView(repository_, "Missing Actual [Closed]", AMGitHubRepository::MissingActualClosedMap);
	closedMissingTimeReportMapTypeView_ = new AMGitHubRepositoryIssueMapTypeView(repository_, "Missing Time Report [Closed]", AMGitHubRepository::MissingTimeReportClosedMap);
	closedSpecifiedMapTypeView_ = new AMGitHubRepositoryIssueMapTypeView(repository_, "Fully Specified [Closed]", AMGitHubRepository::FullySpecifiedClosedMap);

	allIssueMapTypeViews_.append(allIssuesMapTypeView_);
	allIssueMapTypeViews_.append(openIssuesMapTypeView_);
	allIssueMapTypeViews_.append(closedIssuesMapTypeView_);

	allIssueMapTypeViews_.append(openSpecifiedMapTypeView_);
	allIssueMapTypeViews_.append(openMissingEstimateMapTypeView_);

	allIssueMapTypeViews_.append(closedUntrackedMapTypeView_);
	allIssueMapTypeViews_.append(closedPartiallyTrackedMapTypeView_);
	allIssueMapTypeViews_.append(closedFullyTrackedMapTypeView_);

	allIssueMapTypeViews_.append(closedCompleteMapTypeView_);
	allIssueMapTypeViews_.append(closedMissingEstimateMapTypeView_);
	allIssueMapTypeViews_.append(closedMissingActualMapTypeView_);
	allIssueMapTypeViews_.append(closedMissingTimeReportMapTypeView_);
	allIssueMapTypeViews_.append(closedSpecifiedMapTypeView_);

	for(int x = 0, size = allIssueMapTypeViews_.count(); x < size; x++)
		mainFL_->addRow(allIssueMapTypeViews_.at(x)->issuesMapTypeString(), allIssueMapTypeViews_.at(x));

	issuesForReviewButton_ = new QPushButton("Issues to Review");
	connect(issuesForReviewButton_, SIGNAL(clicked()), this, SLOT(onIssuesForReviewButtonClicked()));
	mainFL_->addRow("Review", issuesForReviewButton_);

	setLayout(mainFL_);
}

#include <QDebug>
void AMGitHubRepositoryView::onIssuesForReviewButtonClicked()
{
	QStringList labelsToFilter;
	labelsToFilter << "Project Tracking Disabled" << "Investigation Required" << "PR - Work In Progress" << "PR - Awaiting Testing" << "On The Radar";

	QList<int> issueForReview;
	issueForReview.append(AMGitHubRepository::filterForLabels(repository_->missingEstimateClosedIssues(), labelsToFilter).keys());
	issueForReview.append(AMGitHubRepository::filterForLabels(repository_->missingActualClosedIssues(), labelsToFilter).keys());
	issueForReview.append(AMGitHubRepository::filterForLabels(repository_->missingTimeClosedIssues(), labelsToFilter).keys());
	issueForReview.append(AMGitHubRepository::filterForLabels(repository_->missingEstimateOpenIssues(), labelsToFilter).keys());

	QList<int> printedIssues;

	QString outputString;
	for(int x = 0, size = issueForReview.count(); x < size; x++){
		if(!printedIssues.contains(issueForReview.at(x))){
			printedIssues.append(issueForReview.at(x));
			outputString.append(QString("github.com/acquaman/acquaman/issues/%1 ").arg(issueForReview.at(x)));
		}
	}

	qDebug() << "Total Count: " << printedIssues.count();
	qDebug() << outputString;
}

AMGitHubIssueMapView::AMGitHubIssueMapView(const QMap<int, AMGitHubIssue *> *issueMap, QWidget *parent) :
	QGroupBox(parent)
{
	issueMap_ = issueMap;

	QString issueListString;
	QMap<int, AMGitHubIssue*>::const_iterator i = issueMap_->constBegin();
	while (i != issueMap_->constEnd()) {
		issueListString.append(QString("%1, ").arg(i.key()));
		i++;
	}
	issueListString.remove(issueListString.size()-2, 2);
	QLabel *issueList = new QLabel(issueListString);

	QHBoxLayout *issueListHL = new QHBoxLayout();
	issueListHL->addWidget(issueList);
	setLayout(issueListHL);
}

AMGitHubRepositoryIssueMapTypeView::AMGitHubRepositoryIssueMapTypeView(AMGitHubRepository *repository, const QString &issuesMapTypeString, AMGitHubRepository::IssueMapType issuesMapType, QWidget *parent) :
	QWidget(parent)
{
	repository_ = repository;
	issuesMapTypeString_ = issuesMapTypeString;
	issuesMapType_ = issuesMapType;
	mapView_ = 0;

	QHBoxLayout *mainHL = new QHBoxLayout();
	countLabel_ = new QLabel(QString("%1").arg(repository_->issuesCount(issuesMapType_)));
	moreInfoButton_ = new QPushButton("More Info");
	mainHL->addWidget(countLabel_);
	mainHL->addWidget(moreInfoButton_);

	setLayout(mainHL);
	connect(moreInfoButton_, SIGNAL(clicked()), this, SLOT(onMoreInfoButtonClicked()));
}

void AMGitHubRepositoryIssueMapTypeView::onMoreInfoButtonClicked()
{
	if(mapView_){
		if(mapView_->isVisible())
			mapView_->close();
		mapView_->deleteLater();
		mapView_ = 0;
	}

	mapView_ = new AMGitHubIssueMapView(repository_->issuesMap(issuesMapType_));
	mapView_->setTitle(issuesMapTypeString_);
	mapView_->show();
}
