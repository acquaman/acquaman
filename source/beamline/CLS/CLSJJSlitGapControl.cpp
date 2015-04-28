#include "CLSJJSlitGapControl.h"

#include "beamline/CLS/CLSMAXvMotor.h"

CLSJJSlitGapControl::CLSJJSlitGapControl(AMControl *firstMotorControl, AMControl *secondMotorControl, QObject *parent) :
	AMCompositeControl("JJSlitsGapControl", "mm", parent, "JJ Slits Gap Control")
{
	/// Initialize member variables.

	value_ = 0.0;
	setpoint_ = 0.0;
	moveInProgress_ = false;

	firstMotor_ = 0;
	secondMotor_ = 0;

	// Initialize inherited variables.

	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("JJ Gap Control");

	// Current settings.

	setFirstMotorControl(firstMotorControl);
	setSecondMotorControl(secondMotorControl);
}

CLSJJSlitGapControl::~CLSJJSlitGapControl()
{

}

bool CLSJJSlitGapControl::canMove() const
{
	bool firstMotorOK = (firstMotor_ && firstMotor_->canMove());
	bool secondMotorOK = (secondMotor_ && secondMotor_->canMove());

	return (firstMotorOK && secondMotorOK && isConnected());
}

bool CLSJJSlitGapControl::canStop() const
{
	bool firstMotorOK = (firstMotor_ && firstMotor_->canStop());
	bool secondMotorOK = (secondMotor_ && secondMotor_->canStop());

	return (firstMotorOK && secondMotorOK && isConnected());
}

AMControl::FailureExplanation CLSJJSlitGapControl::move(double setpoint)
{
	Q_UNUSED(setpoint)

	return AMControl::LimitFailure;
}

bool CLSJJSlitGapControl::stop()
{
	bool result = false;

	if (!canStop()) {
		// error mon.

	} else {

		bool firstStop = firstMotor_->stop();
		bool secondStop = secondMotor_->stop();

		result = (firstStop && secondStop);
	}

	return result;
}

void CLSJJSlitGapControl::setFirstMotorControl(AMControl *newControl)
{
	if (firstMotor_ != newControl) {

		if (firstMotor_) {
			children_.removeOne(firstMotor_);
		}

		firstMotor_ = newControl;

		if (firstMotor_) {
			addChildControl(firstMotor_);
		}

		emit firstMotorChanged(firstMotor_);
	}
}

void CLSJJSlitGapControl::setSecondMotorControl(AMControl *newControl)
{
	if (secondMotor_ != newControl) {

		if (secondMotor_) {
			children_.removeOne(secondMotor_);
		}

		secondMotor_ = newControl;

		if (secondMotor_) {
			addChildControl(secondMotor_);
		}

		emit secondMotorChanged(secondMotor_);
	}
}

void CLSJJSlitGapControl::setValue(double newValue)
{
	if (value_ != newValue) {
		value_ = newValue;
		emit valueChanged(value_);
	}
}

void CLSJJSlitGapControl::setSetpoint(double newValue)
{
	if (setpoint_ != newValue) {
		setpoint_ = newValue;
		emit setpointChanged(setpoint_);
	}
}

void CLSJJSlitGapControl::setMoveInProgress(bool isMoving)
{
	if (moveInProgress_ != isMoving) {
		moveInProgress_ = isMoving;
		emit movingChanged(moveInProgress_);

		if (moveInProgress_)
			emit moveStarted();
	}
}
