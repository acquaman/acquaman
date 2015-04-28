#include "CLSJJSlitGapControl.h"

#include "beamline/CLS/CLSMAXvMotor.h"
#include "util/AMErrorMonitor.h"
#include "actions3/AMListAction3.h"

CLSJJSlitGapControl::CLSJJSlitGapControl(AMControl *firstMotorControl, AMControl *secondMotorControl, QObject *parent) :
	AMCompositeControl("JJSlitsGapControl", "mm", parent, "JJ Slits Gap Control")
{
	/// Initialize member variables.

	value_ = 0.0;
	setpoint_ = 0.0;
	moveInProgress_ = false;

	firstMotor_ = 0;
	secondMotor_ = 0;

	moveCancelled_ = new QSignalMapper(this);
	connect( moveCancelled_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveCancelled(QObject*)) );

	moveFailed_ = new QSignalMapper(this);
	connect( moveFailed_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveFailed(QObject*)) );

	moveSucceeded_ = new QSignalMapper(this);
	connect( moveSucceeded_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveSucceeded(QObject*)) );

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

bool CLSJJSlitGapControl::isConnected() const
{
	bool firstMotorOK = (firstMotor_ && firstMotor_->isConnected());
	bool secondMotorOK = (secondMotor_ && secondMotor_->isConnected());

	return (firstMotorOK && secondMotorOK);
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

	if (!isConnected()) {
		AMErrorMon::error(this, CLSJJSLITGAPCONTROL_NOT_CONNECTED, "JJ slits gap control not connected. Cannot complete move as requested.");
		return AMControl::NotConnectedFailure;
	}

	if (isMoving()) {
		AMErrorMon::error(this, CLSJJSLITGAPCONTROL_ALREADY_MOVING, "JJ slits gap control already moving. Cannot start a new move until old one is complete.");
		return AMControl::AlreadyMovingFailure;
	}

	if (!canMove()) {
		AMErrorMon::error(this, CLSJJSLITGAPCONTROL_CANNOT_MOVE, "JJ slits gap control cannot move. Child controls may be set improperly or unable to move themselves.");
		return AMControl::LimitFailure;
	}

	// Update the saved setpoint value.

	setSetpoint(setpoint);

	// Create move action.

	AMListAction3 *moveAction = new AMListAction3(new AMListActionInfo3("JJSlitsGapControlMove", "JJSlitsGapControlMove"), AMListAction3::Parallel);

	if (!moveAction) {
		AMErrorMon::error(this, CLSJJSLITGAPCONTROL_INVALID_ACTION, "JJ slits gap control cannot move. The move action generated was invalid.");
		return AMControl::LimitFailure;
	}

	// Create signal mappings for this action.

	moveCancelled_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(cancelled()), moveCancelled_, SLOT(map(QObject*)) );

	moveFailed_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(failed()), moveFailed_, SLOT(map(QObject*)) );

	moveSucceeded_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(succeeded()), moveSucceeded_, SLOT(map(QObject*)) );

	// Run action.

	moveAction->start();

	return AMControl::NoFailure;
}

bool CLSJJSlitGapControl::stop()
{
	bool result = false;

	if (!canStop()) {
		AMErrorMon::error(this, CLSJJSLITGAPCONTROL_CANNOT_STOP, "JJ slit gap control cannot stop.");

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

void CLSJJSlitGapControl::updateValue()
{
	if (isConnected()) {
		double firstMotorValue = firstMotor_->value();
		double secondMotorValue = secondMotor_->value();

		// draw conclusions about the current gap value.

		// set new gap.

		setValue(0);
	}
}

void CLSJJSlitGapControl::onMoveCancelled(QObject *action)
{
	moveCleanup(action);
	emit moveFailed(AMControl::WasStoppedFailure);
}

void CLSJJSlitGapControl::onMoveFailed(QObject *action)
{
	moveCleanup(action);
	emit moveFailed(AMControl::OtherFailure);
}

void CLSJJSlitGapControl::onMoveSucceeded(QObject *action)
{
	moveCleanup(action);
	emit moveSucceeded();
}

void CLSJJSlitGapControl::moveCleanup(QObject *action)
{
	if (action) {
		setMoveInProgress(false);

		disconnect( action, 0, moveCancelled_, 0 );
		disconnect( action, 0, moveFailed_, 0 );
		disconnect( action, 0, moveSucceeded_, 0 );

		moveCancelled_->removeMappings(action);
		moveFailed_->removeMappings(action);
		moveSucceeded_->removeMappings(action);

		action->deleteLater();
	}
}
