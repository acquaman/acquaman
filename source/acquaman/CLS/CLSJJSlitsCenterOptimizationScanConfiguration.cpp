#include "CLSJJSlitsCenterOptimizationScanConfiguration.h"
#include "acquaman/CLS/CLSJJSlitsCenterOptimizationScanController.h"
#include "ui/acquaman/AMGenericStepScanConfigurationView.h"

CLSJJSlitsCenterOptimizationScanConfiguration::CLSJJSlitsCenterOptimizationScanConfiguration(QObject *parent) :
    AMGenericStepScanConfiguration(parent)
{

}

CLSJJSlitsCenterOptimizationScanConfiguration::CLSJJSlitsCenterOptimizationScanConfiguration(const CLSJJSlitsCenterOptimizationScanConfiguration &original) :
	AMGenericStepScanConfiguration(original)
{

}

CLSJJSlitsCenterOptimizationScanConfiguration::~CLSJJSlitsCenterOptimizationScanConfiguration()
{

}

AMScanConfiguration* CLSJJSlitsCenterOptimizationScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new CLSJJSlitsCenterOptimizationScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController* CLSJJSlitsCenterOptimizationScanConfiguration::createController()
{
	AMScanActionController *controller = new CLSJJSlitsCenterOptimizationScanController(this);
	controller->buildScanController();
	return controller;
}

AMScanConfigurationView* CLSJJSlitsCenterOptimizationScanConfiguration::createView() const
{
	return AMGenericStepScanConfigurationView(this);
}

void CLSJJSlitsCenterOptimizationScanConfiguration::setJJSlits(CLSJJSlits *jjSlits)
{

}
