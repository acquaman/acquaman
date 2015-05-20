#include "VESPERSXIAFilterActuator.h"

VESPERSXIAFilterActuator::VESPERSXIAFilterActuator(const QString &name, AMControl *toggleControl, AMControl *stateControl, QObject *parent)
	: AMPseudoMotorControl(name, "", parent)
{
	toggle_ = 0;
	state_ = 0;

	value_ = Invalid;
	setpoint_ = Invalid;

	setAllowsMovesWhileMoving(false);
	setContextKnownDescription(QString("%1 Actuator Control").arg(name));

	connect( this, SIGNAL(connected(bool)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(valueChanged(double)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(setpointChanged(double)), this, SIGNAL(enumChanged()) );

	setToggleControl(toggleControl);
	setStatusControl(stateControl);

	updateValue();
}

VESPERSXIAFilterActuator::~VESPERSXIAFilterActuator()
{

}

bool VESPERSXIAFilterActuator::canMeasure() const
{
	return isConnected() && state_->canMeasure();
}

bool VESPERSXIAFilterActuator::canMove() const
{
	return isConnected() && toggle_->canMove();
}

bool VESPERSXIAFilterActuator::canStop() const
{
	return false;
}

void VESPERSXIAFilterActuator::setToggleControl(AMControl *newControl)
{
	if (toggle_ != newControl){

		if (toggle_)
			removeChildControl(toggle_);

		toggle_ = newControl;

		if (toggle_)
			addChildControl(toggle_);
	}
}

void VESPERSXIAFilterActuator::setStatusControl(AMControl *newControl)
{
	if (state_ != newControl){

		if (state_)
			removeChildControl(state_);

		state_ = newControl;

		if (state_)
			addChildControl(state_);
	}
}

void VESPERSXIAFilterActuator::updateConnected()
{
	bool connected = (
				toggle_ && toggle_->isConnected()
				&& state_ && state_->isConnected());

	setConnected(connected);
}

void VESPERSXIAFilterActuator::updateValue()
{
	if (isConnected())
		setValue(0);

	else
		setValue(Invalid);
}

void VESPERSXIAFilterActuator::updateIsMoving()
{
	if (isConnected())
		setIsMoving(true);
}
