#include "CLSJJSlitBladesControl.h"

#include "util/AMErrorMonitor.h"

CLSJJSlitBladesControl::CLSJJSlitBladesControl(const QString &name, AMControl *upperBladeControl, AMControl *lowerBladeControl, QObject *parent, const QString &units, const QString &description, double tolerance) :
	AMCompositeControl(name, units, parent, description)
{
	// Initialize member variables.

	connected_ = false;
	value_ = 0.0;
	setpoint_ = 0.0;
	moveInProgress_ = false;

	gap_ = 0.0;
	centerPosition_ = 0.0;

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

	// Current settings.

	setUpperBladeControl(upperBladeControl);
	setLowerBladeControl(lowerBladeControl);
}

CLSJJSlitBladesControl::~CLSJJSlitBladesControl()
{

}

bool CLSJJSlitBladesControl::canMove() const
{
	bool result = (!isMoving() && isConnected() && upperBladeControl_->canMove() && lowerBladeControl_->canMove());

	return result;
}

bool CLSJJSlitBladesControl::canStop() const
{
	bool result = (isConnected() && upperBladeControl_->canStop() && lowerBladeControl_->canStop());

	return result;
}

AMControl::FailureExplanation CLSJJSlitBladesControl::move(double setpoint)
{
	if (!isConnected()) {
		AMErrorMon::error(this, CLSJJSLITSCONTROL_NOT_CONNECTED, "JJ slits not connected. Cannot complete move as requested.");
		return AMControl::NotConnectedFailure;
	}

	if (isMoving()) {
		AMErrorMon::error(this, CLSJJSLITSCONTROL_ALREADY_MOVING, "JJ slits already moving. Cannot start a new move until old one is complete.");
		return AMControl::AlreadyMovingFailure;
	}

	if (!canMove()) {
		AMErrorMon::error(this, CLSJJSLITSCONTROL_CANNOT_MOVE, "JJ slits cannot move. Child controls may be set improperly or unable to move themselves.");
		return AMControl::LimitFailure;
	}

	// Identify the current gap and center position.

	updateGap();
	updateCenterPosition();

	// Update the saved setpoint value.

	setSetpoint(setpoint);

	// Create move action.

	AMAction3 *moveAction = createMoveAction(setpoint);

	if (!moveAction) {
		AMErrorMon::error(this, CLSJJSLITSCONTROL_INVALID_ACTION, "JJ slits cannot move. An invalid move action was generated.");
		return AMControl::LimitFailure;
	}

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

bool CLSJJSlitBladesControl::stop()
{
	bool result = false;

	if (!canStop()) {
		AMErrorMon::error(this, CLSJJSLITSCONTROL_CANNOT_STOP, "JJ slits cannot stop.");
		result = false;

	} else {

		bool upperStop = upperBladeControl_->stop();
		bool lowerStop = lowerBladeControl_->stop();

		result = (upperStop && lowerStop);
	}

	return result;
}

void CLSJJSlitBladesControl::setUpperBladeControl(AMControl *newControl)
{
	if (upperBladeControl_ != newControl) {

		if (upperBladeControl_) {
			disconnect( upperBladeControl_, 0, this, 0 );
			children_.removeOne(upperBladeControl_);
		}

		upperBladeControl_ = newControl;

		if (upperBladeControl_) {
			addChildControl(upperBladeControl_);

			connect( upperBladeControl_, SIGNAL(valueChanged(double)), this, SLOT(updateControlStates()) );
			connect( upperBladeControl_, SIGNAL(connected(bool)), this, SLOT(updateControlStates()) );
			connect( upperBladeControl_, SIGNAL(alarmChanged(int,int)), this, SIGNAL(alarmChanged(int,int)) );
			connect( upperBladeControl_, SIGNAL(error(int)), this, SIGNAL(error(int)) );
		}

		updateControlStates();

		emit upperBladeControlChanged(upperBladeControl_);
	}
}

void CLSJJSlitBladesControl::setLowerBladeControl(AMControl *newControl)
{
	if (lowerBladeControl_ != newControl) {

		if (lowerBladeControl_) {
			disconnect( lowerBladeControl_, 0, this, 0 );
			children_.removeOne(lowerBladeControl_);
		}

		lowerBladeControl_ = newControl;

		if (lowerBladeControl_) {
			addChildControl(lowerBladeControl_);

			connect( lowerBladeControl_, SIGNAL(valueChanged(double)), this, SLOT(updateControlStates()) );
			connect( lowerBladeControl_, SIGNAL(connected(bool)), this, SLOT(updateControlStates()) );
			connect( lowerBladeControl_, SIGNAL(alarmChanged(int,int)), this, SIGNAL(alarmChanged(int,int)) );
			connect( lowerBladeControl_, SIGNAL(error(int)), this, SIGNAL(error(int)) );
		}

		updateControlStates();

		emit lowerBladeControlChanged(lowerBladeControl_);
	}
}

void CLSJJSlitBladesControl::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connected(connected_);
	}
}

