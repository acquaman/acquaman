#include "VESPERSXASScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSXASDacqScanController.h"
#include "ui/VESPERS/VESPERSXASScanConfigurationView.h"

VESPERSXASScanConfiguration::VESPERSXASScanConfiguration(QObject *parent)
	: AMXASScanConfiguration(parent)
{
}

VESPERSXASScanConfiguration::VESPERSXASScanConfiguration(const VESPERSXASScanConfiguration &original)
	: AMXASScanConfiguration(parent)
{

}

AMScanConfiguration *VESPERSXASScanConfiguration::createCopy() const
{
	return new VESPERSXASScanConfiguration(*this);
}

AMScanController *VESPERSXASScanConfiguration::createController()
{
	return new VESPERSXASDacqScanController(this);
}

AMScanConfigurationView *VESPERSXASScanConfiguration::createView()
{
	return new VESPERSXASScanConfigurationView(this);
}

QString VESPERSXASScanConfiguration::detailedDescription()
{
	return QString("Puked");
}
