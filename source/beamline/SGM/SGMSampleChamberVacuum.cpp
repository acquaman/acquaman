#include "SGMSampleChamberVacuum.h"
#include "beamline/AMPVControl.h"
#include "beamline/SGM/SGMSampleChamberVacuumMoveControl.h"
#include "actions3/AMActionSupport.h"

SGMSampleChamberVacuum::SGMSampleChamberVacuum(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Initialize class variables.

	move_ = 0;

	// Setup available vacuum states.

	addValueOption(Vented, "Vented", Vented, SGMSAMPLECHAMBERVACUUM_VENTED_MIN, SGMSAMPLECHAMBERVACUUM_VENTED_MAX);
	addValueOption(RoughVacuum, "Rough vacuum", RoughVacuum, SGMSAMPLECHAMBERVACUUM_ROUGHVACUUM_MIN, SGMSAMPLECHAMBERVACUUM_ROUGHVACUUM_MAX);
	addValueOption(HighVacuum, "High vacuum", HighVacuum, SGMSAMPLECHAMBERVACUUM_HIGHVACUUM_MIN, SGMSAMPLECHAMBERVACUUM_HIGHVACUUM_MAX);
}

SGMSampleChamberVacuum::~SGMSampleChamberVacuum()
{

}

bool SGMSampleChamberVacuum::canMove() const
{
	bool result = false;

	if (move_)
		result = move_->canMove();

	return result;
}

bool SGMSampleChamberVacuum::canStop() const
{
	bool result = false;

	if (move_)
		result = move_->canStop();

	return result;
}

void SGMSampleChamberVacuum::setPressure(AMControl *newControl)
{
	if (setBaseControl(newControl)) {
		updateMove();
		emit pressureChanged(newControl);
	}
}

void SGMSampleChamberVacuum::setMoveControl(SGMSampleChamberVacuumMoveControl *newControl)
{
	if (move_ != newControl) {

		if (move_)
			removeChildControl(move_);

		move_ = newControl;

		if (move_)
			addChildControl(move_);

		updateMove();
		updateStates();

		emit moveChanged(move_);
	}
}

void SGMSampleChamberVacuum::updatePressure()
{
	if (move_)
		setPressure(move_->pressure());
}

void SGMSampleChamberVacuum::updateMove()
{
	if (move_)
		move_->setPressure(pressure());
}

AMAction3* SGMSampleChamberVacuum::createMoveAction(double setpoint)
{
	return AMActionSupport::buildControlMoveAction(move_, setpoint);
}
