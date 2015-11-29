#include "SGMSampleChamberVacuumMoveControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"
#include "beamline/AMControlSet.h"
#include "beamline/SGM/SGMSampleChamberVacuum.h"
#include "beamline/SGM/SGMVATValve.h"
#include "beamline/SGM/SGMTurboPump.h"

SGMSampleChamberVacuumMoveControl::SGMSampleChamberVacuumMoveControl(const QString &name, QObject *parent) :
	AMEnumeratedControl(name, "", parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("Vacuum move control");
	setAllowsMovesWhileMoving(false);

	// Initialize class variables.

	vacuum_ = 0;
	doorStatus_ = 0;
	pressure_ = 0;
	turbos_ = 0;
	roughingPump_ = 0;
	vatValve_ = 0;
}

SGMSampleChamberVacuumMoveControl::~SGMSampleChamberVacuumMoveControl()
{

}

bool SGMSampleChamberVacuumMoveControl::canMeasure() const
{
	return false;
}

bool SGMSampleChamberVacuumMoveControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = true;

	return result;
}

bool SGMSampleChamberVacuumMoveControl::canStop() const
{
	return false;
}

void SGMSampleChamberVacuumMoveControl::setVacuum(SGMSampleChamberVacuum *newControl)
{
	if (vacuum_ != newControl) {

		if (vacuum_)
			removeChildControl(vacuum_);

		vacuum_ = newControl;

		if (vacuum_)
			addChildControl(vacuum_);

		updateStates();

		emit vacuumChanged(vacuum_);
	}
}

void SGMSampleChamberVacuumMoveControl::setDoorStatus(AMControl *newControl)
{
	if (doorStatus_ != newControl) {

		if (doorStatus_)
			removeChildControl(doorStatus_);

		doorStatus_ = newControl;

		if (doorStatus_)
			addChildControl(doorStatus_);

		updateStates();

		emit doorStatusChanged(doorStatus_);
	}
}

void SGMSampleChamberVacuumMoveControl::setPressure(AMControl *newControl)
{
	if (pressure_ != newControl) {

		if (pressure_)
			removeChildControl(pressure_);

		pressure_ = newControl;

		if (pressure_)
			addChildControl(pressure_);

		updateStates();

		emit pressureChanged(pressure_);
	}
}

void SGMSampleChamberVacuumMoveControl::setTurbos(AMControlSet *newControls)
{
	if (turbos_ != newControls) {

		if (turbos_)
			disconnect( turbos_, 0, this, 0 );

		turbos_ = newControls;

		if (turbos_)
			connect( turbos_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		updateStates();

		emit turbosChanged(turbos_);
	}
}

void SGMSampleChamberVacuumMoveControl::setRoughingPump(AMControl *newControl)
{
	if (roughingPump_ != newControl) {

		if (roughingPump_)
			removeChildControl(roughingPump_);

		roughingPump_ = newControl;

		if (roughingPump_)
			addChildControl(roughingPump_);

		updateStates();

		emit roughingPumpChanged(roughingPump_);
	}
}

void SGMSampleChamberVacuumMoveControl::setVATValve(SGMVATValve *newControl)
{
	if (vatValve_ != newControl) {

		if (vatValve_)
			removeChildControl(vatValve_);

		vatValve_ = newControl;

		if (vatValve_)
			addChildControl(vatValve_);

		updateStates();

		emit vatValveChanged(vatValve_);
	}
}

void SGMSampleChamberVacuumMoveControl::updateConnected()
{
	bool isConnected = (
				doorStatus_ && doorStatus_->isConnected() &&
				pressure_ && pressure_->isConnected() &&
				turbos_ && turbos_->isConnected() &&
				roughingPump_ && roughingPump_->isConnected() &&
				vatValve_ && vatValve_->isConnected()
				);

	setConnected(isConnected);
}

void SGMSampleChamberVacuumMoveControl::updateMoving()
{

}

AMAction3* SGMSampleChamberVacuumMoveControl::createMoveAction(double setpoint)
{
	Q_UNUSED(setpoint)
	return 0;
}

int SGMSampleChamberVacuumMoveControl::currentIndex() const
{
	return -1;
}

AMAction3* SGMSampleChamberVacuumMoveControl::createMoveToVentedAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Move to Vented", "Move to Vented"), AMListAction3::Sequential);

	// First, we would prompt user for action--select gas of choice.

	QString description = "Attention! Prepare manifold with gas to vent the chamber.";
	action->addSubAction(waitForInput(description));

	// Next, turn off chamber pumps, if on.

	action->addSubAction(turnOffChamberTurbos());

//	action->addSubAction(turnOffChamberRoughingPump());

	// Next, setup the VAT valve and move valve to OPEN.

	action->addSubAction(openVATValve(SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MIN));

	// Finally, wait for the chamber pressure to reach Vented pressure.

	action->addSubAction(waitForVacuum(SGMSampleChamberVacuum::Vented));

	return action;
}

