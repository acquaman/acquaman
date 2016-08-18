#include "CLSScanConfiguration.h"

CLSScanConfiguration::CLSScanConfiguration(CLSScanConfigurationDbObject *dbObject)
{
	scanConfigurationDbObject_ = dbObject;
}

CLSScanConfiguration::~CLSScanConfiguration()
{
	if (scanConfigurationDbObject_) {
		scanConfigurationDbObject_->deleteLater();
		scanConfigurationDbObject_ = 0;
	}
}

void CLSScanConfiguration::dbWriteScanConfigurationDbObject(AMDbObject *dbObject)
{
	CLSScanConfigurationDbObject *clsScanConfigurationDbObject = qobject_cast<CLSScanConfigurationDbObject *>(dbObject);

	if (clsScanConfigurationDbObject)
		scanConfigurationDbObject_ = clsScanConfigurationDbObject;

	else
		dbObject->deleteLater();
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
