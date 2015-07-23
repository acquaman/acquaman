#include "BioXASScanConfiguration.h"

BioXASScanConfiguration::BioXASScanConfiguration()
{
	dbObject_ = new BioXASScanConfigurationDbObject;
}

BioXASScanConfiguration::BioXASScanConfiguration(const BioXASScanConfiguration &original)
{
	dbObject_ = new BioXASScanConfigurationDbObject(*original.dbObject());
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