AMAction3* SGMSampleChamberVacuumMoveControl::createMoveToRoughVacuumFromVentedAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Move to RoughVacuum", "Move to RoughVacuum"), AMListAction3::Sequential);

	// First, check that the chamber door is closed.

	action->addSubAction(checkDoorClosed());

	// Next, close the VAT valve.

	action->addSubAction(closeVATValve(SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MAX));

	// Next, prompt user for action--close any open gas lines.

	QString instruction = "Close any open gas lines.";
	action->addSubAction(waitForInput(instruction));

	// Next, open the VAT valve.

	action->addSubAction(openVATValve(SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MIN));

	// Next, start up chamber roughing pump.

	action->addSubAction(turnOnChamberRoughingPump());

	// Next, close VAT valve.

	action->addSubAction(closeVATValve(SGMSAMPLECHAMBERVACUUM_VATVALVE_SPEED_MAX));

	// Finally, wait for chamber pressure to drop to rough vacuum.

	action->addSubAction(waitForVacuum(SGMSampleChamberVacuum::RoughVacuum));

	return action;
}

AMAction3* SGMSampleChamberVacuumMoveControl::createMoveToHighVacuumFromRoughVacuumAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Move to HighVacuum", "Move to HighVacuum"), AMListAction3::Sequential);

	// First, check that chamber pressure is at rough vacuum.

	action->addSubAction(waitForVacuum(SGMSampleChamberVacuum::RoughVacuum));

	// Next, turn on chamber turbos.

	action->addSubAction(turnOnChamberTurbos());

	// Finally, wait for chamber pressure to drop.

	action->addSubAction(waitForVacuum(SGMSampleChamberVacuum::HighVacuum));

	return action;
}

AMAction3* SGMSampleChamberVacuumMoveControl::createMoveToHighVacuumFromVentedAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Move to HighVacuum", "Move to HighVacuum"), AMListAction3::Sequential);

	// Check?

	// First, move to rough vacuum.

	action->addSubAction(createMoveToRoughVacuumFromVentedAction());

	// Finally, move to high vacuum.

	action->addSubAction(createMoveToHighVacuumFromRoughVacuumAction());

	return action;
}

AMAction3* SGMSampleChamberVacuumMoveControl::createMoveToRoughVacuumFromHighVacuumAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Move to RoughVacuum", "Move to RoughVacuum"), AMListAction3::Sequential);

	// First, turn off turbo pumps.

	action->addSubAction(turnOffChamberTurbos());

	// Finally, wait for the chamber pressure to drop to rough vacuum.

	action->addSubAction(waitForVacuum(SGMSampleChamberVacuum::RoughVacuum));

	return action;
}

