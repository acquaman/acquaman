#include "SGMSampleChamberVacuumMoveAction.h"
#include "actions3/AMActionSupport.h"
#include "actions3/SGM/SGMSampleChamberVacuumMoveToVentedAction.h"
#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMTurboPump.h"
#include "beamline/SGM/SGMSampleChamberVacuum.h"
#include "ui/SGM/SGMSampleChamberVacuumMoveActionView.h"

SGMSampleChamberVacuumMoveAction::SGMSampleChamberVacuumMoveAction(SGMSampleChamberVacuumMoveActionInfo *info, QObject *parent) :
	AMListAction3(info, Sequential, parent)
{

}

SGMSampleChamberVacuumMoveAction::SGMSampleChamberVacuumMoveAction(const SGMSampleChamberVacuumMoveAction &original) :
	AMListAction3(original)
{

}

SGMSampleChamberVacuumMoveAction::~SGMSampleChamberVacuumMoveAction()
{

}

AMAction3* SGMSampleChamberVacuumMoveAction::waitForInput(const QString &instruction)
{
	return AMActionSupport::buildTimeoutAction(SGMSAMPLECHAMBERVACUUMMOVEACTION_INPUT_TIMEOUT, instruction);
}

AMAction3* SGMSampleChamberVacuumMoveAction::waitForVacuum(double setpoint)
{
	AMAction3 *result = 0;

	if (SGMBeamline::sgm()->sampleChamberVacuum()) {
		result = AMActionSupport::buildControlWaitAction(SGMBeamline::sgm()->sampleChamberVacuum(), setpoint);
		result->info()->setLongDescription("Waiting for sample chamber vacuum to reach " + SGMBeamline::sgm()->sampleChamberVacuum()->indexToString(int(setpoint)));
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::closeVATValve(double speed)
{
	AMAction3 *result = 0;

	if (SGMBeamline::sgm()->sampleChamberVacuum() && SGMBeamline::sgm()->sampleChamberVacuum()->vatValve()) {
		AMListAction3 *closeValve = new AMListAction3(new AMListActionInfo3("Closing VAT valve", "Closing VAT valve"), AMListAction3::Sequential);
		closeValve->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->sampleChamberVacuum()->vatValve()->speed(), speed));
		closeValve->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->sampleChamberVacuum()->vatValve(), SGMVATValve::Closed));

		result = closeValve;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::checkVATValveClosed()
{
	AMAction3 *result = 0;

	if (SGMBeamline::sgm()->sampleChamberVacuum() && SGMBeamline::sgm()->sampleChamberVacuum()->vatValve()) {
		result = AMActionSupport::buildControlWaitAction(SGMBeamline::sgm()->sampleChamberVacuum()->vatValve(), SGMVATValve::Closed, SGMSAMPLECHAMBERVACUUMCONTROL_VATVALVE_TIMEOUT);
		result->info()->setLongDescription("Checking that the VAT valve is closed");
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::openVATValve(double speed)
{
	AMAction3 *result = 0;

	if (SGMBeamline::sgm()->sampleChamberVacuum() && SGMBeamline::sgm()->sampleChamberVacuum()->vatValve()) {
		AMListAction3 *openValve = new AMListAction3(new AMListActionInfo3("Opening VAT valve", "Opening VAT valve"), AMListAction3::Sequential);
		openValve->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->sampleChamberVacuum()->vatValve()->speed(), speed));
		openValve->addSubAction(AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->sampleChamberVacuum()->vatValve(), SGMVATValve::Open));

		result = openValve;
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::checkVATValveOpen()
{
	AMAction3 *result = 0;

	if (SGMBeamline::sgm()->sampleChamberVacuum() && SGMBeamline::sgm()->sampleChamberVacuum()->vatValve()) {
		result = AMActionSupport::buildControlWaitAction(SGMBeamline::sgm()->sampleChamberVacuum()->vatValve(), SGMVATValve::Open, SGMSAMPLECHAMBERVACUUMCONTROL_VATVALVE_TIMEOUT);
		result->info()->setLongDescription("Checking that the VAT valve is open");
	}

	return result;
}

AMAction3* SGMSampleChamberVacuumMoveAction::turnOffChamberTurbos()
{
	AMAction3 *result = 0;

	if (SGMBeamline::sgm()->sampleChamberVacuum() && SGMBeamline::sgm()->sampleChamberVacuum()->turbos()) {
		AMListAction3 *changeTurbos = new AMListAction3(new AMListActionInfo3("Turning off chamber turbos", "Turning off chamber turbos"), AMListAction3::Parallel);

		AMControlSet *turbos = SGMBeamline::sgm()->sampleChamberVacuum()->turbos();

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

	if (SGMBeamline::sgm()->sampleChamberVacuum() && SGMBeamline::sgm()->sampleChamberVacuum()->turbos()) {
		AMListAction3 *checkTurbos = new AMListAction3(new AMListActionInfo3("Checking that chamber turbos are off", "Checking that chamber turbos are off"), AMListAction3::Parallel);

		AMControlSet *turbos = SGMBeamline::sgm()->sampleChamberVacuum()->turbos();

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

	if (SGMBeamline::sgm()->sampleChamberVacuum() && SGMBeamline::sgm()->sampleChamberVacuum()->turbos()) {
		AMListAction3 *changeTurbos = new AMListAction3(new AMListActionInfo3("Turning on chamber turbos", "Turning on chamber turbos"), AMListAction3::Parallel);

		AMControlSet *turbos = SGMBeamline::sgm()->sampleChamberVacuum()->turbos();

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

	if (SGMBeamline::sgm()->sampleChamberVacuum() && SGMBeamline::sgm()->sampleChamberVacuum()->turbos()) {
		AMListAction3 *checkTurbos = new AMListAction3(new AMListActionInfo3("Checking that chamber turbos are on", "Checking that chamber turbos are on"), AMListAction3::Parallel);

		AMControlSet *turbos = SGMBeamline::sgm()->sampleChamberVacuum()->turbos();

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
