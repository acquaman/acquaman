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

AMScanController* BioXASGenericStepScanConfiguration::createController()
{
	AMScanActionController *controller = new BioXASGenericStepScanController(this);
	controller->buildScanController();

	return controller;
}
