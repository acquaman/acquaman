#include "AMZenHubGetEstimatesActionInfo.h"

AMZenHubGetEstimatesActionInfo::AMZenHubGetEstimatesActionInfo(const QString &owner, const QString &repo, const QStringList &estimateURLs, QObject *parent) :
	AMActionInfo3("REST Zenhub Estimates Call", "REST Zenhub Estimates Call", ":/user-away.png", parent)
{
	owner_ = owner;
	repo_ = repo;
	estimateURLs_ = estimateURLs;

	setShortDescription("REST Zenhub Estimates Call");
	setLongDescription("REST Zenhub Estimates Call");
}

AMZenHubGetEstimatesActionInfo::AMZenHubGetEstimatesActionInfo(const AMZenHubGetEstimatesActionInfo &other) :
	AMActionInfo3(other)
{
	owner_ = other.owner();
	repo_ = other.repo();
	estimateURLs_ = other.estimateURLs();
}

AMActionInfo3 *AMZenHubGetEstimatesActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMZenHubGetEstimatesActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMZenHubGetEstimatesActionInfo::setOwner(const QString &owner)
{
	owner_ = owner;
}

void AMZenHubGetEstimatesActionInfo::setRepo(const QString &repo)
{
	repo_ = repo;
}

void AMZenHubGetEstimatesActionInfo::setEstimateURLs(const QStringList &estimateURLs)
{
	estimateURLs_ = estimateURLs;
}
