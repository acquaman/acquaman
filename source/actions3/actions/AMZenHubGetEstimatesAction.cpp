#include "AMZenHubGetEstimatesAction.h"

#include "actions3/actions/AMRestAction.h"

#include <QDebug>

#include "AMGithubProjectManager/AMGitHubIssue.h"
#include "util/AMErrorMonitor.h"

AMZenHubGetEstimatesAction::AMZenHubGetEstimatesAction(AMZenHubGetEstimatesActionInfo *info, QNetworkAccessManager *networkAccessManager, const QString &authorizationHeader, QMap<int, AMGitHubIssue*> *allIssues, QObject *parent) :
	AMAction3(info, parent)
{
	networkAccessManager_ = networkAccessManager;
	authorizationHeader_ = authorizationHeader;
	restAction_ = 0;

	allIssues_ = allIssues;
}

AMZenHubGetEstimatesAction::AMZenHubGetEstimatesAction(const AMZenHubGetEstimatesAction &other) :
	AMAction3(other)
{
	networkAccessManager_ = other.networkAccessManager();
	authorizationHeader_ = other.authorizationHeader();
	restAction_ = 0;

	allIssues_ = other.allIssues_;
}

AMAction3* AMZenHubGetEstimatesAction::createCopy() const
{
	return new AMZenHubGetEstimatesAction(*this);
}

void AMZenHubGetEstimatesAction::setNetworkAccessManager(QNetworkAccessManager *networkAccessManager)
{
	networkAccessManager_ = networkAccessManager;
}

void AMZenHubGetEstimatesAction::setAuthorizationHeader(const QString &authorizationHeader)
{
	authorizationHeader_ = authorizationHeader;
}

void AMZenHubGetEstimatesAction::setAllIssuesMap(QMap<int, AMGitHubIssue*> *allIssues)
{
	allIssues_ = allIssues;
}

void AMZenHubGetEstimatesAction::startImplementation()
{
	AMZenHubGetEstimatesActionInfo *actionInfo = zenhubGetEstimatesActionInfo();
	estimatesURLsCopy_ = actionInfo->estimateURLs();

	if(!networkAccessManager_){
		QString fundamentalFailureMessage = QString("There was an error starting the zenhub request, no network access manager was available.");
		AMErrorMon::alert(this, AMZENHUBGETESTIMATESACTION_CANNOT_START_WITHOUT_NETWORKACCESSMANAGER, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}

	if(actionInfo->owner().isEmpty()){
		QString fundamentalFailureMessage = QString("There was an error starting the request because the owner was empty.");
		AMErrorMon::alert(this, AMZENHUBGETESTIMATESACTION_CANNOT_START_WITH_EMPTY_OWNER, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}
	if(actionInfo->repo().isEmpty()){
		QString fundamentalFailureMessage = QString("There was an error starting the request because the repo was empty.");
		AMErrorMon::alert(this, AMZENHUBGETESTIMATESACTION_CANNOT_START_WITH_EMPTY_REPO, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}
	if(!allIssues_){
		QString fundamentalFailureMessage = QString("There was an error starting the request to %1/%2, no all issues map was available.").arg(actionInfo->owner()).arg(actionInfo->repo());
		AMErrorMon::alert(this, AMZENHUBGETESTIMATESACTION_CANNOT_START_WITHOUT_ALL_ISSUES_MAP, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}

	restAction_ = setupNextRestAction();
	if(!restAction_){
		QString fundamentalFailureMessage = QString("There was an error starting the zenhub request, the REST action was not instantiated.");
		AMErrorMon::alert(this, AMZENHUBGETESTIMATESACTION_CANNOT_START_WITHOUT_RESTACTION, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}

	connect(restAction_, SIGNAL(fullResponseReady(QVariant,QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onRestActionFullResponseReady(QVariant,QList<QNetworkReply::RawHeaderPair>)));
	connect(restAction_, SIGNAL(failed()), this, SLOT(onRestActionFailed()));
	restAction_->start();

	setStarted();
}

void AMZenHubGetEstimatesAction::pauseImplementation()
{
}

void AMZenHubGetEstimatesAction::resumeImplementation()
{
}

void AMZenHubGetEstimatesAction::cancelImplementation()
{
	setCancelled();
}

void AMZenHubGetEstimatesAction::skipImplementation(const QString &command)
{
	Q_UNUSED(command)
}

AMRestAction* AMZenHubGetEstimatesAction::setupNextRestAction()
{
	if(estimatesURLsCopy_.count() > 0){
		QString oneEstimateURL = estimatesURLsCopy_.takeFirst();
		qDebug() << "Moving on to " << oneEstimateURL << "(" << estimatesURLsCopy_.count() << " remaining)";
		lastURL_ = oneEstimateURL;

		AMRestActionInfo *getEstimatesActionInfo = new AMRestActionInfo(oneEstimateURL, AMRestActionInfo::GetRequest);
		getEstimatesActionInfo->setRawHeader("Authorization", authorizationHeader_.toLocal8Bit());
		QString xauthtokenData = "ced5ec812ee0ee5576da1345550d0110ba65946262541a1db6030ebda5dd4c0d95b31d24d954c563";
		getEstimatesActionInfo->setRawHeader("x-authentication-token", xauthtokenData.toLocal8Bit());
		return new AMRestAction(getEstimatesActionInfo, networkAccessManager_);
	}
	else
		return 0;
}

void AMZenHubGetEstimatesAction::onRestActionFullResponseReady(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs)
{
	disconnect(restAction_, SIGNAL(fullResponseReady(QVariant,QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onRestActionFullResponseReady(QVariant,QList<QNetworkReply::RawHeaderPair>)));
	disconnect(restAction_, SIGNAL(failed()), this, SLOT(onRestActionFailed()));

	int estimateValue = -1;
	QVariantList githubListReply = fullResponse.toList();
	for(int x = 0, size = githubListReply.count(); x < size; x++){
		if(githubListReply.at(x).toMap().value("selected").toBool())
			estimateValue = githubListReply.at(x).toMap().value("value").toInt();
	}

	int issueNumber = lastURL_.section("/", -2, -2).toInt();
	qDebug() << "The estimate for " << issueNumber << " is " << estimateValue;

	if(allIssues_->contains(issueNumber))
		allIssues_->value(issueNumber)->setEstimatedComplexityValue(AMGitHubIssue::estimatedComplexityFromInteger(estimateValue));
	else
		qDebug() << "Could not find " << issueNumber << " to set zenhub estimate for " << lastURL_;


	restAction_ = setupNextRestAction();
	if(!restAction_){
		setSucceeded();
		return;
	}

	connect(restAction_, SIGNAL(fullResponseReady(QVariant,QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onRestActionFullResponseReady(QVariant,QList<QNetworkReply::RawHeaderPair>)));
	connect(restAction_, SIGNAL(failed()), this, SLOT(onRestActionFailed()));
	restAction_->start();
}

void AMZenHubGetEstimatesAction::onRestActionFailed()
{
	setFailed();
}
