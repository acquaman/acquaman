#include "AMGitHubGetIssuesAction.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>

#include "AMRestAction.h"
#include "AMGithubProjectManager/AMGitHubMilestone.h"
#include "util/AMErrorMonitor.h"

AMGitHubGetIssuesAction::AMGitHubGetIssuesAction(AMGitHubGetIssuesActionInfo *info, QNetworkAccessManager *networkAccessManager, const QString &authorizationHeader, QMap<int, AMGitHubIssue*> *allIssues, QMap<int, AMGitHubMilestone*> *allMilestones, QObject *parent) :
	AMAction3(info, parent)
{
	networkAccessManager_ = networkAccessManager;
	authorizationHeader_ = authorizationHeader;

	allIssues_ = allIssues;
	allMilestones_ = allMilestones;

	currentIssuesPage_ = 1;
	lastPage_ = false;
}

AMGitHubGetIssuesAction::AMGitHubGetIssuesAction(const AMGitHubGetIssuesAction &other) :
	AMAction3(other)
{
	networkAccessManager_ = other.networkAccessManager();
	authorizationHeader_ = other.authorizationHeader();

	allIssues_ = other.allIssues_;
	allMilestones_ = other.allMilestones_;

	currentIssuesPage_ = 1;
	lastPage_ = false;
}

AMAction3* AMGitHubGetIssuesAction::createCopy() const
{
	return new AMGitHubGetIssuesAction(*this);
}

void AMGitHubGetIssuesAction::setNetworkAccessManager(QNetworkAccessManager *networkAccessManager)
{
	networkAccessManager_ = networkAccessManager;
}

void AMGitHubGetIssuesAction::setAuthorizationHeader(const QString &authorizationHeader)
{
	authorizationHeader_ = authorizationHeader;
}

void AMGitHubGetIssuesAction::setAllIssuesMap(QMap<int, AMGitHubIssue*> *allIssues)
{
	allIssues_ = allIssues;
}

void AMGitHubGetIssuesAction::setAllMilestonesMap(QMap<int, AMGitHubMilestone*> *allMilestones)
{
	allMilestones_ = allMilestones;
}

