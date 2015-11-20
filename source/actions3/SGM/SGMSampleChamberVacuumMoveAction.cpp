#include "SGMSampleChamberVacuumMoveAction.h"
#include "beamline/SGM/SGMBeamline.h"
#include "actions3/AMActionSupport.h"

SGMSampleChamberVacuumMoveAction::SGMSampleChamberVacuumMoveAction(SGMSampleChamberVacuumMoveActionInfo *info, QObject *parent) :
	AMListAction3(info, Sequential, parent)
{
	// Identify the beamline object for the sample chamber vacuum.

	vacuum_ = SGMBeamline::sgm()->sampleChamberVacuum();
	AMNumber vacuumSetpoint = vacuumMoveInfo()->setpoint();

	// Make connections, before beginning.

	connect( this, SIGNAL(failed()), this, SLOT(onActionFailed()) );

	// Begin.

	if (vacuum && vacuum->isConnected() && vacuumSetpoint != AMNumber(AMNumber::InvalidError) && vacuum->validSetpoint(double(vacuumSetpoint))) {
		double oldVacuum = vacuum->value();
		double newVacuum = double(vacuumSetpoint);

		if ()
	}

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

SGMSampleChamberVacuumMoveAction::SGMSampleChamberVacuumMoveAction(const SGMSampleChamberVacuumMoveAction &original) :
	AMAction3(original)
{

}

SGMSampleChamberVacuumMoveAction::~SGMSampleChamberVacuumMoveAction()
{

}

AMAction3* SGMSampleChamberVacuumMoveAction::createMoveToVentedAction()
{
	AMAction3 *result = 0;
	int currentValue = int(value());

	switch (currentValue) {
	case SGMSampleChamberVacuum::RoughVacuum:
		result = createMoveToVentedFromVacuumAction(); // same process for both vacuum states.
		break;
	case SGMSampleChamberVacuum::HighVacuum:
		result = createMoveToVentedFromVacuumAction();
		break;
	default:
		break;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::createMoveToVentedFromVacuumAction()
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

	action->addSubAction(AMActionSupport::buildControlWaitAction(this, SGMSampleChamberVacuum::Vented, SGMSAMPLECHAMBERVACUUM_MOVE_TIMEOUT));

	// Return action.

	return action;
}

AMAction3* SGMSampleChamberVacuumMoveAction::createMoveToRoughVacuumAction()
{
	AMAction3 *result = 0;
	int currentValue = int(value());

	switch (currentValue) {
	case SGMSampleChamberVacuum::Vented:
		result = createMoveToRoughVacuumFromVentedAction();
		break;
	case SGMSampleChamberVacuum::HighVacuum:
		result = createMoveToRoughVacuumFromHighVacuumAction();
		break;
	default:
		break;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::createMoveToRoughVacuumFromVentedAction()
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

	action->addSubAction(AMActionSupport::buildControlWaitAction(this, SGMSampleChamberVacuum::RoughVacuum, SGMSAMPLECHAMBERVACUUM_MOVE_TIMEOUT));

	// Return action.

	return action;
}

AMAction3* SGMSampleChamberVacuumMoveAction::createMoveToRoughVacuumFromHighVacuumAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Evacuating sample chamber to rough vacuum", "Evacuating sample chamber to rough vacuum"), AMListAction3::Sequential);

	// Turn off the chamber turbos.

	action->addSubAction(turnOffChamberTurbos());

	// Finally, wait for chamber pressure to drop.

	action->addSubAction(AMActionSupport::buildControlWaitAction(this, SGMSampleChamberVacuum::RoughVacuum, SGMSAMPLECHAMBERVACUUM_MOVE_TIMEOUT));

	// Return action.

	return action;
}

AMAction3* SGMSampleChamberVacuumMoveAction::createMoveToHighVacuumAction()
{
	AMAction3 *result = 0;
	int currentValue = int(value());

	switch (currentValue) {
	case SGMSampleChamberVacuum::Vented:
		result = createMoveToHighVacuumFromVentedAction();
		break;
	case SGMSampleChamberVacuum::HighVacuum:
		result = createMoveToHighVacuumFromRoughVacuumAction();
		break;
	default:
		break;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::createMoveToHighVacuumFromVentedAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Evacuating sample chamber to high vacuum", "Evacuating sample chamber to high vacuum"), AMListAction3::Sequential);

	// First, get the sample chamber to rough vacuum.

	action->addSubAction(createMoveToRoughVacuumFromVentedAction());

	// Finally, move to high vacuum.

	action->addSubAction(createMoveToHighVacuumFromRoughVacuumAction());

	// Return action.

	return action;
}

AMAction3* SGMSampleChamberVacuumMoveAction::createMoveToHighVacuumFromRoughVacuumAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Evacuating sample chamber to high vacuum", "Evacuating sample chamber to high vacuum"), AMListAction3::Sequential);

	// First, turn on chamber turbos.

	action->addSubAction(turnOnChamberTurbos());

	// Finally, wait for chamber pressure to drop.

	action->addSubAction(AMActionSupport::buildControlWaitAction(this, SGMSampleChamberVacuum::HighVacuum, SGMSAMPLECHAMBERVACUUM_MOVE_TIMEOUT));

	// Return action.

	return action;
}

AMAction3* SGMSampleChamberVacuumMoveAction::closeVATValve(double speed)
{
	AMAction3 *result = 0;

	if (vacuum_ && vacuum_->vatValve()) {
		AMListAction3 *closeValve = new AMListAction3(new AMListActionInfo3("Close VAT valve", "Close VAT valve"), AMListAction3::Sequential);
		closeValve->addSubAction(AMActionSupport::buildControlMoveAction(vacuum_->vatValve()->speed(), speed));
		closeValve->addSubAction(AMActionSupport::buildControlMoveAction(vacuum_->vatValve(), SGMVATValve::Closed));

		result = closeValve;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::checkVATValveClosed()
{
	AMAction3 *result = 0;

	if (vacuum_ && vacuum_->vatValve())
		result = AMActionSupport::buildControlWaitAction(vacuum_->vatValve(), SGMVATValve::Closed, SGMSAMPLECHAMBERVACUUMCONTROL_VATVALVE_TIMEOUT);

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::openVATValve(double speed)
{
	AMAction3 *result = 0;

	if (vacuum_ && vacuum_->vatValve()) {
		AMListAction3 *openValve = new AMListAction3(new AMListActionInfo3("Open VAT valve", "Open VAT valve"), AMListAction3::Sequential);
		openValve->addSubAction(AMActionSupport::buildControlMoveAction(vacuum_->vatValve()->speed(), speed));
		openValve->addSubAction(AMActionSupport::buildControlMoveAction(vacuum_->vatValve(), SGMVATValve::Open));

		result = openValve;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::checkVATValveOpen()
{
	AMAction3 *result = 0;

	if (vacuum_ && vacuum_->vatValve())
		result = AMActionSupport::buildControlWaitAction(vacuum_->vatValve(), SGMVATValve::Open, SGMSAMPLECHAMBERVACUUMCONTROL_VATVALVE_TIMEOUT);

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::turnOffChamberTurbos()
{
	AMAction3 *result = 0;

	if (vacuum_ && vacuum_->turbos()) {
		AMListAction3 *changeTurbos = new AMListAction3(new AMListActionInfo3("Turning off chamber turbos", "Turning off chamber turbos"), AMListAction3::Parallel);

		for (int turboIndex = 0, turboCount = turbos->count(); turboIndex < turboCount; turboIndex++) {
			AMAction3 *changeTurbo = turnOffTurbo(qobject_cast<SGMTurboPump*>(turbos->at(turboIndex)));

			if (changeTurbo)
				changeTurbos->addSubAction(changeTurbo);
		}

		result = changeTurbos;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::checkChamberTurbosOff()
{
	AMAction3 *result = 0;

	if (vacuum_ && vacuum_->turbos()) {
		AMListAction3 *checkTurbos = new AMListAction3(new AMListActionInfo3("Turning on chamber turbos", "Turning on chamber turbos"), AMListAction3::Parallel);

		AMControlSet *turbos = vacuum_->turbos();

		for (int turboIndex = 0, turboCount = turbos->count(); turboIndex < turboCount; turboIndex++) {
			AMAction3 *checkTurbo = checkTurboOff(qobject_cast<SGMTurboPump*>(turbos->at(turboIndex)));

			if (checkTurbo)
				checkTurbos->addSubAction(checkTurbo);
		}

		result = checkTurbos;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::turnOnChamberTurbos()
{
	AMAction3 *result = 0;

	if (vacuum_ && vacuum_->turbos()) {
		AMListAction3 *changeTurbos = new AMListAction3(new AMListActionInfo3("Turning on chamber turbos", "Turning on chamber turbos"), AMListAction3::Parallel);

		AMControlSet *turbos = vacuum_->turbos();

		for (int turboIndex = 0, turboCount = turbos->count(); turboIndex < turboCount; turboIndex++) {
			AMAction3 *changeTurbo = turnOnTurbo(qobject_cast<SGMTurboPump*>(turbos->at(turboIndex)));

			if (changeTurbo)
				changeTurbos->addSubAction(changeTurbo);
		}

		result = changeTurbos;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::checkChamberTurbosOn()
{
	AMAction3 *result = 0;

	if (vacuum_ && vacuum_->turbos()) {
		AMListAction3 *checkTurbos = new AMListAction3(new AMListActionInfo3("Turning on chamber turbos", "Turning on chamber turbos"), AMListAction3::Parallel);

		AMControlSet *turbos = vacuum_->turbos();

		for (int turboIndex = 0, turboCount = turbos->count(); turboIndex < turboCount; turboIndex++) {
			AMAction3 *checkTurbo = checkTurboOn(qobject_cast<SGMTurboPump*>(turbos->at(turboIndex)));

			if (checkTurbo)
				checkTurbos->addSubAction(checkTurbo);
		}

		result = checkTurbos;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::turnOffTurbo(SGMTurboPump *turbo)
{
	AMAction3 *result = 0;

	if (turbo)
		result = AMActionSupport::buildControlMoveAction(turbo->running(), SGMTurboPump::NotRunning);

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::checkTurboOff(SGMTurboPump *turbo)
{
	AMAction3 *result = 0;

	if (turbo)
		result = AMActionSupport::buildControlWaitAction(turbo->running(), SGMTurboPump::NotRunning, SGMSAMPLECHAMBERVACUUMCONTROL_TURBO_TIMEOUT);

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::turnOnTurbo(SGMTurboPump *turbo)
{
	AMAction3 *result = 0;

	if (turbo)
		result = AMActionSupport::buildControlMoveAction(turbo->running(), SGMTurboPump::Running);

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::checkTurboOn(SGMTurboPump *turbo)
{
	AMAction3 *result = 0;

	if (turbo)
		result = AMActionSupport::buildControlWaitAction(turbo->running(), SGMTurboPump::Running, SGMSAMPLECHAMBERVACUUMCONTROL_TURBO_TIMEOUT);

	return result;
}

