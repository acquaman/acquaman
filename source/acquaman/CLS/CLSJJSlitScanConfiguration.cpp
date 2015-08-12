#include "CLSJJSlitScanConfiguration.h"
#include "acquaman/CLS/CLSJJSlitScanController.h"
#include "ui/acquaman/CLS/CLSJJSlitScanConfigurationView.h"

CLSJJSlitScanConfiguration::CLSJJSlitScanConfiguration(QObject *parent) :
    AMGenericStepScanConfiguration(parent)
{
	setName("JJ Slit Scan");
	setUserScanName("JJ Slit Scan");
}

CLSJJSlitScanConfiguration::CLSJJSlitScanConfiguration(const CLSJJSlitScanConfiguration &original) :
	AMGenericStepScanConfiguration(original)
{

}

CLSJJSlitScanConfiguration::~CLSJJSlitScanConfiguration()
{

}

AMScanConfiguration* CLSJJSlitScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new CLSJJSlitScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController* CLSJJSlitScanConfiguration::createController()
{
	AMScanActionController *controller = new CLSJJSlitScanController(this);
	controller->buildScanController();
	return controller;
}

AMScanConfigurationView* CLSJJSlitScanConfiguration::createView() const
{
	return new CLSJJSlitScanConfigurationView(this);
}
