#include "BioXASSollerSlitScanController.h"

BioXASSollerSlitScanController::BioXASSollerSlitScanController(BioXASSollerSlitScanConfiguration *configuration, QObject *parent)
	: AMStepScanActionController(configuration, parent)
{
}

BioXASSollerSlitScanController::~BioXASSollerSlitScanController()
{

}

AMAction3 * BioXASSollerSlitScanController::createInitializationActions()
{
	return 0;
}

AMAction3 * BioXASSollerSlitScanController::createCleanupActions()
{
	return 0;
}

void BioXASSollerSlitScanController::buildScanControllerImplementation()
{
}
