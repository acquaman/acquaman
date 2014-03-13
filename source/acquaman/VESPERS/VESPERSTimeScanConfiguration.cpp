#include "VESPERSTimeScanConfiguration.h"

#include "acquaman/VESPERS/VESPERSTimeScanActionController.h"
#include "ui/VESPERS/VESPERSTimeScanConfigurationView.h"

VESPERSTimeScanConfiguration::VESPERSTimeScanConfiguration(QObject *parent)
	: AMTimedRegionScanConfiguration(parent)
{
	setTime(1);
	setTotalTime(10);
}

VESPERSTimeScanConfiguration::VESPERSTimeScanConfiguration(const VESPERSTimeScanConfiguration &original)
	: AMTimedRegionScanConfiguration(original)
{

}

AMScanConfiguration *VESPERSTimeScanConfiguration::createCopy() const
{
	return new VESPERSTimeScanConfiguration(*this);
}

AMScanController *VESPERSTimeScanConfiguration::createController()
{
	AMScanActionController *controller = new VESPERSTimeScanActionController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView *VESPERSTimeScanConfiguration::createView()
{
	return new VESPERSTimeScanConfigurationView(this);
}
