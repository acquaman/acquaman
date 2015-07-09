#include "BioXASMainXASScanConfiguration.h"

#include "acquaman/BioXAS/BioXASMainXASScanActionController.h"
#include "ui/BioXAS/BioXASXASScanConfigurationView.h"


BioXASMainXASScanConfiguration::BioXASMainXASScanConfiguration(QObject *parent)
	: BioXASXASScanConfiguration(parent)
{

}

BioXASMainXASScanConfiguration::BioXASMainXASScanConfiguration(const BioXASMainXASScanConfiguration &original)
	: BioXASXASScanConfiguration(original)
{

}

BioXASMainXASScanConfiguration::~BioXASMainXASScanConfiguration()
{

}

AMScanConfiguration* BioXASMainXASScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new BioXASMainXASScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController* BioXASMainXASScanConfiguration::createController()
{
	AMScanActionController *controller = new BioXASMainXASScanActionController(this);
	controller->buildScanController();

	return controller;
}
