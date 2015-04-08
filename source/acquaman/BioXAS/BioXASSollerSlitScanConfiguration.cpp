#include "BioXASSollerSlitScanConfiguration.h"

BioXASSollerSlitScanConfiguration::BioXASSollerSlitScanConfiguration(QObject *parent)
	: AMStepScanConfiguration(parent)
{
}

BioXASSollerSlitScanConfiguration::BioXASSollerSlitScanConfiguration(const BioXASSollerSlitScanConfiguration &original)
	: AMStepScanConfiguration(original)
{

}

BioXASSollerSlitScanConfiguration::~BioXASSollerSlitScanConfiguration()
{

}

AMScanConfiguration *BioXASSollerSlitScanConfiguration::createCopy() const
{
	return 0;
}

AMScanController *BioXASSollerSlitScanConfiguration::createController()
{
	return 0;
}

AMScanConfigurationView *BioXASSollerSlitScanConfiguration::createView()
{
	return 0;
}

QString BioXASSollerSlitScanConfiguration::detailedDescription() const
{
	return "";
}
