#include "CLSJJSlitOptimizationAction.h"
#include "acquaman/AMScanActionController.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMScanAction.h"
#include "beamline/CLS/CLSBeamline.h"
#include "util/AMErrorMonitor.h"

CLSJJSlitOptimizationAction::CLSJJSlitOptimizationAction(CLSJJSlitOptimizationActionInfo *info, QObject *parent) :
	AMListAction3(info, AMListAction3::Sequential, parent)
{
	// Identify the JJ slits object to be optimized.

	CLSJJSlits *jjSlits = CLSBeamline::clsBeamline()->jjSlits();

	if (jjSlits && jjSlits->isConnected()) {

		AMControl *control = CLSBeamline::clsBeamline()->exposedControlByInfo(info->control());
		AMGenericStepScanConfiguration *configuration = info->scanConfiguration();

		if (control && configuration) {

			// Create scan for the control being optimized.

			AMScanAction *jjSlitsOptimizationScan = new AMScanAction(new AMScanActionInfo(configuration));
			addSubAction(jjSlitsOptimizationScan);

			// Analyze the step scan results, identify the optimal value.

			AMScan *results = jjSlitsOptimizationScan->controller()->scan();

			// ...

			double optimalValue = 0;

			// Move control to optimal positions.

			addSubAction(AMActionSupport::buildControlMoveAction(control, optimalValue));
		}
	}
}

CLSJJSlitOptimizationAction::CLSJJSlitOptimizationAction(const CLSJJSlitOptimizationAction &original) :
	AMListAction3(original)
{

}

CLSJJSlitOptimizationAction::~CLSJJSlitOptimizationAction()
{

}
