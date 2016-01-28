#include "AMXRFScanConfiguration.h"

#include "acquaman/AMXRFScanController.h"

AMXRFScanConfiguration::AMXRFScanConfiguration(QObject *parent)
	: AMScanConfiguration(parent)
{
	setAutoExportEnabled(false);
}

AMXRFScanConfiguration::AMXRFScanConfiguration(const AMXRFScanConfiguration &original)
	: AMScanConfiguration(original)
{
	setAutoExportEnabled(original.autoExportEnabled());
}

AMXRFScanConfiguration::~AMXRFScanConfiguration()
{

}

AMScanConfiguration *AMXRFScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new AMXRFScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController *AMXRFScanConfiguration::createController()
{return 0;
//	return new AMXRFScanController(this);
}

QString AMXRFScanConfiguration::description() const
{
	return "X-Ray Fluorescence scan";
}

QString AMXRFScanConfiguration::detailedDescription() const
{
	return "X-Ray Fluorescence scan for a single acquisition of an XRF detector";
}
