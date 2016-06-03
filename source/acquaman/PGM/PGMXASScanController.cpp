#include "PGMXASScanController.h"

PGMXASScanController::PGMXASScanController(PGMXASScanConfiguration *configuration, QObject *parent)
	: AMGenericStepScanController(configuration, parent)
{
	configuration_ = configuration;
}

