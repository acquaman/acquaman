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

		// Pre-optimization settings.

		double initialVerticalGap = jjSlits->verticalGapControl()->value();
		double initialVerticalCenter = jjSlits->verticalCenterControl()->value();
		double initialHorizontalGap = jjSlits->horizontalGapControl()->value();
		double initialHorizontalCenter = jjSlits->horizontalCenterControl()->value();

		double finalVerticalGap = 0;
		double finalVerticalCenter = 0;
		double finalHorizontalGap = 0;
		double finalHorizontalCenter = 0;

		if (info->direction() == CLSJJSlits::Direction::Vertical && info->property() == CLSJJSlits::Property::Center) {

			// Move all controls to the origin.

			addSubAction(jjSlits->createMoveToOriginAction());

			// Move the control not being optimized to the max gap (out of the way).

			addSubAction(AMActionSupport::buildControlMoveAction(jjSlits->horizontalGapControl(), jjSlits->horizontalGapControl()->maximumValue()));

			// Create scan for the control being optimized.

			AMScanAction *jjSlitsOptimizationScan = new AMScanAction(new AMScanActionInfo(info->scanConfiguration()));
			addSubAction(jjSlitsOptimizationScan);

			// Analyze the step scan results, identify the optimal value.

			AMScan *results = jjSlitsOptimizationScan->controller()->scan();

			// ...

			double optimalValue = 0;

			finalVerticalGap = initialVerticalGap;
			finalVerticalCenter = optimalValue;
			finalHorizontalGap = initialHorizontalGap;
			finalHorizontalCenter = initialHorizontalCenter;
		}

		// Move all controls to home position.

		addSubAction(jjSlits->createMoveToOriginAction());

		// Move controls to optimal positions.

		addSubAction(jjSlits->createMoveAction(finalVerticalGap, finalHorizontalGap, finalVerticalCenter, finalVerticalCenter));
	}
}

CLSJJSlitOptimizationAction::CLSJJSlitOptimizationAction(const CLSJJSlitOptimizationAction &original) :
	AMListAction3(original)
{

}

CLSJJSlitOptimizationAction::~CLSJJSlitOptimizationAction()
{

}
