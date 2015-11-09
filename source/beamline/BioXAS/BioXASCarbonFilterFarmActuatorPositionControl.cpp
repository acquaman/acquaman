#include "BioXASCarbonFilterFarmActuatorPositionControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASCarbonFilterFarmActuatorPositionControl::BioXASCarbonFilterFarmActuatorPositionControl(const QString &name, const QString &units, QObject *parent) :
	AMPseudoMotorControl(name, units, parent)
{
	// Initialize local variables.

	position_ = 0;
	status_ = 0;

	// Initialize inherited variables.

	value_ = 0;
	setpoint_ = 0;
	minimumValue_ = -1000;
	maximumValue_ = 1000;

	setTolerance(0.05);
	setContextKnownDescription("Actuator Control");
	setAllowsMovesWhileMoving(false);
}

BioXASCarbonFilterFarmActuatorPositionControl::~BioXASCarbonFilterFarmActuatorPositionControl()
{

}

bool BioXASCarbonFilterFarmActuatorPositionControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = position_->canMeasure();

	return result;
}

bool BioXASCarbonFilterFarmActuatorPositionControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = position_->canMove();

	return result;
}

bool BioXASCarbonFilterFarmActuatorPositionControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = position_->canStop();

	return result;
}

void BioXASCarbonFilterFarmActuatorPositionControl::setPositionControl(AMControl *newControl)
{
	if (position_ != newControl) {

		if (position_)
			removeChildControl(position_);

		position_ = newControl;

		if (position_)
			addChildControl(position_);

		updateStates();

		emit positionControlChanged(position_);
	}
}

void BioXASCarbonFilterFarmActuatorPositionControl::setStatusControl(AMControl *newControl)
{
	if (status_ != newControl) {

		if (status_)
			removeChildControl(status_);

		status_ = newControl;

		if (status_)
			addChildControl(status_);

		updateStates();

		emit statusControlChanged(status_);
	}
}

void BioXASCarbonFilterFarmActuatorPositionControl::updateConnected()
{
	bool isConnected = (
				position_ && position_->isConnected() &&
				status_ && status_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASCarbonFilterFarmActuatorPositionControl::updateValue()
{
	double newValue = 0;

	if (isConnected())
		newValue = position_->value();

	setValue(newValue);
}

void BioXASCarbonFilterFarmActuatorPositionControl::updateMoving()
{
	bool isMoving = false;

	if (isConnected())
		isMoving = position_->isMoving();

	setIsMoving(isMoving);
}

AMAction3* BioXASCarbonFilterFarmActuatorPositionControl::createMoveAction(double setpoint)
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Move BioXAS Carbon Filter Farm Actuator", "Move BioXAS Carbon Filter Farm Actuator"), AMListAction3::Sequential);

	AMAction3 *move = AMActionSupport::buildControlMoveAction(position_, setpoint);
	action->addSubAction(move);

	AMAction3 *check = AMActionSupport::buildControlWaitAction(status_, InPosition, TIMEOUT_MOVE);
	action->addSubAction(check);

	return action;
}
