#include "CLSJJSlitsOptimizationAction.h"
#include "beamline/CLS/CLSBeamline.h"

CLSJJSlitsOptimizationAction::CLSJJSlitsOptimizationAction(CLSJJSlitsOptimizationActionInfo *info, QObject *parent) :
	AMAction3(info, parent)
{
	CLSJJSlits *jjSlits = CLSBeamline::clsBeamline()->jjSlits();

	QList<AMGenericStepScanConfiguration*> configurationList = info->configurationList();
	AMDetectorInfo detectorInfo = info->detectorInfo();

	if (detectorInfo != AMDetectorInfo()) {

		// Start the optimization.

		setStarted();

		foreach (AMGenericStepScanConfiguration *configuration, configurationList) {

		}
	}


}

CLSJJSlitsOptimizationAction::CLSJJSlitsOptimizationAction(const CLSJJSlitsOptimizationAction &original) :
	AMAction3(original)
{

}

CLSJJSlitsOptimizationAction::~CLSJJSlitsOptimizationAction()
{

}
