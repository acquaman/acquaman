#include "BioXASMirrorPseudoMotorControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"
#include <math.h>

BioXASMirrorMotorControl::BioXASMirrorMotorControl(const QString &name, const QString &units, double upstreamLength, double downstreamLength, QObject *parent, const QString &description) :
	BioXASMirrorControl(name, units, upstreamLength, downstreamLength, parent, description)
{
	// Initialize member variables.

	upstreamInboard_ = 0;
	upstreamOutboard_ = 0;
	downstream_ = 0;
}

BioXASMirrorMotorControl::~BioXASMirrorMotorControl()
{

}

bool BioXASMirrorMotorControl::canMeasure() const
{
	bool result = false;

	if (isConnected()) {
		result = ( upstreamInboard_->canMeasure() && upstreamOutboard_->canMeasure() && downstream_->canMeasure() );
	}

	return result;
}

bool BioXASMirrorMotorControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = ( upstreamInboard_->canMove() && upstreamOutboard_->canMove() && downstream_->canMove() );

	return result;
}

bool BioXASMirrorMotorControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = ( upstreamInboard_->canStop() && upstreamOutboard_->canStop() && downstream_->canStop() );

	return result;
}

bool BioXASMirrorMotorControl::validValue(double value) const
{
	bool result = false;

	if (value < maximumValue() && value > minimumValue()) {
		result = true;
	}

	return true;
}

bool BioXASMirrorMotorControl::validSetpoint(double value) const
{
	return validValue(value);
}

void BioXASMirrorMotorControl::setUpstreamInboardControl(BioXASMirrorMotor *newControl)
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

void BioXASMirrorMotorControl::setUpstreamOutboardControl(BioXASMirrorMotor *newControl)
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

void BioXASMirrorMotorControl::setDownstreamControl(BioXASMirrorMotor *newControl)
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

void BioXASMirrorMotorControl::updateConnected()
{
	bool isConnected = (
				upstreamInboard_ && upstreamInboard_->isConnected() &&
				upstreamOutboard_ && upstreamOutboard_->isConnected() &&
				downstream_ && downstream_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASMirrorMotorControl::updateMoving()
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


