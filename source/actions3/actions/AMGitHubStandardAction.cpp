#include "AMGitHubStandardAction.h"

#include <QDebug>

#include "actions3/actions/AMRestAction.h"
#include "util/AMErrorMonitor.h"

AMGitHubStandardAction::AMGitHubStandardAction(AMActionInfo3 *info, QNetworkAccessManager *networkAccessManager, const QString &authorizationHeader, QObject *parent) :
	AMAction3(info, parent)
{
	networkAccessManager_ = networkAccessManager;
	authorizationHeader_ = authorizationHeader;

//	currentIssuesPage_ = 1;
	lastPage_ = false;

	headerLastPageNumber_ = -1;
	headerNextPageNumber_ = -1;

	restAction_ = 0;
}

AMGitHubStandardAction::AMGitHubStandardAction(const AMGitHubStandardAction &other) :
	AMAction3(other)
{
	networkAccessManager_ = other.networkAccessManager();
	authorizationHeader_ = other.authorizationHeader();

//	currentIssuesPage_ = 1;
	lastPage_ = false;

	headerLastPageNumber_ = -1;
	headerNextPageNumber_ = -1;

	restAction_ = 0;
}

//AMAction3* AMGitHubStandardAction::createCopy() const
//{
//	return new AMGitHubStandardAction(*this);
//}

void AMGitHubStandardAction::setNetworkAccessManager(QNetworkAccessManager *networkAccessManager)
{
	networkAccessManager_ = networkAccessManager;
}

void AMGitHubStandardAction::setAuthorizationHeader(const QString &authorizationHeader)
{
	authorizationHeader_ = authorizationHeader;
}

void AMGitHubStandardAction::startImplementation()
{
	restAction_ = setupNextRestAction();

	if(!networkAccessManager_){
		QString fundamentalFailureMessage = QString("There was an error starting the github request, no network access manager was available.");
		AMErrorMon::alert(this, AMGITHUBSTANDARDACTION_CANNOT_START_WITHOUT_NETWORKACCESSMANAGER, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}
	if(!restAction_){
		QString fundamentalFailureMessage = QString("There was an error starting the github request, the REST action was not instantiated.");
		AMErrorMon::alert(this, AMGITHUBSTANDARDACTION_CANNOT_START_WITHOUT_RESTACTION, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}


	connect(restAction_, SIGNAL(fullResponseReady(QVariant,QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onRestActionFullResponseReady(QVariant,QList<QNetworkReply::RawHeaderPair>)));
	connect(restAction_, SIGNAL(failed()), this, SLOT(onRestActionFailed()));
	restAction_->start();

	setStarted();
}

void AMGitHubStandardAction::pauseImplementation()
{
}

void AMGitHubStandardAction::resumeImplementation()
{
}

void AMGitHubStandardAction::cancelImplementation()
{
	setCancelled();
}

void AMGitHubStandardAction::skipImplementation(const QString &command)
{
	Q_UNUSED(command)
}

void AMGitHubStandardAction::onRestActionFullResponseReady(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs)
{
	disconnect(restAction_, SIGNAL(fullResponseReady(QVariant,QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onRestActionFullResponseReady(QVariant,QList<QNetworkReply::RawHeaderPair>)));
	disconnect(restAction_, SIGNAL(failed()), this, SLOT(onRestActionFailed()));

	restResponsePreImplementation(fullResponse, headerPairs);

	bool foundLinkHeader = false;

	for(int x = 0; x < headerPairs.count(); x++){
		if(headerPairs.at(x).first == "Link"){
//			qDebug() << "Found link header";
			foundLinkHeader = true;
			QString linkHeader = headerPairs.at(x).second;
			QStringList linkHeaderItems = linkHeader.split(',');
			for(int y = 0; y < linkHeaderItems.count(); y++){
				if(linkHeaderItems.at(y).contains("; rel=\"last\""))
					headerLastPageNumber_ = AMRestAction::pageNumberFromURLString(linkHeaderItems.at(y));
				if(linkHeaderItems.at(y).contains("; rel=\"next\"")){
					headerNextPageNumber_ = AMRestAction::pageNumberFromURLString(linkHeaderItems.at(y));
					headerNextURL_ = linkHeaderItems.at(y).section(';', 0, 0).remove('<').remove('>');
				}
			}
		}
	}
	if(!foundLinkHeader){
//		qDebug() << "Last page by virtue of no link header";
		lastPage_ = true;
	}

	restResponseImplementation(fullResponse, headerPairs);

	restAction_ = setupNextRestAction();
	if(restAction_){
		if((headerNextPageNumber_ != -1) && (headerLastPageNumber_ != -1) && (headerNextPageNumber_ == headerLastPageNumber_)){
//			qDebug() << "Last page by virtue of same next and last";
			lastPage_ = true;
		}

		connect(restAction_, SIGNAL(fullResponseReady(QVariant,QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onRestActionFullResponseReady(QVariant,QList<QNetworkReply::RawHeaderPair>)));
		connect(restAction_, SIGNAL(failed()), this, SLOT(onRestActionFailed()));
		restAction_->start();
	}
	else
		setSucceeded();

//	if(!lastPage_){
//		currentIssuesPage_ = nextPageNumber;
//		if(nextPageNumber == lastPageNumber)
//			lastPage_ = true;

////		if(nextPageNumber == 2)
////			lastPage_ = true;

//		setupRestAction();

//		connect(restAction_, SIGNAL(fullResponseReady(QVariant,QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onRestActionFullResponseReady(QVariant,QList<QNetworkReply::RawHeaderPair>)));
//		connect(restAction_, SIGNAL(failed()), this, SLOT(onRestActionFailed()));
//		restAction_->start();
//	}
//	else
//		setSucceeded();
}

void AMGitHubStandardAction::onRestActionFailed()
{
	setFailed();
}
