#include "SGMSampleChamberVacuumMoveToHighVacuumFromVentedAction.h"
#include "actions3/SGM/SGMSampleChamberVacuumMoveToRoughVacuumFromVentedAction.h"
#include "actions3/SGM/SGMSampleChamberVacuumMoveToHighVacuumFromRoughVacuumAction.h"
#include "beamline/SGM/SGMSampleChamberVacuum.h"

SGMSampleChamberVacuumMoveToHighVacuumFromVentedAction::SGMSampleChamberVacuumMoveToHighVacuumFromVentedAction(SGMSampleChamberVacuumMoveActionInfo *info, QObject *parent) :
	SGMSampleChamberVacuumMoveAction(info, parent)
{
	// First, check that the chamber door is closed.

	addSubAction(checkDoorClosed());

	// Next, close the VAT valve.

	addSubAction(closeVATValve(SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MAX));

	// Next, prompt user for action--close any open gas lines.

	QString instruction = "Close any open gas lines.";
	addSubAction(waitForInput(instruction));

	// Next, open the VAT valve.

	addSubAction(openVATValve(SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MIN));

	// Next, start up chamber roughing pump.

	addSubAction(turnOnChamberRoughingPump());

	// Next, close VAT valve.

	addSubAction(closeVATValve(SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MAX));

	// Next, wait for chamber pressure to drop to rough vacuum.

	addSubAction(waitForVacuum(SGMSampleChamberVacuum::RoughVacuum));

	// Next, turn on chamber turbos.

	addSubAction(turnOnChamberTurbos());

	// Finally, wait for chamber pressure to drop.

	addSubAction(waitForVacuum(SGMSampleChamberVacuum::HighVacuum));
}

SGMSampleChamberVacuumMoveToHighVacuumFromVentedAction::SGMSampleChamberVacuumMoveToHighVacuumFromVentedAction(const SGMSampleChamberVacuumMoveToHighVacuumFromVentedAction &original) :
	SGMSampleChamberVacuumMoveAction(original)
{

}

SGMSampleChamberVacuumMoveToHighVacuumFromVentedAction::~SGMSampleChamberVacuumMoveToHighVacuumFromVentedAction()
{

}

