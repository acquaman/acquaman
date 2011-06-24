#include "VESPERSXASDacqScanController.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "beamline/AMBeamlineActionsList.h"

VESPERSXASDacqScanController::VESPERSXASDacqScanController(VESPERSXASScanConfiguration *cfg, QObject *parent)
{
}

bool VESPERSXASDacqScanController::initializeImplementation()
{
	return false;
}

bool VESPERSXASDacqScanController::startImplementation()
{
	return false;
}

AMnDIndex VESPERSXASDacqScanController::toScanIndex(QMap<int, double> aeData)
{
	return AMnDIndex();
}

void VESPERSXASDacqScanController::onInitializationActionsSucceeded()
{

}

void VESPERSXASDacqScanController::onInitializationActionsFailed(int explanation)
{

}

void VESPERSXASDacqScanController::onInitializationActionsProgress(double elapsed, double total)
{

}
