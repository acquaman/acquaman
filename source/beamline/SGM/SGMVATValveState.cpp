#include "SGMVATValveState.h"
#include "source/beamline/AMPVControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "beamline/SGM/SGMVATValve.h"

SGMVATValveState::SGMVATValveState(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Setup value options.

	addValueOption(Closed, "Closed", SGMVATVALVE_CLOSED_SETPOINT);
	addValueOption(Open, "Open", SGMVATVALVE_OPEN_SETPOINT, SGMVATVALVE_OPEN_MIN, SGMVATVALVE_OPEN_MAX);
}

SGMVATValveState::~SGMVATValveState()
{

}

void SGMVATValveState::setPosition(AMPVControl *newControl)
{
	if (AMSingleEnumeratedControl::setBaseControl(newControl))
		emit positionChanged(newControl);
}

void SGMVATValveState::setSpeed(AMControl *newControl)
{
	if (speed_ != newControl) {

		if (speed_)
			removeChildControl(speed_);

		speed_ = newControl;

		if (speed_)
			addChildControl(speed_);

		emit speedChanged(speed_);
	}
}

AMAction3* SGMVATValveState::createMoveAction(double indexSetpoint)
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Moving leak valve", "Moving leak valve"), AMListAction3::Sequential);

	AMAction3 *updateSpeed = 0;

	// If the valve is opening, always want to move at the min speed.

	if (int(indexSetpoint) == Open)
		updateSpeed = AMActionSupport::buildControlMoveAction(speed_, SGMVATVALVE_SPEED_MIN);
	else
		updateSpeed = AMActionSupport::buildControlMoveAction(speed_, SGMVATVALVE_SPEED_MAX);

	// Add modifying the speed to the list of actions.

	action->addSubAction(updateSpeed);

	// Add the valve position change.

	action->addSubAction(AMSingleEnumeratedControl::createMoveAction(indexSetpoint));

	// Return the action.

	return action;
}

void SGMVATValveState::updateConnected()
{
	if(speed_) {
		setConnected(speed_->isConnected());
	}
}

