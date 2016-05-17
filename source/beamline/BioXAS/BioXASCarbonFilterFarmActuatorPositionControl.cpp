#include "BioXASCarbonFilterFarmActuatorPositionControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASCarbonFilterFarmActuatorPositionControl::BioXASCarbonFilterFarmActuatorPositionControl(const QString &name, const QString &units, QObject *parent) :
	AMPseudoMotorControl(name, units, parent)
{
	// Initialize local variables.

	motor_ = 0;
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
		result = motor_->canMeasure();

	return result;
}

bool BioXASCarbonFilterFarmActuatorPositionControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = motor_->canMove();

	return result;
}

bool BioXASCarbonFilterFarmActuatorPositionControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = motor_->canStop();

	return result;
}

double BioXASCarbonFilterFarmActuatorPositionControl::statusValue() const
{
	double result = -1;

	if (status_ && status_->canMeasure())
		result = status_->value();

	return result;
}

void BioXASCarbonFilterFarmActuatorPositionControl::setMotor(CLSMAXvMotor *newControl)
{
	if (motor_ != newControl) {

                removeChildControl(motor_);
		motor_ = newControl;
                addChildControl(motor_);

		updateStates();

		emit motorChanged(motor_);
	}
}

void BioXASCarbonFilterFarmActuatorPositionControl::setStatus(AMControl *newControl)
{
	if (status_ != newControl) {

                removeChildControl(status_); // disconnects all signals.
		status_ = newControl;
                if (addChildControl(status_))
			connect( status_, SIGNAL(valueChanged(double)), this, SLOT(onStatusControlValueChanged()) );

		updateStates();

		emit statusChanged(status_);
	}
}

void BioXASCarbonFilterFarmActuatorPositionControl::updateConnected()
{
	bool isConnected = (
				motor_ && motor_->isConnected() &&
				status_ && status_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASCarbonFilterFarmActuatorPositionControl::updateValue()
{
	double newValue = 0;

	if (isConnected())
		newValue = motor_->value();

	setValue(newValue);
}

void BioXASCarbonFilterFarmActuatorPositionControl::updateMoving()
{
	bool isMoving = false;

	if (isConnected())
		isMoving = motor_->isMoving();

	setIsMoving(isMoving);
}

void BioXASCarbonFilterFarmActuatorPositionControl::onStatusControlValueChanged()
{
	emit statusValueChanged( statusValue() );
}

AMAction3* BioXASCarbonFilterFarmActuatorPositionControl::createMoveAction(double setpoint)
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Move BioXAS Carbon Filter Farm Actuator", "Move BioXAS Carbon Filter Farm Actuator"), AMListAction3::Sequential);

	AMAction3 *move = AMActionSupport::buildControlMoveAction(motor_, setpoint);
	action->addSubAction(move);

	AMAction3 *check = AMActionSupport::buildControlWaitAction(status_, InPosition);
	action->addSubAction(check);

	return action;
}
