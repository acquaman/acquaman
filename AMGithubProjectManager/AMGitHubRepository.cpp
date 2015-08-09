#include "AMGitHubRepository.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMRestAction.h"
#include "actions3/actions/AMGitHubGetIssuesAction.h"
#include "actions3/actions/AMGitHubGetCommentsAction.h"
#include "actions3/actions/AMZenHubGetEstimatesAction.h"

#include "AMGitHubMilestone.h"
#include "AMGitHubComplexityManager.h"

#include <QDebug>

AMGitHubRepository::AMGitHubRepository(const QString &owner, const QString &repo, QNetworkAccessManager *manager, const QString &accessToken, QObject *parent) :
	QObject(parent)
{
	owner_ = owner;
	repo_ = repo;

	manager_ = manager;
	accessToken_ = accessToken;

	allIssues_ = new QMap<int, AMGitHubIssue*>();

	allOpenIssues_ = new QMap<int, AMGitHubIssue*>();
	fullySpecifiedOpenIssues_ = new QMap<int, AMGitHubIssue*>();
	missingEstimateOpenIssues_ = new QMap<int, AMGitHubIssue*>();

	allClosedIssues_ = new QMap<int, AMGitHubIssue*>();
	completelyUntrackedIssues_ = new QMap<int, AMGitHubIssue*>();
	trackedWithoutEstimateIssues_ = new QMap<int, AMGitHubIssue*>();
	fullyTrackedIssues_ = new QMap<int, AMGitHubIssue*>();
	completeIssues_ = new QMap<int, AMGitHubIssue*>();
	missingEstimateClosedIssues_ = new QMap<int, AMGitHubIssue*>();
	missingActualClosedIssues_ = new QMap<int, AMGitHubIssue*>();
	missingTimeClosedIssues_ = new QMap<int, AMGitHubIssue*>();
	fullySpecifiedClosedIssues_ = new QMap<int, AMGitHubIssue*>();


	allIssueFamilies_ = new QMap<int, AMGitHubIssueFamily*>();
	allMilestones_ = new QMap<int, AMGitHubMilestone*>();
}

void AMGitHubRepository::initiateRepositoryLoading()
{
	AMGitHubGetIssuesActionInfo *getAllIssuesActionInfo = new AMGitHubGetIssuesActionInfo(owner_, repo_, AMGitHubGetIssuesActionInfo::AllIssues);
	AMGitHubGetIssuesAction *getAllIssuesAction = new AMGitHubGetIssuesAction(getAllIssuesActionInfo, manager_, accessToken_, allIssues_, allMilestones_);
	connect(getAllIssuesAction, SIGNAL(succeeded()), this, SLOT(onGetAllIssuesActionSucceeded()));
	getAllIssuesAction->start();
}

void AMGitHubRepository::onGetAllIssuesActionSucceeded()
{
	QMap<int, AMGitHubIssue*>::const_iterator j = allIssues_->constBegin();
	while (j != allIssues_->constEnd()) {
		if(j.value()->isPullRequest() && allIssues_->contains(j.value()->originatingIssueNumber()))
			j.value()->setOriginatingIssue(allIssues_->value(j.value()->originatingIssueNumber()));

		// Not ProjectTrackingDisabled, Has Comments, Comments URL not empty, Is closed
		if( (!j.value()->projectTrackingDisabled()) && (j.value()->commentCount() > 0) && (!j.value()->commentsURL().isEmpty()) && (j.value()->issueState() == AMGitHubIssue::ClosedState) ){
			commentURLs_.append(j.value()->commentsURL());
		}
		j++;
	}

	AMGitHubGetCommentsActionInfo *getAllCommentsActionInfo = new AMGitHubGetCommentsActionInfo(owner_, repo_, commentURLs_);
	AMGitHubGetCommentsAction *getAllCommentsAction = new AMGitHubGetCommentsAction(getAllCommentsActionInfo, manager_, accessToken_, allIssues_);
	connect(getAllCommentsAction, SIGNAL(succeeded()), this, SLOT(onGetAllCommentsActionSucceeded()));
	getAllCommentsAction->start();
}

void AMGitHubRepository::onGetAllCommentsActionSucceeded(){

	QStringList allEstimateURLs;
	QMap<int, AMGitHubIssue*>::const_iterator h = allIssues_->constBegin();
	while(h != allIssues_->constEnd()){
		if(!h.value()->projectTrackingDisabled()){
			// do this elsewhere
//			if(h.value()->timeEstimateString().isNull())
//				qDebug() << h.value()->issueNumber() << " has no time estimate";
//			else
//				qDebug() << h.value()->issueNumber() << " time estimate: " << h.value()->timeEstimateString();

			allEstimateURLs << QString("https://api.zenhub.io/v2/%1/%2/issues/%3/estimates").arg(owner_).arg(repo_).arg(h.value()->issueNumber());
		}

		h++;
	}

	AMZenHubGetEstimatesActionInfo *getAllEstimatesActionInfo = new AMZenHubGetEstimatesActionInfo("acquaman", "acquaman", allEstimateURLs);
	AMZenHubGetEstimatesAction *getAllEstimatesAction = new AMZenHubGetEstimatesAction(getAllEstimatesActionInfo, manager_, accessToken_, allIssues_);
	connect(getAllEstimatesAction, SIGNAL(succeeded()), this, SLOT(onGetAllZenhubEstimatesSucceeded()));
	getAllEstimatesAction->start();
}

void AMGitHubRepository::onGetAllZenhubEstimatesSucceeded()
{

	QMap<int, AMGitHubIssue*>::const_iterator h = allIssues_->constBegin();
	while(h != allIssues_->constEnd()){
		if(h.value()->isOpen()){
			allOpenIssues_->insert(h.key(), h.value());
			if(h.value()->issueFullySpecifiedForState())
				fullySpecifiedOpenIssues_->insert(h.key(), h.value());
			if(h.value()->issueMissingEstimateComplexity())
				missingEstimateOpenIssues_->insert(h.key(), h.value());
		}
		if(h.value()->isClosed()){
			allClosedIssues_->insert(h.key(), h.value());
			if(h.value()->issueCompletelyUntracked())
				completelyUntrackedIssues_->insert(h.key(), h.value());
			if(h.value()->issueTrackedWithoutEstimates())
				trackedWithoutEstimateIssues_->insert(h.key(), h.value());
			if(h.value()->fullyTrackedIssue())
				fullyTrackedIssues_->insert(h.key(), h.value());
			if(h.value()->completeIssue())
				completeIssues_->insert(h.key(), h.value());
			if(h.value()->issueMissingEstimateComplexity())
				missingEstimateClosedIssues_->insert(h.key(), h.value());
			if(h.value()->issueMissingActualComplexity())
				missingActualClosedIssues_->insert(h.key(), h.value());
			if(h.value()->issueMissingTimeReporting())
				missingTimeClosedIssues_->insert(h.key(), h.value());
			if(h.value()->issueFullySpecifiedForState())
				fullySpecifiedClosedIssues_->insert(h.key(), h.value());
		}

		h++;
	}

	qDebug() << "Repository loaded";
	emit repositoryLoaded();
}
