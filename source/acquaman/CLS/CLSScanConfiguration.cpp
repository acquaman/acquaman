#include "CLSScanConfiguration.h"

CLSScanConfiguration::CLSScanConfiguration(CLSScanConfigurationDbObject *dbObject)
{
	configurationDbObject_ = dbObject;
}

CLSScanConfiguration::~CLSScanConfiguration()
{
	if (configurationDbObject_) {
		configurationDbObject_->deleteLater();
		configurationDbObject_ = 0;
	}
}

void CLSScanConfiguration::dbWriteScanConfigurationDbObject(AMDbObject *object)
{
	CLSScanConfigurationDbObject *dbo;

	if ((dbo = qobject_cast<CLSScanConfigurationDbObject *>(object)))
		configurationDbObject_ = dbo;

	else
		object->deleteLater();
}


QString CLSScanConfiguration::regionsOfInterestHeaderString(const QList<AMRegionOfInterest *> &regions) const
{
	QString string = "";

	if (!regions.isEmpty()){

		string.append("\nRegions Of Interest\n");

		foreach (AMRegionOfInterest *region, regions)
			string.append(QString("%1\t%2 eV\t%3 eV\n").arg(region->name()).arg(region->lowerBound()).arg(region->upperBound()));
	}

	return string;
}
