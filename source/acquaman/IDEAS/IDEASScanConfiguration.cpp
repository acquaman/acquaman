#include "IDEASScanConfiguration.h"

IDEASScanConfiguration::IDEASScanConfiguration()
	:CLSScanConfiguration(new IDEASScanConfigurationDbObject)
{
//	dbObject_ = new IDEASScanConfigurationDbObject;
	vespersScanConfigurationDbObject_ = qobject_cast<IDEASScanConfigurationDbObject *>(scanConfigurationDbObject_);
	totalTime_ = 0;
	timeOffset_ = 0.4;
}

IDEASScanConfiguration::IDEASScanConfiguration(const IDEASScanConfiguration &original)
	:CLSScanConfiguration(new IDEASScanConfigurationDbObject(*original.dbObject()))
{
//	dbObject_ = new IDEASScanConfigurationDbObject(*original.dbObject());
	vespersScanConfigurationDbObject_ = qobject_cast<IDEASScanConfigurationDbObject *>(scanConfigurationDbObject_);
	timeOffset_ = original.timeOffset();
	totalTime_ = original.totalTime();
}

IDEASScanConfiguration::~IDEASScanConfiguration()
{
//	dbObject_->deleteLater();
}

//void IDEASScanConfiguration::dbWriteScanConfigurationDbObject(AMDbObject *object)
//{
//	IDEASScanConfigurationDbObject *dbo;

//	if ((dbo = qobject_cast<IDEASScanConfigurationDbObject *>(object)))
//		dbObject_ = dbo;
//}

QString IDEASScanConfiguration::fluorescenceHeaderString(IDEAS::FluorescenceDetectors detector) const
{
	QString string = "";

	if (detector.testFlag(IDEAS::NoXRF))
		string.append("Fluorescence Detector:\tNone\n");

	else if (detector.testFlag(IDEAS::Ketek))
		string.append("Fluorescence Detector:\tKetek Detector\n");

	else if (detector.testFlag(IDEAS::Ge13Element))
		string.append("Fluorescence Detector:\tGermanium 13 Element Detector\n");

	return string;
}

//QString IDEASScanConfiguration::regionsOfInterestHeaderString(const QList<AMRegionOfInterest *> &regions) const
//{
//	QString string = "";

//	if (!regions.isEmpty()){

//		string.append("\nRegions Of Interest\n");

//		foreach (AMRegionOfInterest *region, regions)
//			string.append(QString("%1\t%2 eV\t%3 eV\n").arg(region->name()).arg(region->lowerBound()).arg(region->upperBound()));
//	}

//	return string;
//}