AMAction3* SGMSampleChamberVacuumMoveControl::waitForInput(const QString &instruction)
{
	AMAction3 *result = AMActionSupport::buildTimeoutAction(SGMSAMPLECHAMBERVACUUMMOVECONTROL_INPUT_TIMEOUT, instruction);
	result->info()->setLongDescription(instruction);

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveControl::waitForVacuum(double setpoint)
{
	AMAction3 *result = 0;

	if (vacuum_) {
		result = AMActionSupport::buildControlWaitAction(vacuum_, setpoint);
		result->info()->setLongDescription("Waiting for sample chamber vacuum to reach " + vacuum_->indexToString(int(setpoint)));
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveControl::closeVATValve(double speed)
{
	AMAction3 *result = 0;

	if (vatValve_) {
		AMListAction3 *closeValve = new AMListAction3(new AMListActionInfo3("Closing VAT valve", "Closing VAT valve"), AMListAction3::Sequential);
		closeValve->addSubAction(AMActionSupport::buildControlMoveAction(vatValve_->speed(), speed));
		closeValve->addSubAction(AMActionSupport::buildControlMoveAction(vatValve_, SGMVATValve::Closed));

		result = closeValve;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveControl::checkVATValveClosed()
{
	AMAction3 *result = 0;

	if (vatValve_) {
		result = AMActionSupport::buildControlWaitAction(vatValve_, SGMVATValve::Closed, SGMSAMPLECHAMBERVACUUMMOVECONTROL_VATVALVE_TIMEOUT);
		result->info()->setLongDescription("Checking that the VAT valve is closed");
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveControl::openVATValve(double speed)
{
	AMAction3 *result = 0;

	if (vatValve_) {
		AMListAction3 *openValve = new AMListAction3(new AMListActionInfo3("Opening VAT valve", "Opening VAT valve"), AMListAction3::Sequential);
		openValve->addSubAction(AMActionSupport::buildControlMoveAction(vatValve_->speed(), speed));
		openValve->addSubAction(AMActionSupport::buildControlMoveAction(vatValve_, SGMVATValve::Open));

		result = openValve;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveControl::checkVATValveOpen()
{
	AMAction3 *result = 0;

	if (vatValve_) {
		result = AMActionSupport::buildControlWaitAction(vatValve_, SGMVATValve::Open, SGMSAMPLECHAMBERVACUUMMOVECONTROL_VATVALVE_TIMEOUT);
		result->info()->setLongDescription("Checking that the VAT valve is open");
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveControl::turnOffChamberTurbos()
{
	AMAction3 *result = 0;

	if (turbos_ && !turbos_->isEmpty()) {
		AMListAction3 *changeTurbos = new AMListAction3(new AMListActionInfo3("Turning off chamber turbos", "Turning off chamber turbos"), AMListAction3::Parallel);

		for (int turboIndex = 0, turboCount = turbos_->count(); turboIndex < turboCount; turboIndex++) {
			AMAction3 *changeTurbo = turnOffTurbo(qobject_cast<SGMTurboPump*>(turbos_->at(turboIndex)));

			if (changeTurbo)
				changeTurbos->addSubAction(changeTurbo);
		}

		result = changeTurbos;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveControl::checkChamberTurbosOff()
{
	AMAction3 *result = 0;

	if (turbos_ && !turbos_->isEmpty()) {
		AMListAction3 *checkTurbos = new AMListAction3(new AMListActionInfo3("Checking that chamber turbos are off", "Checking that chamber turbos are off"), AMListAction3::Parallel);

		for (int turboIndex = 0, turboCount = turbos_->count(); turboIndex < turboCount; turboIndex++) {
			AMAction3 *checkTurbo = checkTurboOff(qobject_cast<SGMTurboPump*>(turbos_->at(turboIndex)));

			if (checkTurbo)
				checkTurbos->addSubAction(checkTurbo);
		}

		result = checkTurbos;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveControl::turnOnChamberTurbos()
{
	AMAction3 *result = 0;

	if (turbos_ && !turbos_->isEmpty()) {
		AMListAction3 *changeTurbos = new AMListAction3(new AMListActionInfo3("Turning on chamber turbos", "Turning on chamber turbos"), AMListAction3::Parallel);

		for (int turboIndex = 0, turboCount = turbos_->count(); turboIndex < turboCount; turboIndex++) {
			AMAction3 *changeTurbo = turnOnTurbo(qobject_cast<SGMTurboPump*>(turbos_->at(turboIndex)));

			if (changeTurbo)
				changeTurbos->addSubAction(changeTurbo);
		}

		result = changeTurbos;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveControl::checkChamberTurbosOn()
{
	AMAction3 *result = 0;

	if (turbos_ && !turbos_->isEmpty()) {
		AMListAction3 *checkTurbos = new AMListAction3(new AMListActionInfo3("Checking that chamber turbos are on", "Checking that chamber turbos are on"), AMListAction3::Parallel);

		for (int turboIndex = 0, turboCount = turbos_->count(); turboIndex < turboCount; turboIndex++) {
			AMAction3 *checkTurbo = checkTurboOn(qobject_cast<SGMTurboPump*>(turbos_->at(turboIndex)));

			if (checkTurbo)
				checkTurbos->addSubAction(checkTurbo);
		}

		result = checkTurbos;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveControl::turnOffTurbo(SGMTurboPump *turbo)
{
	AMAction3 *result = 0;

	if (turbo)
		result = AMActionSupport::buildControlMoveAction(turbo->running(), SGMTurboPump::NotRunning);

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveControl::checkTurboOff(SGMTurboPump *turbo)
{
	AMAction3 *result = 0;

	if (turbo)
		result = AMActionSupport::buildControlWaitAction(turbo->running(), SGMTurboPump::NotRunning, SGMSAMPLECHAMBERVACUUMMOVECONTROL_TURBO_TIMEOUT);

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveControl::turnOnTurbo(SGMTurboPump *turbo)
{
	AMAction3 *result = 0;

	if (turbo)
		result = AMActionSupport::buildControlMoveAction(turbo->running(), SGMTurboPump::Running);

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveControl::checkTurboOn(SGMTurboPump *turbo)
{
	AMAction3 *result = 0;

	if (turbo)
		result = AMActionSupport::buildControlWaitAction(turbo->running(), SGMTurboPump::Running, SGMSAMPLECHAMBERVACUUMMOVECONTROL_TURBO_TIMEOUT);

	return result;
}

