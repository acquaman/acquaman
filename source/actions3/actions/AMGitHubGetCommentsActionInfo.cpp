#include "AMGitHubGetCommentsActionInfo.h"

AMGitHubGetCommentsActionInfo::AMGitHubGetCommentsActionInfo(const QString &owner, const QString &repo, const QStringList &commentURLs, QObject *parent)
	: AMActionInfo3("REST Github Comments Call", "REST Github Comments Call", ":/user-away.png", parent)
{
	owner_ = owner;
	repo_ = repo;
	commentURLs_ = commentURLs;

	setShortDescription("REST Github Comments Call");
	setLongDescription("REST Github Comments Call");
}

AMGitHubGetCommentsActionInfo::AMGitHubGetCommentsActionInfo(const AMGitHubGetCommentsActionInfo &other)
	: AMActionInfo3(other)
{
	owner_ = other.owner();
	repo_ = other.repo();
	commentURLs_ = other.commentURLs();
}

AMActionInfo3 *AMGitHubGetCommentsActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMGitHubGetCommentsActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMGitHubGetCommentsActionInfo::setOwner(const QString &owner)
{
	owner_ = owner;
}

void AMGitHubGetCommentsActionInfo::setRepo(const QString &repo)
{
	repo_ = repo;
}

void AMGitHubGetCommentsActionInfo::setCommentURLs(const QStringList &commentURLs)
{
	commentURLs_ = commentURLs;
}
