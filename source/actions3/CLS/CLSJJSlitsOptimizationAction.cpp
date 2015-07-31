#include "CLSJJSlitsOptimizationAction.h"

#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/CLS/CLSJJSlitOptimizationAction.h"
#include "beamline/CLS/CLSBeamline.h"
#include "util/AMErrorMonitor.h"

CLSJJSlitsOptimizationAction::CLSJJSlitsOptimizationAction(CLSJJSlitsOptimizationActionInfo *info, QObject *parent) :
	AMListAction3(info, AMListAction3::Sequential, parent)
{
	CLSJJSlits *jjSlits = CLSBeamline::clsBeamline()->jjSlits();

	// Assume now we are optimizing the center position of both V and H blades.

	if (jjSlits && jjSlits->isConnected()) {

		// Save the desired gap to reapply after optimization (configuration?)

		double oldVerticalGap = 0;
		double oldHorizontalGap = 0;

		// Optimize the center position of the V blades.

		addSubAction(new CLSJJSlitOptimizationAction(new CLSJJSlitOptimizationActionInfo()));

		// Save the optimal center position of the V blades.

		double newVerticalCenter = 0;

		// Optimize the center position of the H blades.

		addSubAction(new CLSJJSlitOptimizationAction(new CLSJJSlitOptimizationActionInfo()));

		// Save the optimal center position of the H blades.

		double newHorizontalCenter = 0;

		//


	} else {

		if (!jjSlits)
			AMErrorMon::alert(this, CLSJJSLITSOPTIMIZATIONACTION_JJSLITS_NOT_VALID, "Failed to complete JJ slits optimization--JJ slits not valid.");

		else if (!jjSlits->isConnected())
			AMErrorMon::alert(this, CLSJJSLITSOPTIMIZATIONACTION_JJSLITS_NOT_CONNECTED, "Failed to complete JJ slits optimization--JJ slits not connected.");

		setFailed();
	}
}

CLSJJSlitsOptimizationAction::CLSJJSlitsOptimizationAction(const CLSJJSlitsOptimizationAction &original) :
	AMListAction3(original)
{

}

CLSJJSlitsOptimizationAction::~CLSJJSlitsOptimizationAction()
{

}
