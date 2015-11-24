#include "SGMSampleChamberVacuumMoveAction.h"
#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMTurboPump.h"
#include "beamline/SGM/SGMSampleChamberVacuum.h"
#include "actions3/AMActionSupport.h"
#include "util/AMErrorMonitor.h"

SGMSampleChamberVacuumMoveAction::SGMSampleChamberVacuumMoveAction(SGMSampleChamberVacuumMoveActionInfo *info, QObject *parent) :
	AMListAction3(info, Sequential, parent)
{
	// Identify the beamline object for the sample chamber vacuum.

	vacuum_ = SGMBeamline::sgm()->sampleChamberVacuum();
	AMNumber vacuumSetpoint = vacuumMoveInfo()->setpoint();

	// Check that the vacuum control is valid and able to move before proceeding.

	bool proceedToAction = true;
	QString errorString;

	if (!vacuum_) {
		errorString = QString("Failed to move the given sample chamber vacuum control--control is invalid.");
		AMErrorMon::alert(this, SGMSAMPLECHAMBERVACUUMMOVEACTION_INVALID_CONTROL, errorString);
		setFailed(errorString);
		proceedToAction = false;

	} else if (!vacuum_->isConnected()) {
		errorString = QString("Failed to move %1: control is not connected.").arg(vacuum_->name());
		AMErrorMon::alert(this, SGMSAMPLECHAMBERVACUUMMOVEACTION_CONTROL_NOT_CONNECTED, errorString);
		setFailed(errorString);
		proceedToAction = false;

	} else if (!vacuum_->canMove()) {
		errorString = QString("Failed to move %1: control cannot move.").arg(vacuum_->name());
		AMErrorMon::alert(this, SGMSAMPLECHAMBERVACUUMMOVEACTION_CONTROL_CANNOT_MOVE, errorString);
		setFailed(errorString);
		proceedToAction = false;

	} else if (vacuum_->isMoving() && !vacuum_->allowsMovesWhileMoving()) {
		errorString = QString("Failed to move %1: control is already moving.").arg(vacuum_->name());
		AMErrorMon::alert(this, SGMSAMPLECHAMBERVACUUMMOVEACTION_CONTROL_ALREADY_MOVING, errorString);
		setFailed(errorString);
		proceedToAction = false;

	} else if (!vacuum_->validSetpoint(double(vacuumSetpoint))) {
		errorString = QString("Failed to move %1: provided setpoint is invalid.").arg(vacuum_->name());
		AMErrorMon::alert(this, SGMSAMPLECHAMBERVACUUMMOVEACTION_INVALID_SETPOINT, errorString);
		setFailed(errorString);
		proceedToAction = false;
	}

	// If the vacuum control is valid and can move, attempt to find the correct
	// set of actions that will take its value from the current value to the new
	// desired value.

	// This part might be a little restrictive--eg. it would be nice to have a set of
	// actions that could get the chamber vented from any state. Action fails if
	// current state is other than what is listed.

	AMAction3 *action = 0;
	double oldVacuum = vacuum_->value();
	double newVacuum = double(vacuumSetpoint);

	if (proceedToAction) {
		if (oldVacuum == SGMSampleChamberVacuum::Vented && newVacuum == SGMSampleChamberVacuum::RoughVacuum)
			action = createMoveToVentedFromVacuumAction();
		else if (oldVacuum == SGMSampleChamberVacuum::Vented && newVacuum == SGMSampleChamberVacuum::HighVacuum)
			action = createMoveToVentedFromVacuumAction();
		else if (oldVacuum == SGMSampleChamberVacuum::RoughVacuum && newVacuum == SGMSampleChamberVacuum::Vented)
			action = createMoveToRoughVacuumFromVentedAction();
		else if (oldVacuum == SGMSampleChamberVacuum::RoughVacuum && newVacuum == SGMSampleChamberVacuum::HighVacuum)
			action = createMoveToRoughVacuumFromHighVacuumAction();
		else if (oldVacuum == SGMSampleChamberVacuum::HighVacuum && newVacuum == SGMSampleChamberVacuum::Vented)
			action = createMoveToHighVacuumFromVentedAction();
		else if (oldVacuum == SGMSampleChamberVacuum::HighVacuum && newVacuum == SGMSampleChamberVacuum::RoughVacuum)
			action = createMoveToHighVacuumFromRoughVacuumAction();
	}

	// If an appropriate course was found, add the action.
	// Otherwise, this action should fail.

	if (action) {
		addSubAction(action);

	} else {
		errorString = QString("Failed to move %1: unable to resolve the move from %2 to %3 into valid actions.").arg(vacuum_->name()).arg(oldVacuum).arg(newVacuum);
		AMErrorMon::alert(this, SGMSAMPLECHAMBERVACUUMMOVEACTION_CONTROL_SUBACTION_INVALID, errorString);
		setFailed(errorString);
	}
}

