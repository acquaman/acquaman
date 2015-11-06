#include "AMPseudoMotorEnumeratedControl.h"
#include "actions3/AMActionSupport.h"

AMPseudoMotorEnumeratedControl::AMPseudoMotorEnumeratedControl(const QString &name, const QString &units, QObject *parent) :
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

AMPseudoMotorEnumeratedControl::~AMPseudoMotorEnumeratedControl()
{

}

bool AMPseudoMotorEnumeratedControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = control_->canMeasure();

	return result;
}

bool AMPseudoMotorEnumeratedControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = control_->canMove();

	return result;
}

bool AMPseudoMotorEnumeratedControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = control_->canStop();

	return result;
}

bool AMPseudoMotorEnumeratedControl::validValue(double value) const
{
	return (value >= 0 && value < enumNames().count());
}

bool AMPseudoMotorEnumeratedControl::validSetpoint(double value) const
{
	return (value >= 0 && value < moveEnumNames().count());
}

int AMPseudoMotorEnumeratedControl::indexOf(const QString &name) const
{
	return enumNames().indexOf(name);
}

void AMPseudoMotorEnumeratedControl::setControl(AMControl *newControl)
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

void AMPseudoMotorEnumeratedControl::updateStates()
{
	updateConnected();
	updateEnumStates();
	updateMaximumValue();
	updateValue();
	updateMoving();
}

void AMPseudoMotorEnumeratedControl::updateConnected()
{
	bool isConnected = ( control_ && control_->isConnected() );
	setConnected(isConnected);
}

void AMPseudoMotorEnumeratedControl::updateEnumStates()
{
	setEnumStates(generateEnumStates());
	setMoveEnumStates(generateMoveEnumStates());
}

void AMPseudoMotorEnumeratedControl::updateMaximumValue()
{
	setMaximumValue(enumNames().count() - 1);
}

void AMPseudoMotorEnumeratedControl::updateMoving()
{
	bool isMoving = ( control_ && control_->isMoving());
	setIsMoving(isMoving);
}
