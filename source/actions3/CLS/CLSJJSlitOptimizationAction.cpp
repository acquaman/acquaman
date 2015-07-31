#include "CLSJJSlitOptimizationAction.h"
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

		if (info->bladesDirection() == CLSJJSlits::Blades::Vertical && info->bladesValue() == CLSJJSlits::Blades::Center) {

			// Move all controls to the 'home' position.

			addSubAction(jjSlits->createMoveToOriginAction());

			// Move the control not being optimized to the max gap (out of the way).

			addSubAction(AMActionSupport::buildControlMoveAction(jjSlits->horizontalGapControl(), jjSlits->horizontalGapControl()->maximumValue()));

			// Create generic step scan for the control being optimized.

			AMScanAction *jjSlitsOptimizationScan = new AMScanAction(new AMScanActionInfo(info->scanConfiguration()));
			addSubAction(jjSlitsOptimizationScan);

			// Analyze the step scan results, identify the optimal value.

			AMScan *results = jjSlitsOptimizationScan->controller()->scan();
			double optimalValue = 0;

			// Move all controls to home position.

			addSubAction(jjSlits->createMoveToHomePositionAction());

			// Update blade positions.

			addSubAction(jjSlits->createMoveAction(info->initialVerticalGap(), optimalValue, info->initialHorizontalGap(), info->initialHorizontalCenter()));
	}
}