void CLSJJSlitBladesControl::setValue(double newValue)
{
	if (value_ != newValue) {
		value_ = newValue;
		emit valueChanged(value_);
	}
}

void CLSJJSlitBladesControl::setSetpoint(double newValue)
{
	if (setpoint_ != newValue) {
		setpoint_ = newValue;
		emit setpointChanged(setpoint_);
	}
}

void CLSJJSlitBladesControl::setMoveInProgress(bool isMoving)
{
	if (moveInProgress_ != isMoving) {
		moveInProgress_ = isMoving;
		emit movingChanged(moveInProgress_);
	}
}

void CLSJJSlitBladesControl::setGap(double newValue)
{
	if (gap_ != newValue) {
		gap_ = newValue;
		emit gapChanged(gap_);
	}
}

void CLSJJSlitBladesControl::setCenterPosition(double newValue)
{
	if (centerPosition_ != newValue) {
		centerPosition_ = newValue;
		emit centerPositionChanged(centerPosition_);
	}
}

void CLSJJSlitBladesControl::updateControlStates()
{
	updateConnected();
	updateGap();
	updateCenterPosition();
	updateValue();
}

void CLSJJSlitBladesControl::updateConnected()
{
	bool upperControlOK = (upperBladeControl_ && upperBladeControl_->isConnected());
	bool lowerControlOK = (lowerBladeControl_ && lowerBladeControl_->isConnected());

	bool connected = (upperControlOK && lowerControlOK);

	setConnected(connected);
}

void CLSJJSlitBladesControl::updateGap()
{
	if (isConnected()) {
		setGap( calculateGap(upperBladeControl_->value(), lowerBladeControl_->value()) );
	}
}

void CLSJJSlitBladesControl::updateCenterPosition()
{
	if (isConnected()) {
		setCenterPosition( calculateCenterPosition(upperBladeControl_->value(), lowerBladeControl_->value()) );
	}
}

void CLSJJSlitBladesControl::onMoveStarted()
{
	setMoveInProgress(true);
	emit moveStarted();
}

void CLSJJSlitBladesControl::onMoveCancelled(QObject *action)
{
	moveCleanup(action);
	emit moveFailed(AMControl::WasStoppedFailure);
}

void CLSJJSlitBladesControl::onMoveFailed(QObject *action)
{
	moveCleanup(action);
	emit moveFailed(AMControl::OtherFailure);
}

void CLSJJSlitBladesControl::onMoveSucceeded(QObject *action)
{
	moveCleanup(action);
	emit moveSucceeded();
}

void CLSJJSlitBladesControl::moveCleanup(QObject *action)
{
	if (action) {
		setMoveInProgress(false);

		action->disconnect();

		moveCancelled_->removeMappings(action);
		moveFailed_->removeMappings(action);
		moveSucceeded_->removeMappings(action);

		action->deleteLater();
	}
}

double CLSJJSlitBladesControl::calculateLowerPosition(double gap, double center)
{
	return center + (gap / 2.0);
}

double CLSJJSlitBladesControl::calculateUpperPosition(double gap, double center)
{
	return center - (gap / 2.0);
}

double CLSJJSlitBladesControl::calculateGap(double upperBladePosition, double lowerBladePosition)
{
	// Upper blade opens with a negative setpoint; lower blade opens with positive.
	return (lowerBladePosition - upperBladePosition);
}

double CLSJJSlitBladesControl::calculateCenterPosition(double upperBladePosition, double lowerBladePosition)
{
	return ((upperBladePosition - lowerBladePosition) / 2.0 + lowerBladePosition);
}



