#include "AMPseudoMotorLadderControl.h"
#include "actions3/AMActionSupport.h"

AMPseudoMotorLadderControl::AMPseudoMotorLadderControl(const QString &name, const QString &units, QObject *parent) :
	AMPseudoMotorControl(name, units, parent)
{
	// Initialize local variables.

	control_ = 0;

	// Initialize inherited variables.

	value_ = -1;
	setpoint_ = -1;
	minimumValue_ = 0;
	maximumValue_ = 0;
}

AMPseudoMotorLadderControl::~AMPseudoMotorLadderControl()
{

}

bool AMPseudoMotorLadderControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = control_->canMeasure();

	return result;
}

bool AMPseudoMotorLadderControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = control_->canMove();

	return result;
}

bool AMPseudoMotorLadderControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = control_->canStop();

	return result;
}

bool AMPseudoMotorLadderControl::validValue(double value) const
{
	return (value >= 0 && value < enumNames().count());
}

bool AMPseudoMotorLadderControl::validSetpoint(double value) const
{
	return (value >= 0 && value < moveEnumNames().count());
}

int AMPseudoMotorLadderControl::indexOf(const QString &name) const
{
	return enumNames().indexOf(name);
}

void AMPseudoMotorLadderControl::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			removeChildControl(control_);

		control_ = newControl;

		if (control_)
			addChildControl(control_);

		updateStates();

		emit controlChanged(control_);
	}
}

void AMPseudoMotorLadderControl::updateStates()
{
	updateConnected();
	updateEnumStates();
	updateMaximumValue();
	updateValue();
	updateMoving();
}

void AMPseudoMotorLadderControl::updateConnected()
{
	bool isConnected = ( control_ && control_->isConnected() );
	setConnected(isConnected);
}

void AMPseudoMotorLadderControl::updateEnumStates()
{
	setEnumStates(generateEnumStates());
	setMoveEnumStates(generateMoveEnumStates());
}

void AMPseudoMotorLadderControl::updateMaximumValue()
{
	setMaximumValue(enumNames().count() - 1);
}

void AMPseudoMotorLadderControl::updateMoving()
{
	bool isMoving = ( control_ && control_->isMoving());
	setIsMoving(isMoving);
}
