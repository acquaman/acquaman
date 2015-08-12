#include "CLSJJSlitScanController.h"
#include "beamline/CLS/CLSBeamline.h"

CLSJJSlitScanController::CLSJJSlitScanController(CLSJJSlitScanConfiguration *configuration, QObject *parent) :
	AMGenericStepScanController(configuration, parent)
{
	// Initialize class variables.

	jjSlitScanConfiguration_ = configuration;

	jjSlits_ = CLSBeamline::clsBeamline()->jjSlits();
}

CLSJJSlitScanController::~CLSJJSlitScanController()
{

}

AMAction3* CLSJJSlitScanController::createInitializationActions()
{
	return 0;
}

AMAction3* CLSJJSlitScanController::createCleanupActions()
{
	return 0;
}

void CLSJJSlitScanController::buildScanControllerImplementation()
{

}
