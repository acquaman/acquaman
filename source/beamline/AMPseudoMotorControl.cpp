#include "AMPseudoMotorControl.h"
#include "util/AMErrorMonitor.h"

AMPseudoMotorControl::AMPseudoMotorControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	AMControl(name, units, parent, description)
{
	// Initialize local variables.

	connected_ = false;
	value_ = 0;
	setpoint_ = 0;
	moveInProgress_ = false;
	isMoving_ = false;
	minimumValue_ = 0;
	maximumValue_ = 0;

	startedMapper_ = new QSignalMapper(this);
	connect( startedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveStarted(QObject*)) );

	cancelledMapper_ = new QSignalMapper(this);
	connect( cancelledMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveCancelled(QObject*)) );

	failedMapper_ = new QSignalMapper(this);
	connect( failedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveFailed(QObject*)) );

	succeededMapper_ = new QSignalMapper(this);
	connect( succeededMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveSucceeded(QObject*)) );
}

AMPseudoMotorControl::~AMPseudoMotorControl()
{

}

void AMPseudoMotorControl::addChildControl(AMControl *control)
{
	if (control) {
		children_ << control;

		connect( control, SIGNAL(connected(bool)), this, SLOT(updateStates()) );
		connect( control, SIGNAL(valueChanged(double)), this, SLOT(updateValue()) );
		connect( control, SIGNAL(movingChanged(bool)), this, SLOT(updateMoving()) );
	}
}

void AMPseudoMotorControl::removeChildControl(AMControl *control)
{
	if (control) {
		disconnect( control, 0, this, 0 );
		children_.removeOne(control);
	}
}

AMControl::FailureExplanation AMPseudoMotorControl::move(double setpoint)
{
	if (!isConnected()) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_NOT_CONNECTED, QString("Failed to move %1: control is not connected.").arg(name()));
		return AMControl::NotConnectedFailure;
	}

	if (!canMove()) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_CANNOT_MOVE, QString("Failed to move %1: control cannot move.").arg(name()));
		return AMControl::OtherFailure;
	}

	if (isMoving() && !allowsMovesWhileMoving()) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_ALREADY_MOVING, QString("Failed to move %1: control is already moving.").arg(name()));
		return AMControl::AlreadyMovingFailure;
	}

	if (!validSetpoint(setpoint)) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_INVALID_SETPOINT, QString("Failed to move %1: provided setpoint is invalid.").arg(name()));
		return AMControl::LimitFailure;
	}

	// Update the setpoint.

	setSetpoint(setpoint);

	// Otherwise, create new move action.

	AMAction3 *moveAction = createMoveAction(setpoint_);

	if (!moveAction) {
		AMErrorMon::information(this, AMPSEUDOMOTORCONTROL_INVALID_MOVE_ACTION, QString("Did not move %1: setpoint within tolerance deadband.").arg(name()));
		onMoveStarted(0);
		onMoveSucceeded(0);
//		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_INVALID_MOVE_ACTION, QString("Failed to move %1: invalid move action generated.").arg(name()));
//		return AMControl::LimitFailure;

	} else {

		// Create move action signal mappings.

		startedMapper_->setMapping(moveAction, moveAction);
		connect( moveAction, SIGNAL(started()), startedMapper_, SLOT(map()) );

		cancelledMapper_->setMapping(moveAction, moveAction);
		connect( moveAction, SIGNAL(cancelled()), cancelledMapper_, SLOT(map()) );

		failedMapper_->setMapping(moveAction, moveAction);
		connect( moveAction, SIGNAL(failed()), failedMapper_, SLOT(map()) );

		succeededMapper_->setMapping(moveAction, moveAction);
		connect( moveAction, SIGNAL(succeeded()), succeededMapper_, SLOT(map()) );

		// Run action.

		moveAction->start();
	}

	return AMControl::NoFailure;
}

bool AMPseudoMotorControl::stop()
{
	bool result = true;

	foreach (AMControl *child, children_) {
		bool childStopped = false;

		if (child && child->shouldStop()) {
			if (child->canStop())
				childStopped = child->stop();
			else
				childStopped = false;
		}

		result = result && childStopped;
	}

	return result;
}

void AMPseudoMotorControl::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connected(connected_);
	}
}

void AMPseudoMotorControl::setValue(double newValue)
{
	if (value_ != newValue) {
		value_ = newValue;
		emit valueChanged(value_);
	}
}

void AMPseudoMotorControl::setSetpoint(double newValue)
{
	if (setpoint_ != newValue) {
		setpoint_ = newValue;
		emit setpointChanged(setpoint_);
	}
}

void AMPseudoMotorControl::setMoveInProgress(bool isMoving)
{
	if (moveInProgress_ != isMoving) {
		moveInProgress_ = isMoving;
		emit movingChanged(moveInProgress_);
	}
}

void AMPseudoMotorControl::setIsMoving(bool isMoving)
{
	if (isMoving_ != isMoving) {
		isMoving_ = isMoving;
		emit movingChanged(isMoving_);
	}
}

void AMPseudoMotorControl::setMinimumValue(double newValue)
{
	if (minimumValue_ != newValue) {
		minimumValue_ = newValue;
		emit minimumValueChanged(minimumValue_);
	}
}

void AMPseudoMotorControl::setMaximumValue(double newValue)
{
	if (maximumValue_ != newValue) {
		maximumValue_ = newValue;
		emit maximumValueChanged(maximumValue_);
	}
}

void AMPseudoMotorControl::updateStates()
{
	updateConnected();
	updateValue();
	updateMoving();
}

void AMPseudoMotorControl::onMoveStarted(QObject *action)
{
	Q_UNUSED(action)
	setMoveInProgress(true);
}

void AMPseudoMotorControl::onMoveCancelled(QObject *action)
{
	moveCleanup(action);
	emit moveFailed(AMControl::WasStoppedFailure);
}

void AMPseudoMotorControl::onMoveFailed(QObject *action)
{
	moveCleanup(action);
	emit moveFailed(AMControl::OtherFailure);
}

void AMPseudoMotorControl::onMoveSucceeded(QObject *action)
{
	moveCleanup(action);
	emit moveSucceeded();
}

void AMPseudoMotorControl::moveCleanup(QObject *action)
{
	setMoveInProgress(false);
	updateMoving();

	if (action) {
		action->disconnect();

		startedMapper_->removeMappings(action);
		cancelledMapper_->removeMappings(action);
		failedMapper_->removeMappings(action);
		succeededMapper_->removeMappings(action);

		action->deleteLater();
	}
}
