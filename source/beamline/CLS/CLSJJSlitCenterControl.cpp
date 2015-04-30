#include "CLSJJSlitCenterControl.h"

#include "util/AMErrorMonitor.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

#include <QDebug>

CLSJJSlitCenterControl::CLSJJSlitCenterControl(AMControl *upperBladeControl, AMControl *lowerBladeControl, QObject *parent, double tolerance) :
	AMCompositeControl("JJSlitsCenterControl", "mm", parent, "JJ Slits Center Control")
{
	// Initialize member variables.

	value_ = 0.0;
	setpoint_ = 0.0;
	gap_ = 2.0;
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
	setTolerance(tolerance);
	setContextKnownDescription("JJ Slits Center Control");

	// Current settings.

	setUpperBladeControl(upperBladeControl);
	setLowerBladeControl(lowerBladeControl);
}

CLSJJSlitCenterControl::~CLSJJSlitCenterControl()
{

}

bool CLSJJSlitCenterControl::isConnected() const
{
	bool result = (upperBladeControl_ && lowerBladeControl_ && AMCompositeControl::isConnected());

	return result;
}

bool CLSJJSlitCenterControl::canMove() const
{
	bool result = (!isMoving() && isConnected() && upperBladeControl_->canMove() && lowerBladeControl_->canMove());

	return result;
}

bool CLSJJSlitCenterControl::canStop() const
{
	bool result = (isConnected() && upperBladeControl_->canStop() && lowerBladeControl_->canStop());

	return result;
}

double CLSJJSlitCenterControl::minimumValue() const
{
	return -30;
}

double CLSJJSlitCenterControl::maximumValue() const
{
	return 30;
}

AMControl::FailureExplanation CLSJJSlitCenterControl::move(double setpoint)
{
	if (!isConnected()) {
		AMErrorMon::error(this, CLSJJSLITCENTERCONTROL_NOT_CONNECTED, "JJ slits center control not connected. Cannot complete move as requested.");
		return AMControl::NotConnectedFailure;
	}

	if (isMoving()) {
		AMErrorMon::error(this, CLSJJSLITCENTERCONTROL_ALREADY_MOVING, "JJ slits center control already moving. Cannot start a new move until old one is complete.");
		return AMControl::AlreadyMovingFailure;
	}

	if (!canMove()) {
		AMErrorMon::error(this, CLSJJSLITCENTERCONTROL_CANNOT_MOVE, "JJ slits center control cannot move. Child controls may be set improperly or unable to move themselves.");
		return AMControl::LimitFailure;
	}

	// Update the saved setpoint value.

	setSetpoint(setpoint);

	// Create move action.

	AMListAction3 *moveAction = new AMListAction3(new AMListActionInfo3("JJSlitsCenterControlMove", "JJSlitsCenterControlMove"), AMListAction3::Parallel);

	double lowerPosition = calculateLowerPosition(gap_, setpoint_);
	double upperPosition = calculateUpperPosition(gap_, setpoint_);

	qDebug() << "\nNew lower: " << lowerPosition;
	qDebug() << "New upper: " << upperPosition << "\n";

	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(upperBladeControl_, upperPosition));
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(lowerBladeControl_, lowerPosition));

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

bool CLSJJSlitCenterControl::stop()
{
	bool result;

	if (!canStop()) {
		AMErrorMon::error(this, CLSJJSLITCENTERCONTROL_CANNOT_STOP, "JJ slit center control cannot stop.");
		result = false;

	} else {

		bool upperStop = upperBladeControl_->stop();
		bool lowerStop = lowerBladeControl_->stop();

		result = (upperStop && lowerStop);
	}

	return result;
}

void CLSJJSlitCenterControl::setGap(double newValue)
{
	if (gap_ != newValue) {
		gap_ = newValue;
		emit gapChanged(gap_);
	}
}

void CLSJJSlitCenterControl::setUpperBladeControl(AMControl *newControl)
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

		updateValue();

		emit upperBladeControlChanged(upperBladeControl_);
	}
}

void CLSJJSlitCenterControl::setLowerBladeControl(AMControl *newControl)
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

		updateValue();

		emit lowerBladeControlChanged(lowerBladeControl_);
	}
}

void CLSJJSlitCenterControl::setValue(double newValue)
{
	if (value_ != newValue) {
		value_ = newValue;
		emit valueChanged(value_);
	}
}

void CLSJJSlitCenterControl::setSetpoint(double newValue)
{
	if (setpoint_ != newValue) {
		setpoint_ = newValue;
		emit setpointChanged(setpoint_);
	}
}

void CLSJJSlitCenterControl::setMoveInProgress(bool isMoving)
{
	if (moveInProgress_ != isMoving) {
		moveInProgress_ = isMoving;
		emit movingChanged(moveInProgress_);
	}
}

void CLSJJSlitCenterControl::updateValue()
{
	if (isConnected()) {
		setValue(calculateCenterPosition(upperBladeControl_->value(), lowerBladeControl_->value()));
	}
}

void CLSJJSlitCenterControl::onMoveStarted()
{
	setMoveInProgress(true);
	emit moveStarted();
}

void CLSJJSlitCenterControl::onMoveCancelled(QObject *action)
{
	moveCleanup(action);
	emit moveFailed(AMControl::WasStoppedFailure);
}

void CLSJJSlitCenterControl::onMoveFailed(QObject *action)
{
	moveCleanup(action);
	emit moveFailed(AMControl::OtherFailure);
}

void CLSJJSlitCenterControl::onMoveSucceeded(QObject *action)
{
	moveCleanup(action);
	emit moveSucceeded();
}

void CLSJJSlitCenterControl::moveCleanup(QObject *action)
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

double CLSJJSlitCenterControl::calculateLowerPosition(double gap, double center)
{
	return center + (gap / 2.0);
}

double CLSJJSlitCenterControl::calculateUpperPosition(double gap, double center)
{
	return center - (gap / 2.0);
}

double CLSJJSlitCenterControl::calculateCenterPosition(double upperBladePosition, double lowerBladePosition)
{
	return ((upperBladePosition - lowerBladePosition) / 2.0 + lowerBladePosition);
}


