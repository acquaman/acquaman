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

	fullySpecifiedFamilies_ = new QMap<int, AMGitHubIssueFamily*>();
	completedFamilies_ = new QMap<int, AMGitHubIssueFamily*>();

	allMilestones_ = new QMap<int, AMGitHubMilestone*>();
}

const QMap<int, AMGitHubIssue*>* AMGitHubRepository::issuesMap(AMGitHubRepository::IssueMapType issuesMapType) const
{
	switch(issuesMapType){
	case AllIssuesMap:
		return allIssues();
	case OpenIssuesMap:
		return openIssues();
	case ClosedIssuesMap:
		return closedIssues();
	case FullySpecifiedOpenMap:
		return fullySpecifiedOpenIssues();
	case MissingEstimateOpenMap:
		return missingEstimateOpenIssues();
	case UntrackedClosedMap:
		return untrackedClosedIssues();
	case PartiallyTrackedClosedMap:
		return partiallyTrackedClosedIssues();
	case FullyTrackedClosedMap:
		return fullyTrackedClosedIssues();
	case CompleteClosedMap:
		return completeClosedIssues();
	case MissingEstimateClosedMap:
		return missingEstimateClosedIssues();
	case MissingActualClosedMap:
		return missingActualClosedIssues();
	case MissingTimeReportClosedMap:
		return missingTimeClosedIssues();
	case FullySpecifiedClosedMap:
		return fullySpecifiedClosedIssues();
	case InvalidIssuesMap:
	default:
		return 0;
	}
}

int AMGitHubRepository::issuesCount(AMGitHubRepository::IssueMapType issuesMapType) const
{
	switch(issuesMapType){
	case AllIssuesMap:
		return issueCount();
	case OpenIssuesMap:
		return openIssueCount();
	case ClosedIssuesMap:
		return closedIssueCount();
	case FullySpecifiedOpenMap:
		return fullySpecifiedOpenIssueCount();
	case MissingEstimateOpenMap:
		return missingEstimateOpenIssueCount();
	case UntrackedClosedMap:
		return completeUntrackedIssueCount();
	case PartiallyTrackedClosedMap:
		return trackedWithoutEstimateIssueCount();
	case FullyTrackedClosedMap:
		return fullyTrackedIssueCount();
	case CompleteClosedMap:
		return completeIssueCount();
	case MissingEstimateClosedMap:
		return missingEstimateClosedIssueCount();
	case MissingActualClosedMap:
		return missingActualClosedIssueCount();
	case MissingTimeReportClosedMap:
		return missingTimeClosedIssueCount();
	case FullySpecifiedClosedMap:
		return fullySpecifiedClosedIssueCount();
	case InvalidIssuesMap:
	default:
		return 0;
	}
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
		// Inline issues that are not pull request use the originating issue for both fields in the family
		if(!h.value()->isPullRequest() && h.value()->inlineIssue()){
			AMGitHubIssueFamily *oneIssueFamily = new AMGitHubIssueFamily(h.value(), h.value());
			allIssueFamilies_->insert(h.value()->issueNumber(), oneIssueFamily);
			if(allMilestones_->contains(h.value()->issueNumber()))
				allMilestones_->value(h.value()->issueNumber())->associateFamily(oneIssueFamily);
		}
		// Issues that aren't pull requests and aren't tracking disabled are the originating issue for a family (we'll iterate again to figure out if they have an associated pull request)
		else if(!h.value()->isPullRequest() && !h.value()->projectTrackingDisabled()){
			AMGitHubIssueFamily *oneIssueFamily = new AMGitHubIssueFamily(h.value(), 0);
			allIssueFamilies_->insert(h.value()->issueNumber(), oneIssueFamily);
			if(allMilestones_->contains(h.value()->issueNumber()))
				allMilestones_->value(h.value()->issueNumber())->associateFamily(oneIssueFamily);
		}

		// Sort open issues into category maps
		if(h.value()->isOpen()){
			allOpenIssues_->insert(h.key(), h.value());
			if(h.value()->issueFullySpecifiedForState())
				fullySpecifiedOpenIssues_->insert(h.key(), h.value());
			if(h.value()->issueMissingEstimateComplexity())
				missingEstimateOpenIssues_->insert(h.key(), h.value());
		}
		// Sort closed issues into category maps
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

	QMap<int, AMGitHubIssue*>::const_iterator i = allIssues_->constBegin();
	while (i != allIssues_->constEnd()) {
		// Pull requests that have originating issues in the family map set themselves as the pull request
		if(i.value()->isPullRequest() && allIssueFamilies_->contains(i.value()->originatingIssueNumber()))
			allIssueFamilies_->value(i.value()->originatingIssueNumber())->setPullRequestIssue(i.value());

		i++;
	}

	QMap<int, AMGitHubIssueFamily*>::const_iterator j = allIssueFamilies_->constBegin();
	while(j != allIssueFamilies_->constEnd()) {

		if(j.value()->fullySpecifiedFamily())
			fullySpecifiedFamilies_->insert(j.value()->originatingIssue()->issueNumber(), j.value());
		if(j.value()->completedFamily())
			completedFamilies_->insert(j.value()->originatingIssue()->issueNumber(), j.value());

		j++;
	}

	qDebug() << "Repository loaded";
	emit repositoryLoaded();
}
