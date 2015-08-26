#include "BioXASCarbonFilterFarmActuatorControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASCarbonFilterFarmActuatorControl::BioXASCarbonFilterFarmActuatorControl(const QString &name, const QString &units, AMControl *positionControl, AMControl *statusControl, QObject *parent) :
	BioXASCarbonFilterFarmControl(name, units, parent)
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

	// Current settings.

	setPositionControl(positionControl);
	setStatusControl(statusControl);
}

BioXASCarbonFilterFarmActuatorControl::~BioXASCarbonFilterFarmActuatorControl()
{

}

bool BioXASCarbonFilterFarmActuatorControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = position_->canMeasure();

	return result;
}

bool BioXASCarbonFilterFarmActuatorControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = position_->canMove();

	return result;
}

bool BioXASCarbonFilterFarmActuatorControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = position_->canStop();

	return result;
}

void BioXASCarbonFilterFarmActuatorControl::setPositionControl(AMControl *newControl)
{
	if (position_ != newControl) {

		if (position_)
			removeChildControl(position_);

		position_ = newControl;

		if (position_)
			addChildControl(position_);

		emit positionControlChanged(position_);

		updateStates();
	}
}

void BioXASCarbonFilterFarmActuatorControl::setStatusControl(AMControl *newControl)
{
	if (status_ != newControl) {

		if (status_)
			removeChildControl(status_);

		status_ = newControl;

		if (status_)
			addChildControl(status_);

		emit statusControlChanged(status_);

		updateStates();
	}
}

void BioXASCarbonFilterFarmActuatorControl::updateConnected()
{
	bool isConnected = (
				position_ && position_->isConnected() &&
				status_ && status_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASCarbonFilterFarmActuatorControl::updateValue()
{
	double newValue = 0;

	if (isConnected())
		newValue = position_->value();

	setValue(newValue);
}

void BioXASCarbonFilterFarmActuatorControl::updateMoving()
{
	bool isMoving = false;

	if (isConnected())
		isMoving = position_->isMoving();

	setIsMoving(isMoving);
}

AMAction3* BioXASCarbonFilterFarmActuatorControl::createMoveAction(double setpoint)
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Move BioXAS Carbon Filter Farm Actuator", "Move BioXAS Carbon Filter Farm Actuator"), AMListAction3::Sequential);

	AMAction3 *move = AMActionSupport::buildControlMoveAction(position_, setpoint);
	action->addSubAction(move);

	AMAction3 *check = AMActionSupport::buildControlWaitAction(status_, BioXASCarbonFilterFarm::Actuator::InPosition, TIMEOUT_MOVE);
	action->addSubAction(check);

	return action;
}
