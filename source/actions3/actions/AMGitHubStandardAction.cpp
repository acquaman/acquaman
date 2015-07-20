#include "AMGitHubStandardAction.h"

AMGitHubStandardAction::AMGitHubStandardAction(AMActionInfo3 *info, QNetworkAccessManager *networkAccessManager, const QString &authorizationHeader, QObject *parent) :
	AMAction3(info, parent)
{
	networkAccessManager_ = networkAccessManager;
	authorizationHeader_ = authorizationHeader;

	currentIssuesPage_ = 1;
	lastPage_ = false;
}

AMGitHubStandardAction::AMGitHubStandardAction(const AMGitHubStandardAction &other) :
	AMAction3(other)
{
	networkAccessManager_ = other.networkAccessManager();
	authorizationHeader_ = other.authorizationHeader();

	currentIssuesPage_ = 1;
	lastPage_ = false;
}

AMAction3* AMGitHubStandardAction::createCopy() const
{
	return new AMGitHubStandardAction(*this);
}

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
