#include "SXRMBScanConfiguration.h"

SXRMBScanConfiguration::SXRMBScanConfiguration()
	:CLSScanConfiguration(new SXRMBScanConfigurationDbObject())
{
//	dbObject_ = new SXRMBScanConfigurationDbObject;
//	configurationDbObject_ = dbObject_;
	sxrmbScanConfigurationDbObject_ = qobject_cast<SXRMBScanConfigurationDbObject *>(scanConfigurationDbObject_);

	timeOffset_ = 0;
	totalTime_ = 0;
}

SXRMBScanConfiguration::SXRMBScanConfiguration(const SXRMBScanConfiguration &original)
	:CLSScanConfiguration(new SXRMBScanConfigurationDbObject(*original.dbObject()))
{
//	dbObject_ = new SXRMBScanConfigurationDbObject(*original.dbObject());
//	configurationDbObject_ = dbObject_;

	sxrmbScanConfigurationDbObject_ = qobject_cast<SXRMBScanConfigurationDbObject *>(scanConfigurationDbObject_);

	timeOffset_ = original.timeOffset();
	totalTime_ = original.totalTime();
}

SXRMBScanConfiguration::~SXRMBScanConfiguration()
{
//	dbObject_->deleteLater();
}

//void SXRMBScanConfiguration::dbWriteScanConfigurationDbObject(AMDbObject *object)
//{
//	SXRMBScanConfigurationDbObject *dbo;

//	if ((dbo = qobject_cast<SXRMBScanConfigurationDbObject *>(object)))
//		dbObject_ = dbo;

//	else
//		object->deleteLater();
//}

bool SXRMBScanConfiguration::powerOnHVControl() const {
	return powerOnHVControl_;
}

double SXRMBScanConfiguration::totalTime(bool recompute)
{
	if (recompute)
		computeTotalTimeImplementation();

	return totalTime_;
}

//QString SXRMBScanConfiguration::regionsOfInterestHeaderString(const QList<AMRegionOfInterest *> &regions) const
//{
//	QString string = "";

//	if (!regions.isEmpty()){

//		string.append("\nRegions Of Interest\n");

//		foreach (AMRegionOfInterest *region, regions)
//			string.append(QString("%1\t%2 eV\t%3 eV\n").arg(region->name()).arg(region->lowerBound()).arg(region->upperBound()));
//	}

//	return string;
//}
