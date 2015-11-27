#include "SGMSampleChamberVacuumMoveToVentedAction.h"
#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMSampleChamberVacuum.h"
#include "actions3/AMActionSupport.h"

SGMSampleChamberVacuumMoveToVentedAction::SGMSampleChamberVacuumMoveToVentedAction(SGMSampleChamberVacuumMoveActionInfo *info, QObject *parent) :
	SGMSampleChamberVacuumMoveAction(info, parent)
{
	QString description;

	// First, we would prompt user for action--select gas of choice.

	description = "Attention! Prepare manifold with gas to vent the chamber.";
	addSubAction(waitForInput(description));

	// Next, turn off chamber pumps, if on.

	addSubAction(turnOffChamberTurbos());

	addSubAction(turnOffChamberRoughingPump());

	// Next, setup the VAT valve and move valve to OPEN.

	addSubAction(openVATValve(SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MIN));

	// Finally, wait for the chamber pressure to reach Vented pressure.

	addSubAction(waitForVacuum(SGMSampleChamberVacuum::Vented));
}

SGMSampleChamberVacuumMoveToVentedAction::SGMSampleChamberVacuumMoveToVentedAction(const SGMSampleChamberVacuumMoveToVentedAction &original) :
	SGMSampleChamberVacuumMoveAction(original)
{

}

SGMSampleChamberVacuumMoveToVentedAction::~SGMSampleChamberVacuumMoveToVentedAction()
{

}

