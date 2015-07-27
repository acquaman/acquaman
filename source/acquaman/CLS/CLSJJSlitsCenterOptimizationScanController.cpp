#include "CLSJJSlitsCenterOptimizationScanController.h"

CLSJJSlitsCenterOptimizationScanController::CLSJJSlitsCenterOptimizationScanController(CLSJJSlitsCenterOptimizationScanConfiguration *configuration, QObject *parent) :
	AMGenericStepScanController(configuration, parent)
{
	jjSlitsScanConfiguration_ = configuration;
}

CLSJJSlitsCenterOptimizationScanController::~CLSJJSlitsCenterOptimizationScanController()
{

}

AMAction3* CLSJJSlitsCenterOptimizationScanController::createInitializationActions()
{
	return 0;
}

AMAction3* CLSJJSlitsCenterOptimizationScanController::createCleanupActions()
{
	return 0;
}