void AMGitHubGetIssuesAction::startImplementation()
{
	AMGitHubGetIssuesActionInfo *actionInfo = githubGetIssueActionInfo();

	if(actionInfo->owner().isEmpty()){
		QString fundamentalFailureMessage = QString("There was an error starting the request because the owner was empty.");
		AMErrorMon::alert(this, AMGITHUBGETISSUESACTION_CANNOT_START_WITH_EMPTY_OWNER, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}
	if(actionInfo->repo().isEmpty()){
		QString fundamentalFailureMessage = QString("There was an error starting the request because the repo was empty.");
		AMErrorMon::alert(this, AMGITHUBGETISSUESACTION_CANNOT_START_WITH_EMPTY_REPO, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}
	if(!networkAccessManager_){
		QString fundamentalFailureMessage = QString("There was an error starting the request to %1/%2, no network access manager was available.").arg(actionInfo->owner()).arg(actionInfo->repo());
		AMErrorMon::alert(this, AMGITHUBGETISSUESACTION_CANNOT_START_WITHOUT_NETWORKACCESSMANAGER, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}
	if(!allIssues_){
		QString fundamentalFailureMessage = QString("There was an error starting the request to %1/%2, no all issues map was available.").arg(actionInfo->owner()).arg(actionInfo->repo());
		AMErrorMon::alert(this, AMGITHUBGETISSUESACTION_CANNOT_START_WITHOUT_ALL_ISSUES_MAP, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}
	if(!allMilestones_){
		QString fundamentalFailureMessage = QString("There was an error starting the request to %1/%2, no all milestones map was available.").arg(actionInfo->owner()).arg(actionInfo->repo());
		AMErrorMon::alert(this, AMGITHUBGETISSUESACTION_CANNOT_START_WITHOUT_ALL_MILESTONES_MAP, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}

	QString issuesString = QString("https://api.github.com/repos/%1/%2/issues?filter=all&state=%3&page=%4&per_page=30").arg(actionInfo->owner()).arg(actionInfo->repo()).arg(actionInfo->issueStateString()).arg(currentIssuesPage_);
	AMRestActionInfo *getAllIssuesActionInfo = new AMRestActionInfo(issuesString, AMRestActionInfo::GetRequest);
	getAllIssuesActionInfo->setRawHeader("Authorization", authorizationHeader_.toLocal8Bit());
	AMRestAction *getAllIssuesAction = new AMRestAction(getAllIssuesActionInfo, networkAccessManager_);

	connect(getAllIssuesAction, SIGNAL(fullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onGetAllIssuesFullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)));
	connect(getAllIssuesAction, SIGNAL(failed()), this, SLOT(onGetAllIssuesFailed()));
	getAllIssuesAction->start();

	setStarted();
}

void AMGitHubGetIssuesAction::pauseImplementation()
{
}

void AMGitHubGetIssuesAction::resumeImplementation()
{
}

void AMGitHubGetIssuesAction::cancelImplementation()
{
	setCancelled();
}

void AMGitHubGetIssuesAction::skipImplementation(const QString &command)
{
	Q_UNUSED(command)
}

void AMGitHubGetIssuesAction::onGetAllIssuesFullResponseReady(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs){
	QVariant allIssuesResponse = fullResponse;

	int lastPageNumber = -1;
	int nextPageNumber = -1;

	for(int x = 0; x < headerPairs.count(); x++){
		if(headerPairs.at(x).first == "Link"){
			QString linkHeader = headerPairs.at(x).second;
			QStringList linkHeaderItems = linkHeader.split(',');
			for(int y = 0; y < linkHeaderItems.count(); y++){
				if(linkHeaderItems.at(y).contains("; rel=\"last\""))
					lastPageNumber = AMRestAction::pageNumberFromURLString(linkHeaderItems.at(y));
				if(linkHeaderItems.at(y).contains("; rel=\"next\""))
					nextPageNumber = AMRestAction::pageNumberFromURLString(linkHeaderItems.at(y));
			}
		}
	}

	if(allIssuesResponse.canConvert(QVariant::List)){
		QVariantMap jsonMap;
		QVariantList githubListReply = allIssuesResponse.toList();
		if(githubListReply.at(0).canConvert(QVariant::Map)){
			for(int x = 0; x < githubListReply.count(); x++){
				jsonMap = githubListReply.at(x).toMap();

				AMGitHubMilestone *associatedMilestone = 0;
				if(jsonMap.contains("milestone") && !jsonMap.value("milestone").toMap().value("title").toString().isEmpty()){
					int milestoneNumber = -1;
					if(jsonMap.value("milestone").toMap().value("number").canConvert<int>())
						milestoneNumber = jsonMap.value("milestone").toMap().value("number").toInt();
					if(milestoneNumber > 0 && !allMilestones_->contains(milestoneNumber)){
						AMGitHubMilestone *oneMilestone = new AMGitHubMilestone(jsonMap.value("milestone").toMap());
						allMilestones_->insert(oneMilestone->number(), oneMilestone);

						associatedMilestone = oneMilestone;
					}
					else if(milestoneNumber > 0 && allMilestones_->contains(milestoneNumber))
						associatedMilestone = allMilestones_->value(milestoneNumber);
				}

				AMGitHubIssue *oneIssue = new AMGitHubIssue(jsonMap);
				allIssues_->insert(oneIssue->issueNumber(), oneIssue);

				if(associatedMilestone && !oneIssue->projectTrackingDisabled())
					associatedMilestone->appendAssociatedIssue(oneIssue);
			}
		}
	}

	if(!lastPage_){
		AMGitHubGetIssuesActionInfo *actionInfo = githubGetIssueActionInfo();

		currentIssuesPage_ = nextPageNumber;
		if(nextPageNumber == lastPageNumber)
			lastPage_ = true;

//		if(nextPageNumber == 2)
//			lastPage_ = true;

		qDebug() << "More more issues to fetch, going to page " << currentIssuesPage_;

		QString issuesString = QString("https://api.github.com/repos/%1/%2/issues?filter=all&state=%3&page=%4&per_page=30").arg(actionInfo->owner()).arg(actionInfo->repo()).arg(actionInfo->issueStateString()).arg(currentIssuesPage_);
		AMRestActionInfo *getAllIssuesActionInfo = new AMRestActionInfo(issuesString, AMRestActionInfo::GetRequest);
		getAllIssuesActionInfo->setRawHeader("Authorization", authorizationHeader_.toLocal8Bit());
		AMRestAction *getAllIssuesAction = new AMRestAction(getAllIssuesActionInfo, networkAccessManager_);

		connect(getAllIssuesAction, SIGNAL(fullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onGetAllIssuesFullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)));
		connect(getAllIssuesAction, SIGNAL(failed()), this, SLOT(onGetAllIssuesFailed()));
		getAllIssuesAction->start();
	}
	else{

		setSucceeded();
//		QMap<int, AMGitHubIssue*>::const_iterator j = allIssues_->constBegin();
//		while (j != allIssues_->constEnd()) {
//			if(j.value()->isPullRequest() && allIssues_->contains(j.value()->originatingIssueNumber()))
//				j.value()->setOriginatingIssue(allIssues_->value(j.value()->originatingIssueNumber()));

//			if( (!j.value()->isPullRequest()) && (j.value()->commentCount() > 0) && (!j.value()->commentsURL().isEmpty()) && (j.value()->complexityValue() != AMGitHubIssue::InvalidComplexity) && (j.value()->issueState() == AMGitHubIssue::ClosedState) ){
//				commentURLs_.append(j.value()->commentsURL());
//			}
//			j++;
//		}

//		if(commentURLs_.count() > 0){
//			QString oneCommentURL = commentURLs_.takeFirst();

//			AMRestActionInfo *getOneIssueCommentsActionInfo = new AMRestActionInfo(oneCommentURL, AMRestActionInfo::GetRequest);
//			getOneIssueCommentsActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
//			AMRestAction *getOneIssueCommentsAction = new AMRestAction(getOneIssueCommentsActionInfo, manager_);

//			connect(getOneIssueCommentsAction, SIGNAL(fullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onGetOneIssueCommentsReturned(QVariant, QList<QNetworkReply::RawHeaderPair>)));
//			getOneIssueCommentsAction->start();
//		}
	}
}

void AMGitHubGetIssuesAction::onGetAllIssuesFailed()
{
	setFailed();
}
