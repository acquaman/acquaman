#include "BioXASGenericStepScanConfiguration.h"
#include "acquaman/BioXAS/BioXASGenericStepScanController.h"

BioXASGenericStepScanConfiguration::BioXASGenericStepScanConfiguration(QObject *parent) :
	AMGenericStepScanConfiguration(parent), BioXASScanConfiguration()
{

}

BioXASGenericStepScanConfiguration::BioXASGenericStepScanConfiguration(const BioXASGenericStepScanConfiguration &original) :
	AMGenericStepScanConfiguration(original), BioXASScanConfiguration()
{

}

BioXASGenericStepScanConfiguration::~BioXASGenericStepScanConfiguration()
{

}

AMScanConfiguration* BioXASGenericStepScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new BioXASGenericStepScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController* BioXASGenericStepScanConfiguration::createController()
{
	AMScanActionController *controller = new BioXASGenericStepScanController(this);
	controller->buildScanController();

	return controller;
}
