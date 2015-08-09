#include "AMGitHubGetIssuesAction.h"

#include <QDebug>

#include "AMRestAction.h"
#include "AMGithubProjectManager/AMGitHubMilestone.h"
#include "util/AMErrorMonitor.h"

AMGitHubGetIssuesAction::AMGitHubGetIssuesAction(AMGitHubGetIssuesActionInfo *info, QNetworkAccessManager *networkAccessManager, const QString &authorizationHeader, QMap<int, AMGitHubIssue*> *allIssues, QMap<int, AMGitHubMilestone*> *allMilestones, QObject *parent) :
	AMGitHubStandardAction(info, networkAccessManager, authorizationHeader, parent)
{
	allIssues_ = allIssues;
	allMilestones_ = allMilestones;
}

AMGitHubGetIssuesAction::AMGitHubGetIssuesAction(const AMGitHubGetIssuesAction &other) :
	AMGitHubStandardAction(other)
{
	allIssues_ = other.allIssues_;
	allMilestones_ = other.allMilestones_;
}

AMAction3* AMGitHubGetIssuesAction::createCopy() const
{
	return new AMGitHubGetIssuesAction(*this);
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

	AMGitHubStandardAction::startImplementation();
}

AMRestAction* AMGitHubGetIssuesAction::setupNextRestAction()
{
	if(lastPage_)
		return 0;

	AMGitHubGetIssuesActionInfo *actionInfo = githubGetIssueActionInfo();
	int currentPage;
	if(headerLastPageNumber_ == -1 && headerNextPageNumber_ == -1)
		currentPage = 1;
	else
		currentPage = headerNextPageNumber_;

	QString issuesString = QString("https://api.github.com/repos/%1/%2/issues?filter=all&state=%3&page=%4&per_page=30").arg(actionInfo->owner()).arg(actionInfo->repo()).arg(actionInfo->issueStateString()).arg(currentPage);
	AMRestActionInfo *getAllIssuesActionInfo = new AMRestActionInfo(issuesString, AMRestActionInfo::GetRequest);
	getAllIssuesActionInfo->setRawHeader("Authorization", authorizationHeader_.toLocal8Bit());

	return new AMRestAction(getAllIssuesActionInfo, networkAccessManager_);
}

void AMGitHubGetIssuesAction::restResponsePreImplementation(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs)
{
	Q_UNUSED(fullResponse)
	Q_UNUSED(headerPairs)
}

void AMGitHubGetIssuesAction::restResponseImplementation(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs)
{
	Q_UNUSED(headerPairs)

	qDebug() << "More more issues to fetch, going to page " << headerNextPageNumber_;

	QVariant allIssuesResponse = fullResponse;

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
					associatedMilestone->associateIssue(oneIssue);
			}
		}
	}
}
