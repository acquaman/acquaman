#include "VESPERSTimedLineScanActionController.h"

#include "acquaman/VESPERS/VESPERSTimedLineScanActionControllerAssembler.h"

VESPERSTimedLineScanActionController::VESPERSTimedLineScanActionController(VESPERSTimedLineScanConfiguration *configuration, QObject *parent)
	: VESPERSSpatialLineScanActionController(configuration, parent)
{
	timedConfiguration_ = configuration;
}

VESPERSTimedLineScanActionController::~VESPERSTimedLineScanActionController()
{

}

void VESPERSTimedLineScanActionController::createScanAssembler()
{
	scanAssembler_ = new VESPERSTimedLineScanActionControllerAssembler(timedConfiguration_->iterations(), timedConfiguration_->timePerAcquisition(), this);
}
