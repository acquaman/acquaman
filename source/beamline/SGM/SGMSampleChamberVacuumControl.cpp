#include "SGMSampleChamberVacuumControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

SGMSampleChamberVacuumControl::SGMSampleChamberVacuumControl(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Initialize class variables.

	turbos_ = 0;
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

void SGMSampleChamberVacuumControl::setTurbos(AMControlSet *newControls)
{
	if (turbos_ != newControls) {

		if (turbos_)
			disconnect( turbos_, 0, this, 0 );

		turbos_ = newControls;

		if (turbos_)
			connect( turbos_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		updateConnected();

		emit turbosChanged(turbos_);
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
				turbos_ && turbos_->isConnected() &&
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

	if (turbos_) {
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

AMAction3* SGMSampleChamberVacuumControl::checkChamberTurbosOff()
{
	AMAction3 *result = 0;

	if (turbos_) {
		AMListAction3 *checkTurbos = new AMListAction3(new AMListActionInfo3("Turning on chamber turbos", "Turning on chamber turbos"), AMListAction3::Parallel);

		for (int turboIndex = 0, turboCount = turbos_->count(); turboIndex < turboCount; turboIndex++) {
			AMAction3 *checkTurbo = checkTurboOff(qobject_cast<SGMTurboPump*>(turbos_->at(turboIndex)));

			if (checkTurbo)
				checkTurbos->addSubAction(checkTurbo);
		}

		result = checkTurbos;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumControl::turnOnChamberTurbos()
{
	AMAction3 *result = 0;

	if (turbos_) {
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

AMAction3* SGMSampleChamberVacuumControl::checkChamberTurbosOn()
{
	AMAction3 *result = 0;

	if (turbos_) {
		AMListAction3 *checkTurbos = new AMListAction3(new AMListActionInfo3("Turning on chamber turbos", "Turning on chamber turbos"), AMListAction3::Parallel);

		for (int turboIndex = 0, turboCount = turbos_->count(); turboIndex < turboCount; turboIndex++) {
			AMAction3 *checkTurbo = checkTurboOn(qobject_cast<SGMTurboPump*>(turbos_->at(turboIndex)));

			if (checkTurbo)
				checkTurbos->addSubAction(checkTurbo);
		}

		result = checkTurbos;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumControl::closeVATValve(double speed)
{
	AMAction3 *result = 0;

	if (vatValve_) {
		AMListAction3 *closeValve = new AMListAction3(new AMListActionInfo3("Close VAT valve", "Close VAT valve"), AMListAction3::Sequential);
		closeValve->addSubAction(AMActionSupport::buildControlMoveAction(vatValve_->speed(), speed));
		closeValve->addSubAction(AMActionSupport::buildControlMoveAction(vatValve_, SGMVATValve::Closed));

		result = closeValve;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumControl::checkVATValveClosed()
{
	AMAction3 *result = 0;

	if (vatValve_)
		result = AMActionSupport::buildControlWaitAction(vatValve_, SGMVATValve::Closed, SGMSAMPLECHAMBERVACUUMCONTROL_VATVALVE_TIMEOUT);

	return result;
}

AMAction3* SGMSampleChamberVacuumControl::openVATValve(double speed)
{
	AMAction3 *result = 0;

	if (vatValve_) {
		AMListAction3 *openValve = new AMListAction3(new AMListActionInfo3("Open VAT valve", "Open VAT valve"), AMListAction3::Sequential);
		openValve->addSubAction(AMActionSupport::buildControlMoveAction(vatValve_->speed(), speed));
		openValve->addSubAction(AMActionSupport::buildControlMoveAction(vatValve_, SGMVATValve::Open));

		result = openValve;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumControl::checkVATValveOpen()
{
	AMAction3 *result = 0;

	if (vatValve_)
		result = AMActionSupport::buildControlWaitAction(vatValve_, SGMVATValve::Open, SGMSAMPLECHAMBERVACUUMCONTROL_VATVALVE_TIMEOUT);

	return result;
}
