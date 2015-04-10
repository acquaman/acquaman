#include "BioXASScanConfiguration.h"

BioXASScanConfiguration::BioXASScanConfiguration()
{
	dbObject_ = new BioXASScanConfigurationDbObject;
	timeOffset_ = 0.0;
	totalTime_ = 0.0;
}

BioXASScanConfiguration::BioXASScanConfiguration(const BioXASScanConfiguration &original)
{
	dbObject_ = new BioXASScanConfigurationDbObject(*original.dbObject());
	timeOffset_ = original.timeOffset();
	totalTime_ = original.totalTime();
}

BioXASScanConfiguration::~BioXASScanConfiguration()
{

}

void BioXASScanConfiguration::dbWriteScanConfigurationDbObject(AMDbObject *object)
{
	BioXASScanConfigurationDbObject *dbo;

	if ((dbo = qobject_cast<BioXASScanConfigurationDbObject *>(object)))
		dbObject_ = dbo;
}

double BioXASScanConfiguration::totalTime(bool recompute)
{
	if (recompute)
		computeTotalTimeImplementation();
	return totalTime_;
}

QString BioXASScanConfiguration::regionsOfInterestHeaderString(const QList<AMRegionOfInterest *> &regions) const
{
	QString string = "";

	if (!regions.isEmpty()){

		string.append("\nRegions Of Interest\n");

		foreach (AMRegionOfInterest *region, regions)
			string.append(QString("%1\t%2 eV\t%3 eV\n").arg(region->name()).arg(region->lowerBound()).arg(region->upperBound()));
	}

	return string;
}
