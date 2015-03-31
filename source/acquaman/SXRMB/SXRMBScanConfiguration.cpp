#include "SXRMBScanConfiguration.h"

SXRMBScanConfiguration::SXRMBScanConfiguration()
{
	dbObject_ = new SXRMBScanConfigurationDbObject;
	timeOffset_ = 0;
	totalTime_ = 0;
}

SXRMBScanConfiguration::SXRMBScanConfiguration(const SXRMBScanConfiguration &original)
{
	dbObject_ = new SXRMBScanConfigurationDbObject(*original.dbObject());
	timeOffset_ = original.timeOffset();
	totalTime_ = original.totalTime();
}

SXRMBScanConfiguration::~SXRMBScanConfiguration()
{
	dbObject_->deleteLater();
}

void SXRMBScanConfiguration::dbWriteScanConfigurationDbObject(AMDbObject *object)
{
	SXRMBScanConfigurationDbObject *dbo;

	if ((dbo = qobject_cast<SXRMBScanConfigurationDbObject *>(object)))
		dbObject_ = dbo;

	else
		object->deleteLater();
}

bool SXRMBScanConfiguration::powerOnTEYHVControl() const {
	// protection to make sure we won't power on TEY HV control by accident for Ambiant endstation
	if (endstation() == SXRMB::AmbiantWithGasChamber || endstation() == SXRMB::AmbiantWithoutGasChamber)
		return false;

	return powerOnTEYHVControl_;
}

double SXRMBScanConfiguration::totalTime(bool recompute)
{
	if (recompute)
		computeTotalTimeImplementation();

	return totalTime_;
}

QString SXRMBScanConfiguration::regionsOfInterestHeaderString(const QList<AMRegionOfInterest *> &regions) const
{
	QString string = "";

	if (!regions.isEmpty()){

		string.append("\nRegions Of Interest\n");

		foreach (AMRegionOfInterest *region, regions)
			string.append(QString("%1\t%2 eV\t%3 eV\n").arg(region->name()).arg(region->lowerBound()).arg(region->upperBound()));
	}

	return string;
}
