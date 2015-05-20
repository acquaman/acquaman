#include "BioXASSideM1PitchControl.h"
#include <math.h>

BioXASSideM1PitchControl::BioXASSideM1PitchControl(QObject *parent) :
    AMPseudoMotorControl(parent)
{
	// Initialize inherited variables.

	value_ = 0;
	setpoint_ = 0;
	minimumValue_ = -1000000;
	maximumValue_ = 1000000;

	setAllowsMovesWhileMoving(false);
	setTolerance(0.05);
	setContextKnownDescription("Pitch");

	// Initialize member variables.

	upstreamInboard_ = 0;
	upstreamOutboard_ = 0;
	downstream_ = 0;

	// Current settings.

	updateConnected();
	updateValue();
	updateIsMoving();
}

BioXASSideM1PitchControl::~BioXASSideM1PitchControl()
{

}

bool BioXASSideM1PitchControl::canMeasure() const
{
	bool result = false;

	if (isConnected()) {
		result = ( upstreamInboard_->canMeasure() && upstreamOutboard_->canMeasure() && downstream_->canMeasure() );
	}

	return result;
}

bool BioXASSideM1PitchControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = ( upstreamInboard_->canMove() && upstreamOutboard_->canMove() && downstream_->canMove() );

	return result;
}

bool BioXASSideM1PitchControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = ( upstreamInboard_->canStop() && upstreamOutboard_->canStop() && downstream_->canStop() );

	return result;
}

bool BioXASSideM1PitchControl::validValue(double value) const
{
	bool result = false;

	if (value < maximumValue() && value > minimumValue()) {
		result = true;
	}

	return true;
}

bool BioXASSideM1PitchControl::validSetpoint(double value) const
{
	return validValue(value);
}

void BioXASSideM1PitchControl::setUpstreamInboardControl(AMControl *newControl)
{
	if (upstreamInboard_ != newControl) {

		if (upstreamInboard_)
			removeChildControl(upstreamInboard_);

		upstreamInboard_ = newControl;

		if (upstreamInboard_)
			addChildControl(upstreamInboard_);

		updateStates();
	}
}

void BioXASSideM1PitchControl::setUpstreamOutboardControl(AMControl *newControl)
{
	if (upstreamOutboard_ != newControl) {

		if (upstreamOutboard_)
			removeChildControl(upstreamOutboard_);

		upstreamOutboard_ = newControl;

		if (upstreamOutboard_)
			addChildControl(upstreamOutboard_);

		updateStates();
	}
}

void BioXASSideM1PitchControl::setDownstreamControl(AMControl *newControl)
{
	if (downstream_ != newControl) {

		if (downstream_)
			removeChildControl(downstream_);

		downstream_ = newControl;

		if (downstream_)
			addChildControl(downstream_);

		updateStates();
	}
}

void BioXASSideM1PitchControl::updateConnected()
{
	bool isConnected = (
				upstreamInboard_ && upstreamInboard_->isConnected() &&
				upstreamOutboard_ && upstreamOutboard_->isConnected() &&
				downstream_ && downstream_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASSideM1PitchControl::updateValue()
{
	if (isConnected()) {
		setValue( 0 );
	}
}

void BioXASSideM1PitchControl::updateIsMoving()
{
	if (isConnected()) {
		bool isMoving = (
					upstreamInboard_->isMoving() ||
					upstreamOutboard_->isMoving() ||
					downstream_->isMoving()
					);

		setIsMoving(isMoving);
	}
}

AMAction3* BioXASSideM1PitchControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (isConnected()) {

	}

	return result;
}

double BioXASSideM1PitchControl::calculatePitch(double upstreamInboardX, upstreamInboardY, upstreamInboardZ, upstreamOutboardX, upstreamOutboardY, upstreamOutboardZ, downstreamX, downstreamY, downstreamZ)
{
	double numerator = ((downstreamZ - upstreamInboardZ)*(downstreamY - upstreamOutboardY) - (downstreamZ - upstreamOutboardZ)*(downstreamY - upstreamInboardY));
	double denom = ((downstreamX - upstreamInboardX)*(downstreamY - upstreamOutboardY) + (upstreamOutboardX - downstreamX)*(downstreamY - upstreamInboardY));
	double pitch = atan( numerator / denom ) * 180/M_PI;

	return pitch;
}
