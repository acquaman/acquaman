#include "SGMSampleChamberVacuum.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

SGMSampleChamberVacuum::SGMSampleChamberVacuum(QObject *parent) :
	SGMSampleChamberVacuumControl("sampleChamberVacuum", parent)
{
	// Setup available vacuum states.

	addVacuumOption(Vented, "Vented", SGMSAMPLECHAMBERVACUUM_VENTED_MIN, SGMSAMPLECHAMBERVACUUM_VENTED_MAX);
	addVacuumOption(RoughVacuum, "Rough vacuum", SGMSAMPLECHAMBERVACUUM_ROUGHVACUUM_MIN, SGMSAMPLECHAMBERVACUUM_ROUGHVACUUM_MAX);
	addVacuumOption(HighVacuum, "High vacuum", SGMSAMPLECHAMBERVACUUM_HIGHVACUUM_MIN, SGMSAMPLECHAMBERVACUUM_HIGHVACUUM_MAX);
}

SGMSampleChamberVacuum::~SGMSampleChamberVacuum()
{

}

AMAction3* SGMSampleChamberVacuum::createMoveAction(double indexSetpoint)
{
	AMAction3 *result = 0;

	switch (int(indexSetpoint)) {
	case Vented:
		result = createMoveToVentedAction();
		break;
	case RoughVacuum:
		result = createMoveToRoughVacuumAction();
		break;
	case HighVacuum:
		result = createMoveToHighVacuumAction();
		break;
	default:
		break;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuum::createMoveToVentedAction()
{
	AMAction3 *result = 0;
	int currentValue = int(value());

	switch (currentValue) {
	case RoughVacuum:
		result = createMoveToVentedFromVacuumAction(); // same process for both vacuum states.
		break;
	case HighVacuum:
		result = createMoveToVentedFromVacuumAction();
		break;
	default:
		break;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuum::createMoveToVentedFromVacuumAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Venting sample chamber", "Venting sample chamber"), AMListAction3::Sequential);

	// First, we would prompt user for action--select gas of choice.
	// Ignoring for now.

	// Next, turn off chamber pumps, if on.

	action->addSubAction(turnOffChamberTurbos());
	action->addSubAction(turnOffChamberRoughingPump());

	// Next, setup the VAT valve and move valve to OPEN.

	action->addSubAction(openVATValve(SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MIN));

	// Finally, wait for the chamber pressure to reach Vented pressure.

	action->addSubAction(AMActionSupport::buildControlWaitAction(this, Vented, SGMSAMPLECHAMBERVACUUM_MOVE_TIMEOUT));

	// Return action.

	return action;
}

AMAction3* SGMSampleChamberVacuum::createMoveToRoughVacuumAction()
{
	AMAction3 *result = 0;
	int currentValue = int(value());

	switch (currentValue) {
	case Vented:
		result = createMoveToRoughVacuumFromVentedAction();
		break;
	case HighVacuum:
		result = createMoveToRoughVacuumFromHighVacuumAction();
		break;
	default:
		break;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuum::createMoveToRoughVacuumFromVentedAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Evacuating sample chamber to rough vacuum", "Evacuating sample chamber to rough vacuum"), AMListAction3::Sequential);

	// First, we would close the VAT valve.

	action->addSubAction(closeVATValve(SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MAX));

	// Next, prompt user for action--close any open gas lines.
	// Ignoring for now.

	// Next, open the VAT valve.

	action->addSubAction(openVATValve(SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MIN));

	// Next, start up the chamber roughing pump.

	action->addSubAction(turnOnChamberRoughingPump());

	// Next, close VAT valve.

	action->addSubAction(closeVATValve(SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MAX));

	// Finally, wait for chamber pressure to drop.

	action->addSubAction(AMActionSupport::buildControlWaitAction(this, RoughVacuum, SGMSAMPLECHAMBERVACUUM_MOVE_TIMEOUT));

	// Return action.

	return action;
}

AMAction3* SGMSampleChamberVacuum::createMoveToRoughVacuumFromHighVacuumAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Evacuating sample chamber to rough vacuum", "Evacuating sample chamber to rough vacuum"), AMListAction3::Sequential);

	// Turn off the chamber turbos.

	action->addSubAction(turnOffChamberTurbos());

	// Finally, wait for chamber pressure to drop.

	action->addSubAction(AMActionSupport::buildControlWaitAction(this, RoughVacuum, SGMSAMPLECHAMBERVACUUM_MOVE_TIMEOUT));

	// Return action.

	return action;
}

AMAction3* SGMSampleChamberVacuum::createMoveToHighVacuumAction()
{
	AMAction3 *result = 0;
	int currentValue = int(value());

	switch (currentValue) {
	case Vented:
		result = createMoveToHighVacuumFromVentedAction();
		break;
	case HighVacuum:
		result = createMoveToHighVacuumFromRoughVacuumAction();
		break;
	default:
		break;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuum::createMoveToHighVacuumFromVentedAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Evacuating sample chamber to high vacuum", "Evacuating sample chamber to high vacuum"), AMListAction3::Sequential);

	// First, get the sample chamber to rough vacuum.

	action->addSubAction(createMoveToRoughVacuumFromVentedAction());

	// Finally, move to high vacuum.

	action->addSubAction(createMoveToHighVacuumFromRoughVacuumAction());

	// Return action.

	return action;
}

AMAction3* SGMSampleChamberVacuum::createMoveToHighVacuumFromRoughVacuumAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Evacuating sample chamber to high vacuum", "Evacuating sample chamber to high vacuum"), AMListAction3::Sequential);

	// First, turn on chamber turbos.

	action->addSubAction(turnOnChamberTurbos());

	// Finally, wait for chamber pressure to drop.

	action->addSubAction(AMActionSupport::buildControlWaitAction(this, HighVacuum, SGMSAMPLECHAMBERVACUUM_MOVE_TIMEOUT));

	// Return action.

	return action;
}