SGMSampleChamberVacuumMoveAction::SGMSampleChamberVacuumMoveAction(const SGMSampleChamberVacuumMoveAction &original) :
	AMListAction3(original)
{

}

SGMSampleChamberVacuumMoveAction::~SGMSampleChamberVacuumMoveAction()
{

}

AMAction3* SGMSampleChamberVacuumMoveAction::createMoveToVentedFromVacuumAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Venting sample chamber", "Venting sample chamber"), AMListAction3::Sequential);

	// First, we would prompt user for action--select gas of choice.
	QString instruction = "Prepare manifold with gas to vent the chamber.";
	action->addSubAction(waitForInput(instruction));

	// Next, turn off chamber pumps, if on.

	action->addSubAction(turnOffChamberTurbos());
	action->addSubAction(turnOffChamberRoughingPump());

	// Next, setup the VAT valve and move valve to OPEN.

	action->addSubAction(openVATValve(SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MIN));

	// Finally, wait for the chamber pressure to reach Vented pressure.

	action->addSubAction(AMActionSupport::buildControlWaitAction(vacuum_, SGMSampleChamberVacuum::Vented, SGMSAMPLECHAMBERVACUUM_MOVE_TIMEOUT));

	// Return action.

	return action;
}

AMAction3* SGMSampleChamberVacuumMoveAction::createMoveToRoughVacuumFromVentedAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Evacuating sample chamber to rough vacuum", "Evacuating sample chamber to rough vacuum"), AMListAction3::Sequential);

	// First, we would close the VAT valve.

	action->addSubAction(closeVATValve(SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MAX));

	// Next, prompt user for action--close any open gas lines.
	QString instruction = "Close any open gas lines.";
	action->addSubAction(waitForInput(instruction));

	// Next, open the VAT valve.

	action->addSubAction(openVATValve(SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MIN));

	// Next, start up the chamber roughing pump.

	action->addSubAction(turnOnChamberRoughingPump());

	// Next, close VAT valve.

	action->addSubAction(closeVATValve(SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MAX));

	// Finally, wait for chamber pressure to drop.

	action->addSubAction(AMActionSupport::buildControlWaitAction(vacuum_, SGMSampleChamberVacuum::RoughVacuum, SGMSAMPLECHAMBERVACUUM_MOVE_TIMEOUT));

	// Return action.

	return action;
}

AMAction3* SGMSampleChamberVacuumMoveAction::createMoveToRoughVacuumFromHighVacuumAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Evacuating sample chamber to rough vacuum", "Evacuating sample chamber to rough vacuum"), AMListAction3::Sequential);

	// Turn off the chamber turbos.

	action->addSubAction(turnOffChamberTurbos());

	// Finally, wait for chamber pressure to drop.

	action->addSubAction(AMActionSupport::buildControlWaitAction(vacuum_, SGMSampleChamberVacuum::RoughVacuum, SGMSAMPLECHAMBERVACUUM_MOVE_TIMEOUT));

	// Return action.

	return action;
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

	action->addSubAction(AMActionSupport::buildControlWaitAction(vacuum_, SGMSampleChamberVacuum::HighVacuum, SGMSAMPLECHAMBERVACUUM_MOVE_TIMEOUT));

	// Return action.

	return action;
}

AMAction3* SGMSampleChamberVacuumMoveAction::waitForInput(const QString &instruction)
{
	return AMActionSupport::buildTimeoutAction(SGMSAMPLECHAMBERVACUUMMOVEACTION_INPUT_TIMEOUT, instruction);
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

		AMControlSet *turbos = vacuum_->turbos();

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

