#include "CLSJJSlitGapControl.h"

#include "util/AMErrorMonitor.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

#include <QDebug>

CLSJJSlitGapControl::CLSJJSlitGapControl(AMControl *upperBladeControl, AMControl *lowerBladeControl, QObject *parent) :
	AMCompositeControl("JJSlitsGapControl", "mm", parent, "JJ Slits Gap Control")
{
	// Initialize member variables.

	value_ = 0.0;
	setpoint_ = 0.0;
	centerPosition_ = 0.0;
	moveInProgress_ = false;

	upperBladeControl_ = 0;
	lowerBladeControl_ = 0;

	moveCancelled_ = new QSignalMapper(this);
	connect( moveCancelled_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveCancelled(QObject*)) );

	moveFailed_ = new QSignalMapper(this);
	connect( moveFailed_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveFailed(QObject*)) );

	moveSucceeded_ = new QSignalMapper(this);
	connect( moveSucceeded_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveSucceeded(QObject*)) );

	// Initialize inherited variables.

	setAllowsMovesWhileMoving(false);
	setDisplayPrecision(2);
	setTolerance(0.1);
	setContextKnownDescription("JJ Gap Control");

	// Current settings.

	setUpperBladeControl(upperBladeControl);
	setLowerBladeControl(lowerBladeControl);
}

CLSJJSlitGapControl::~CLSJJSlitGapControl()
{

}

bool CLSJJSlitGapControl::isConnected() const
{
	bool result = (upperBladeControl_ && lowerBladeControl_ && AMCompositeControl::isConnected());

	return result;
}

bool CLSJJSlitGapControl::canMove() const
{
	bool result = (!isMoving() && isConnected() && upperBladeControl_->canMove() && lowerBladeControl_->canMove());

	return result;
}

bool CLSJJSlitGapControl::canStop() const
{
	bool result = (isConnected() && upperBladeControl_->canStop() && lowerBladeControl_->canStop());

	return result;
}

double CLSJJSlitGapControl::minimumValue() const
{
	return -30;
}

double CLSJJSlitGapControl::maximumValue() const
{
	return 30;
}

AMControl::FailureExplanation CLSJJSlitGapControl::move(double setpoint)
{
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

	double offset = calculatePositionOffset(setpoint_, centerPosition_);

	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(upperBladeControl_, -offset));
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(lowerBladeControl_, offset));

	// Create signal mappings for this action.

	connect( moveAction, SIGNAL(started()), this, SLOT(onMoveStarted()) );

	moveCancelled_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(cancelled()), moveCancelled_, SLOT(map()) );

	moveFailed_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(failed()), moveFailed_, SLOT(map()) );

	moveSucceeded_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(succeeded()), moveSucceeded_, SLOT(map()) );

	// Run action.

	moveAction->start();

	return AMControl::NoFailure;
}

bool CLSJJSlitGapControl::stop()
{
	bool result;

	if (!canStop()) {
		AMErrorMon::error(this, CLSJJSLITGAPCONTROL_CANNOT_STOP, "JJ slit gap control cannot stop.");

		result = false;

	} else {

		bool upperStop = upperBladeControl_->stop();
		bool lowerStop = lowerBladeControl_->stop();

		result = (upperStop && lowerStop);
	}

	return result;
}

void CLSJJSlitGapControl::setCenterPosition(double newValue)
{
	if (centerPosition_ != newValue) {
		centerPosition_ = newValue;
		emit centerPositionChanged(centerPosition_);
	}
}

void CLSJJSlitGapControl::setUpperBladeControl(AMControl *newControl)
{
	if (upperBladeControl_ != newControl) {

		if (upperBladeControl_) {
			disconnect( upperBladeControl_, 0, this, 0 );
			children_.removeOne(upperBladeControl_);
		}

		upperBladeControl_ = newControl;

		if (upperBladeControl_) {
			addChildControl(upperBladeControl_);

			connect( upperBladeControl_, SIGNAL(valueChanged(double)), this, SLOT(updateValue()) );
			connect( upperBladeControl_, SIGNAL(connected(bool)), this, SLOT(updateValue()) );
			connect( upperBladeControl_, SIGNAL(alarmChanged(int,int)), this, SIGNAL(alarmChanged(int,int)) );
			connect( upperBladeControl_, SIGNAL(error(int)), this, SIGNAL(error(int)) );
		}

		emit upperBladeControlChanged(upperBladeControl_);
	}
}

void CLSJJSlitGapControl::setLowerBladeControl(AMControl *newControl)
{
	if (lowerBladeControl_ != newControl) {

		if (lowerBladeControl_) {
			disconnect( lowerBladeControl_, 0, this, 0 );
			children_.removeOne(lowerBladeControl_);
		}

		lowerBladeControl_ = newControl;

		if (lowerBladeControl_) {
			addChildControl(lowerBladeControl_);

			connect( lowerBladeControl_, SIGNAL(valueChanged(double)), this, SLOT(updateValue()) );
			connect( lowerBladeControl_, SIGNAL(connected(bool)), this, SLOT(updateValue()) );
			connect( lowerBladeControl_, SIGNAL(alarmChanged(int,int)), this, SIGNAL(alarmChanged(int,int)) );
			connect( lowerBladeControl_, SIGNAL(error(int)), this, SIGNAL(error(int)) );
		}

		emit lowerBladeControlChanged(lowerBladeControl_);
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
	}
}

void CLSJJSlitGapControl::updateValue()
{
	if (isConnected()) {
		double gap = calculateGap(upperBladeControl_->value(), lowerBladeControl_->value());
		setValue(gap);
	}
}

void CLSJJSlitGapControl::onMoveStarted()
{
	setMoveInProgress(true);
	emit moveStarted();
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

double CLSJJSlitGapControl::calculatePositionOffset(double gap, double center)
{
	return (gap/2) + center;
}

double CLSJJSlitGapControl::calculateGap(double upperBladePosition, double lowerBladePosition)
{
	// Upper blade opens with a negative setpoint; lower blade opens with positive.
	return (lowerBladePosition - upperBladePosition);
}
