#include "VESPERS2DDacqScanController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"

VESPERS2DDacqScanController::VESPERS2DDacqScanController(VESPERS2DScanConfiguration *cfg, QObject *parent)
	: AM2DDacqScanController(cfg, parent)
{
	config_ = cfg;

	xAxisCount_ = int(fabs((config_->xEnd()-config_->xStart())/config_->xStep())) + 1;
	yAxisCount_ = int(fabs((config_->yEnd()-config_->yStart())/config_->yStep())) + 1;

	AMPVwStatusControl *control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->pseudoSampleStage()->horiz());
	xAxisPVName_ = control != 0 ? control->writePVName() : "";
	control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->pseudoSampleStage()->vert());
	yAxisPVName_ = control != 0 ? control->writePVName() : "";


}

bool VESPERS2DDacqScanController::initializeImplementation()
{
	/// \todo Flesh out with the appropriate actions like in VESPERSEXAFSDacqScanController.
	setInitialized();
	return true;
}

bool VESPERS2DDacqScanController::startImplementation()
{
	/// \todo Fill in the start implementation in a way that's similar too VESPERSEXAFSDacqScanController.
	return AM2DDacqScanController::startImplementation();
}
