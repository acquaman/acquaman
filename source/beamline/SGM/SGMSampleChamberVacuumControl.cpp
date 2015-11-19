#include "SGMSampleChamberVacuumControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

SGMSampleChamberVacuumControl::SGMSampleChamberVacuumControl(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Initialize class variables.

	turbo5_ = 0;
	turbo6_ = 0;
	vatValve_ = 0;
}

SGMSampleChamberVacuumControl::~SGMSampleChamberVacuumControl()
{

}

void SGMSampleChamberVacuumControl::setPressure(AMControl *newControl)
{
	if (setBaseControl(newControl))
		emit pressureChanged(control_);
}

void SGMSampleChamberVacuumControl::setTurbo5(SGMTurboPump *newControl)
{
	if (turbo5_ != newControl) {

		if (turbo5_)
			removeChildControl(turbo5_);

		turbo5_ = newControl;

		if (turbo5_)
			addChildControl(turbo5_);

		updateConnected();

		emit turbo5Changed(turbo5_);
	}
}

void SGMSampleChamberVacuumControl::setTurbo6(SGMTurboPump *newControl)
{
	if (turbo6_ != newControl) {

		if (turbo6_)
			removeChildControl(turbo6_);

		turbo6_ = newControl;

		if (turbo6_)
			addChildControl(turbo6_);

		updateConnected();

		emit turbo6Changed(turbo6_);
	}
}

void SGMSampleChamberVacuumControl::setVATValve(SGMVATValve *newControl)
{
	if (vatValve_ != newControl) {

		if (vatValve_)
			removeChildControl(vatValve_);

		vatValve_ = newControl;

		if (vatValve_)
			addChildControl(vatValve_);

		updateConnected();

		emit vatValveChanged(vatValve_);
	}
}

void SGMSampleChamberVacuumControl::updateConnected()
{
	bool isConnected = (
				AMSingleEnumeratedControl::isConnected() &&
				turbo5_ && turbo5_->isConnected() &&
				turbo6_ && turbo6_->isConnected() &&
				vatValve_ && vatValve_->isConnected()
				);

	setConnected(isConnected);
}

bool SGMSampleChamberVacuumControl::addVacuumOption(int index, const QString &optionString, double valueMin, double valueMax)
{
	bool result = addValueOption(index, optionString, index, valueMin, valueMax);

	if (result)
		emit vacuumOptionsChanged();

	return result;
}

bool SGMSampleChamberVacuumControl::removeVacuumOption(int index)
{
	bool result = removeOption(index);

	if (result)
		emit vacuumOptionsChanged();

	return result;
}

AMAction3* SGMSampleChamberVacuumControl::turnOffTurbo(SGMTurboPump *turbo)
{
	AMAction3 *result = 0;

	if (turbo)
		result = AMActionSupport::buildControlMoveAction(turbo->running(), SGMTurboPump::NotRunning);

	return result;
}

AMAction3* SGMSampleChamberVacuumControl::checkTurboOff(SGMTurboPump *turbo)
{
	AMAction3 *result = 0;

	if (turbo)
		result = AMActionSupport::buildControlWaitAction(turbo->running(), SGMTurboPump::NotRunning, SGMSAMPLECHAMBERVACUUMCONTROL_TURBO_TIMEOUT);

	return result;
}

AMAction3* SGMSampleChamberVacuumControl::turnOnTurbo(SGMTurboPump *turbo)
{
	AMAction3 *result = 0;

	if (turbo)
		result = AMActionSupport::buildControlMoveAction(turbo->running(), SGMTurboPump::Running);

	return result;
}

AMAction3* SGMSampleChamberVacuumControl::checkTurboOn(SGMTurboPump *turbo)
{
	AMAction3 *result = 0;

	if (turbo)
		result = AMActionSupport::buildControlWaitAction(turbo->running(), SGMTurboPump::Running, SGMSAMPLECHAMBERVACUUMCONTROL_TURBO_TIMEOUT);

	return result;
}

AMAction3* SGMSampleChamberVacuumControl::turnOffChamberTurbos()
{
	AMAction3 *result = 0;

	if (turbo5_ && turbo6_) {
		AMListAction3 *changeTurbos = new AMListAction3(new AMListActionInfo3("Turning off chamber turbos", "Turning off chamber turbos"), AMListAction3::Parallel);
		changeTurbos->addSubAction(turnOffTurbo(turbo5_));
		changeTurbos->addSubAction(turnOffTurbo(turbo6_));

		result = changeTurbos;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumControl::checkChamberTurbosOff()
{
	AMAction3 *result = 0;

	if (turbo5_ && turbo6_) {
		AMListAction3 *checkTurbos = new AMListAction3(new AMListActionInfo3("Checking chamber turbos are off", "Checking chamber turbos are off"), AMListAction3::Parallel);
		checkTurbos->addSubAction(checkTurboOff(turbo5_));
		checkTurbos->addSubAction(checkTurboOff(turbo6_));

		result = checkTurbos;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumControl::turnOnChamberTurbos()
{
	AMAction3 *result = 0;

	if (turbo5_ && turbo6_) {
		AMListAction3 *changeTurbos = new AMListAction3(new AMListActionInfo3("Turning off chamber turbos", "Turning off chamber turbos"), AMListAction3::Parallel);
		changeTurbos->addSubAction(turnOnTurbo(turbo5_));
		changeTurbos->addSubAction(turnOnTurbo(turbo6_));

		result = changeTurbos;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumControl::checkChamberTurbosOn()
{
	AMAction3 *result = 0;

	if (turbo5_ && turbo6_) {
		AMListAction3 *checkTurbos = new AMListAction3(new AMListActionInfo3("Checking chamber turbos are on", "Checking chamber turbos are on"), AMListAction3::Parallel);
		checkTurbos->addSubAction(checkTurboOn(turbo5_));
		checkTurbos->addSubAction(checkTurboOn(turbo6_));

		result = checkTurbos;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumControl::closeVATValve(SGMVATValve *valve, double valveSpeed)
{
	AMAction3 *result = 0;

	if (valve)
		result = valve->createMoveAtSpeedAction(SGMVATValve::Closed, valveSpeed);

	return result;
}

AMAction3* SGMSampleChamberVacuumControl::checkVATValveClosed(SGMVATValve *valve)
{
	AMAction3 *result = 0;

	if (valve)
		result = AMActionSupport::buildControlWaitAction(valve, SGMVATValve::Closed, SGMSAMPLECHAMBERVACUUMCONTROL_VATVALVE_TIMEOUT);

	return result;
}

AMAction3* SGMSampleChamberVacuumControl::openVATValve(SGMVATValve *valve, double valveSpeed)
{
	AMAction3 *result = 0;

	if (valve)
		result = valve->createMoveAtSpeedAction(SGMVATValve::Open, valveSpeed);

	return result;
}

AMAction3* SGMSampleChamberVacuumControl::checkVATValveOpen(SGMVATValve *valve)
{
	AMAction3 *result = 0;

	if (valve)
		result = AMActionSupport::buildControlWaitAction(valve, SGMVATValve::Open, SGMSAMPLECHAMBERVACUUMCONTROL_VATVALVE_TIMEOUT);

	return result;
}
