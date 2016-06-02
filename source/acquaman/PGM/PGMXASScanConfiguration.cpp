#include "PGMXASScanConfiguration.h"

#include "ui/PGM/PGMXASScanConfigurationView.h"
#include "acquaman/PGM/PGMXASScanController.h"

PGMXASScanConfiguration::PGMXASScanConfiguration(QObject *parent)
	: AMGenericStepScanConfiguration(parent)
{

}


PGMXASScanConfiguration::PGMXASScanConfiguration(const PGMXASScanConfiguration &original)
	: AMGenericStepScanConfiguration(original)
{

}

AMScanConfiguration *PGMXASScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new PGMXASScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController *PGMXASScanConfiguration::createController()
{
	AMScanActionController *controller = new PGMXASScanController(this);
	controller->buildScanController();
	return controller;
}

AMScanConfigurationView *PGMXASScanConfiguration::createView()
{
	return new PGMXASScanConfigurationView(this);
}
