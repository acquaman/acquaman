#include "AMGitHubGetCommentsAction.h"

#include <QDebug>

#include "AMRestAction.h"
#include "AMGithubProjectManager/AMGitHubMilestone.h"
#include "util/AMErrorMonitor.h"

AMGitHubGetCommentsAction::AMGitHubGetCommentsAction(AMGitHubGetCommentsActionInfo *info, QNetworkAccessManager *networkAccessManager, const QString &authorizationHeader, QMap<int, AMGitHubIssue*> *allIssues, QObject *parent) :
	AMGitHubStandardAction(info, networkAccessManager, authorizationHeader, parent)
{
	allIssues_ = allIssues;
}

AMGitHubGetCommentsAction::AMGitHubGetCommentsAction(const AMGitHubGetCommentsAction &other) :
	AMGitHubStandardAction(other)
{
	allIssues_ = other.allIssues_;
}

AMAction3* AMGitHubGetCommentsAction::createCopy() const
{
	return new AMGitHubGetCommentsAction(*this);
}

void AMGitHubGetCommentsAction::setAllIssuesMap(QMap<int, AMGitHubIssue*> *allIssues)
{
	allIssues_ = allIssues;
}

void AMGitHubGetCommentsAction::startImplementation()
{
	AMGitHubGetCommentsActionInfo *actionInfo = githubGetCommentsActionInfo();

	if(actionInfo->owner().isEmpty()){
		QString fundamentalFailureMessage = QString("There was an error starting the request because the owner was empty.");
		AMErrorMon::alert(this, AMGITHUBGETCOMMENTSACTION_CANNOT_START_WITH_EMPTY_OWNER, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}
	if(actionInfo->repo().isEmpty()){
		QString fundamentalFailureMessage = QString("There was an error starting the request because the repo was empty.");
		AMErrorMon::alert(this, AMGITHUBGETCOMMENTSACTION_CANNOT_START_WITH_EMPTY_REPO, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}
	if(!allIssues_){
		QString fundamentalFailureMessage = QString("There was an error starting the request to %1/%2, no all issues map was available.").arg(actionInfo->owner()).arg(actionInfo->repo());
		AMErrorMon::alert(this, AMGITHUBGETCOMMENTSACTION_CANNOT_START_WITHOUT_ALL_ISSUES_MAP, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}

	commentURLsCopy_ = actionInfo->commentURLs();

	AMGitHubStandardAction::startImplementation();
}

AMRestAction* AMGitHubGetCommentsAction::setupNextRestAction()
{
	if(!lastPage_ && headerNextPageNumber_ != -1 && headerLastPageNumber_ != -1){
		qDebug() << "Not the last page, try " << headerNextURL_;
		AMRestActionInfo *getCommentsActionInfo = new AMRestActionInfo(headerNextURL_, AMRestActionInfo::GetRequest);
		getCommentsActionInfo->setRawHeader("Authorization", authorizationHeader_.toLocal8Bit());
		return new AMRestAction(getCommentsActionInfo, networkAccessManager_);
	}
	else if(commentURLsCopy_.count() > 0){
		lastPage_ = false;
		headerNextPageNumber_ = -1;
		headerLastPageNumber_ = -1;
		QString oneCommentURL = commentURLsCopy_.takeFirst();
		qDebug() << "Moving on to " << oneCommentURL;

		AMRestActionInfo *getCommentsActionInfo = new AMRestActionInfo(oneCommentURL, AMRestActionInfo::GetRequest);
		getCommentsActionInfo->setRawHeader("Authorization", authorizationHeader_.toLocal8Bit());
		return new AMRestAction(getCommentsActionInfo, networkAccessManager_);
	}
	else
		return 0;
}

void AMGitHubGetCommentsAction::restResponsePreImplementation(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs)
{
	Q_UNUSED(fullResponse)
	Q_UNUSED(headerPairs)
}

void AMGitHubGetCommentsAction::restResponseImplementation(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs)
{
	Q_UNUSED(headerPairs)

	QVariantList githubListReply = fullResponse.toList();
	int issueNumber = githubListReply.at(0).toMap().value("issue_url").toString().section("/", -1, -1).toInt();
	qDebug() << "Checking Issue " << issueNumber << "(" << commentURLsCopy_.count() << " comments remaining to fetch)";
	for(int x = 0, size = githubListReply.count(); x < size; x++){
		if(githubListReply.at(x).toMap().value("body").toString().contains("Time Estimate:")){
			QStringList splitCommentBody = githubListReply.at(x).toMap().value("body").toString().split("\n");
			for(int y = 0, ySize = splitCommentBody.count(); y < ySize; y++){
				if(splitCommentBody.at(y).contains("Time Estimate:")){
					QString timeEstimateString = splitCommentBody.at(y);
					timeEstimateString.remove("Time Estimate:");
					timeEstimateString = timeEstimateString.simplified();
					if(timeEstimateString.endsWith("."))
						timeEstimateString.remove(timeEstimateString.count()-1, 1);

					if(allIssues_->contains(issueNumber))
						allIssues_->value(issueNumber)->setTimeEstimateString(timeEstimateString);
				}
			}
		}
	}
}
