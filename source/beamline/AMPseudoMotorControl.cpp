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
	calibrationInProgress_ = false;

	startedMapper_ = new QSignalMapper(this);
	connect( startedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveStarted(QObject*)) );

	cancelledMapper_ = new QSignalMapper(this);
	connect( cancelledMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveCancelled(QObject*)) );

	failedMapper_ = new QSignalMapper(this);
	connect( failedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveFailed(QObject*)) );

	succeededMapper_ = new QSignalMapper(this);
	connect( succeededMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveSucceeded(QObject*)) );

	calibrationStartedMapper_ = new QSignalMapper(this);
	connect( calibrationStartedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onCalibrationStarted()) );

	calibrationCancelledMapper_ = new QSignalMapper(this);
	connect( calibrationCancelledMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onCalibrationCancelled(QObject*)) );

	calibrationFailedMapper_ = new QSignalMapper(this);
	connect( calibrationFailedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onCalibrationFailed(QObject*)) );

	calibrationSucceededMapper_ = new QSignalMapper(this);
	connect( calibrationSucceededMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onCalibrationSucceeded(QObject*)) );

}

AMPseudoMotorControl::~AMPseudoMotorControl()
{

}

bool AMPseudoMotorControl::canStop() const
{
	bool result = false;

	// This control can stop if all controls are valid and
	// all children that can move can also be stopped.

	if (isConnected()) {

		QList<AMControl*> children = childControls();

		if (children.count() > 0) {

			bool childrenValid = true;
			bool childrenStoppable = true;

			for (int i = 0, count = children.count(); i < count && childrenValid && childrenStoppable; i++) { // We want to stop if we come across either a null child or a child that can move but can't be stopped.
				AMControl *child = childControlAt(i);

				if (child) {

					if (child->canMove() && !child->canStop())
						childrenStoppable = false;

				} else {
					childrenValid = false;
				}
			}

			result = childrenValid && childrenStoppable;
		}
	}

	return result;
}

bool AMPseudoMotorControl::validValue(double value) const
{
	bool isValid = false;

	if (value >= minimumValue() && value <= maximumValue())
		isValid = true;

	return isValid;
}

bool AMPseudoMotorControl::validSetpoint(double value) const
{
	return validValue(value);
}

void AMPseudoMotorControl::addChildControl(AMControl *control)
{
	if (control && !children_.contains(control)) {
		children_ << control;

		connect( control, SIGNAL(connected(bool)), this, SLOT(updateStates()) );
		connect( control, SIGNAL(valueChanged(double)), this, SLOT(updateValue()) );
		connect( control, SIGNAL(movingChanged(bool)), this, SLOT(updateMoving()) );

		updateStates();
	}
}

void AMPseudoMotorControl::removeChildControl(AMControl *control)
{
	if (control && children_.contains(control)) {
		disconnect( control, 0, this, 0 );
		children_.removeOne(control);

		updateStates();
	}
}

QString AMPseudoMotorControl::toString() const
{
	// Note this control's name.

	QString controlName = QString("Name: %1").arg(name());

	// Note this control's description.

	QString controlDescription = QString("Description: %1").arg(description());

	// Note this control's value.

	QString controlValue = (canMeasure()) ? QString("Value: %1").arg(value()) : QString("Value: Not measurable");

	// Note this control's minimum.

	QString controlMinimum = QString("Minimum: %1").arg(minimumValue());

	// Note this control's maximum.

	QString controlMaximum = QString("Maximum: %1").arg(maximumValue());

	// Note this control's value options.

	if (isEnum()) {

		QString controlValues = QString("\tEnum options:\n");

		int enumCount = enumNames().count();

		if (enumCount > 0) {

			for (int enumIndex = 0; enumIndex < enumCount; enumIndex++) {
				QString enumName = enumNames().at(enumIndex);

				if (!enumName.isEmpty())
					controlValues.append(QString("\t\t%1 - %2").arg(enumIndex).arg(enumName));
				else
					controlValues.append(QString("\t\t[Empty enum name]"));

				if (enumIndex < enumCount - 1)
					controlValues.append("\n");
			}

		} else {

			controlValues = QString("\t\t[No enum options]");
		}

		controlValue.append(QString("\n%1").arg(controlValues));
	}

	// Note the values of the control's children.

	int childCount = childControlCount();

	if (childCount > 0) {

		QString childValues = QString("\tChild values:\n");

		for (int childIndex = 0; childIndex < childCount; childIndex++) {
			QString childValue;
			AMControl *child = childControlAt(childIndex);

			if (child) {
				if (child->canMeasure())
					childValue = QString("\t\t%1: %2").arg(child->name()).arg(child->value());
				else
					childValue = QString("\t\t%1 cannot be measured.").arg(child->name());
			} else {
				childValue = QString("\t\t[Null child]");
			}

			childValues.append(childValue);

			if (childIndex < childCount - 1)
				childValues.append("\n");
		}

		controlValue.append(QString("\n%1").arg(childValues));
	}

	// Note this control's general connected state.

	QString controlConnected = QString("Connected: %1").arg(isConnected() ? "Yes" : "No");

	// Note the connected state of each of the child controls.

	if (childCount > 0) {

		QString childrenConnected;

		for (int childIndex = 0; childIndex < childCount; childIndex++) {
			QString childConnected;
			AMControl *child = childControlAt(childIndex);

			if (child)
				childConnected = QString("\t%1 connected: %2").arg(child->name()).arg(child->isConnected() ? "Yes" : "No");
			else
				childConnected = QString("\t[Null child]");

			childrenConnected.append(childConnected);

			if (childIndex < childCount - 1)
				childrenConnected.append("\n");
		}

		controlConnected.append(QString("\n%1").arg(childrenConnected));
	}


	// Note this control's moving state.

	QString controlMoving = QString("Moving: %1").arg(isMoving() ? "Yes" : "No");

	// Note the moving state of each of the child controls.

	QString childrenMoving;

	if (childCount > 0) {

		for (int childIndex = 0; childIndex < childCount; childIndex++) {
			QString childMoving;
			AMControl *child = childControlAt(childIndex);

			if (child)
				childMoving = QString("\t%1 moving: %2").arg(child->name()).arg(child->isMoving() ? "Yes" : "No");
			else
				childMoving = QString("\t[Null child]");

			childrenMoving.append(childMoving);

			if (childIndex < childCount - 1)
				childrenMoving.append("\n");
		}

	} else {

		childrenMoving = QString("\t[No children]");
	}

	controlMoving.append(QString("\n%1").arg(childrenMoving));

	// Note this control's calibrating state.

	QString controlCalibrating = QString("Calibrating: %1").arg(calibrationInProgress() ? "Yes" : "No");

	// Create and return complete info string.

	QString result = QString("%1\n%2\n%3\n%4\n%5\n%6\n%7\n%8").arg(controlName).arg(controlDescription).arg(controlValue).arg(controlMinimum).arg(controlMaximum).arg(controlConnected).arg(controlMoving).arg(controlCalibrating);

	return result;
}

AMControl::FailureExplanation AMPseudoMotorControl::move(double setpoint)
{
	// Check that this control is connected and able to move before proceeding.

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

	// If the new setpoint is within tolerance, no need to proceed with move.
	// Instead report a successful move to setpoint.

	if (withinTolerance(setpoint)) {
		onMoveStarted(0);
		onMoveSucceeded(0);
		return AMControl::NoFailure;
	}

	// Otherwise, an actual move is needed.
	// Create new move action.

	AMAction3 *moveAction = createMoveAction(setpoint_);

	// Check that a valid move action was generated.
	// If an invalid move action was created, abort the move.

	if (!moveAction) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_INVALID_MOVE_ACTION, QString("Did not move %1: invalid move action generated.").arg(name()));
		onMoveStarted(0);
		onMoveFailed(0);
		return AMControl::LimitFailure;
	}

	// Otherwise, proceed with initializing and running the move action.
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

	return AMControl::NoFailure;
}

bool AMPseudoMotorControl::stop()
{
	bool result = false;

	if (canStop()) {

		bool childrenStopped = true;

		// Iterate through all child controls, attempting
		// to stop them.

		foreach (AMControl *control, children_) {
			if (control)
				childrenStopped &= control->stop();
		}

		result = childrenStopped;
	}

	return result;
}

AMControl::FailureExplanation AMPseudoMotorControl::calibrate(double oldValue, double newValue)
{
	// Check that this control is connected and able to be calibrated before proceeding.

	if (!isConnected()) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_NOT_CONNECTED, QString("Failed to calibrate %1: control is not connected.").arg(name()));
		return AMControl::NotConnectedFailure;
	}

	if (!canCalibrate()) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_CANNOT_MOVE, QString("Failed to calibrate %1: control cannot move.").arg(name()));
		return AMControl::OtherFailure;
	}

	if (calibrationInProgress()) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_ALREADY_CALIBRATING, QString("Failed to calibrate %1: control is already being calibrated.").arg(name()));
		return AMControl::AlreadyMovingFailure;
	}

	if (!validValue(oldValue)) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_INVALID_VALUE, QString("Failed to calibrate %1: provided starting value is invalid.").arg(name()));
		return AMControl::LimitFailure;
	}

	if (!validValue(newValue)) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_INVALID_VALUE, QString("Failed to calibrate %1: provided desired value is invalid.").arg(name()));
		return AMControl::LimitFailure;
	}

	// Proceed with creating calibration action.

	AMAction3 *calibrationAction = createCalibrateAction(oldValue, newValue);

	// Check that a valid calibration action was generated.
	// If an invalid calibration action was generated, abort the calibration.

	if (!calibrationAction) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_INVALID_CALIBRATION_ACTION, QString("Did not calibrate %1: invalid calibration action generated.").arg(name()));
		onCalibrationStarted();
		onCalibrationFailed();
		return AMControl::LimitFailure;
	}

	// Otherwise, proceed with initializing and running the calibration action.
	// Create calibration action signal mappings.

	calibrationStartedMapper_->setMapping(calibrationAction, calibrationAction);
	connect( calibrationAction, SIGNAL(started()), calibrationStartedMapper_, SLOT(map()) );

	calibrationCancelledMapper_->setMapping(calibrationAction, calibrationAction);
	connect( calibrationAction, SIGNAL(cancelled()), calibrationCancelledMapper_, SLOT(map()) );

	calibrationFailedMapper_->setMapping(calibrationAction, calibrationAction);
	connect( calibrationAction, SIGNAL(failed()), calibrationFailedMapper_, SLOT(map()) );

	calibrationSucceededMapper_->setMapping(calibrationAction, calibrationAction);
	connect( calibrationAction, SIGNAL(succeeded()), calibrationSucceededMapper_, SLOT(map()) );

	// Run action.

	calibrationAction->start();

	return AMControl::NoFailure;
}

void AMPseudoMotorControl::setEnumStates(const QStringList &enumStateNames)
{
	AMControl::setEnumStates(enumStateNames);

	if (!enumStateNames.isEmpty()) {
		connect( this, SIGNAL(valueChanged(double)), this, SIGNAL(enumChanged()) );
		connect( this, SIGNAL(connected(bool)), this, SIGNAL(enumChanged()) );
		connect( this, SIGNAL(setpointChanged(double)), this, SIGNAL(enumChanged()) );

	} else {
		disconnect( this, SIGNAL(valueChanged(double)), this, SIGNAL(enumChanged()) );
		disconnect( this, SIGNAL(connected(bool)), this, SIGNAL(enumChanged()) );
		disconnect( this, SIGNAL(setpointChanged(double)), this, SIGNAL(enumChanged()) );
	}
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

void AMPseudoMotorControl::setCalibrationInProgress(bool isCalibrating)
{
	if (calibrationInProgress_ != isCalibrating) {
		calibrationInProgress_ = isCalibrating;
		emit calibratingChanged(calibrationInProgress_);
	}
}

void AMPseudoMotorControl::updateStates()
{
	updateConnected();
	updateValue();
	updateMoving();
	updateMinimumValue();
	updateMaximumValue();
}

void AMPseudoMotorControl::onMoveStarted(QObject *action)
{
	Q_UNUSED(action)
	setMoveInProgress(true);
	emit moveStarted();
}

void AMPseudoMotorControl::onMoveCancelled(QObject *action)
{
	moveActionCleanup(action);
	emit moveFailed(AMControl::WasStoppedFailure);
}

void AMPseudoMotorControl::onMoveFailed(QObject *action)
{
	moveActionCleanup(action);
	emit moveFailed(AMControl::OtherFailure);
}

void AMPseudoMotorControl::onMoveSucceeded(QObject *action)
{
	moveActionCleanup(action);
	emit moveSucceeded();
}

void AMPseudoMotorControl::onCalibrationStarted()
{
	setCalibrationInProgress(true);
	emit calibrationStarted();
}

void AMPseudoMotorControl::onCalibrationCancelled(QObject *action)
{
	calibrationActionCleanup(action);
	onCalibrationCancelled();
}

void AMPseudoMotorControl::onCalibrationCancelled()
{
	emit calibrationFailed(AMControl::WasStoppedFailure);
}

void AMPseudoMotorControl::onCalibrationFailed(QObject *action)
{
	calibrationActionCleanup(action);
	onCalibrationFailed();
}

void AMPseudoMotorControl::onCalibrationFailed()
{
	emit calibrationFailed(AMControl::OtherFailure);
}

void AMPseudoMotorControl::onCalibrationSucceeded(QObject *action)
{
	calibrationActionCleanup(action);
	onCalibrationSucceeded();
}

void AMPseudoMotorControl::onCalibrationSucceeded()
{
	emit calibrationSucceeded();
}

AMAction3* AMPseudoMotorControl::createCalibrateAction(double oldValue, double newValue)
{
	Q_UNUSED(oldValue)
	Q_UNUSED(newValue)

	return 0;
}

void AMPseudoMotorControl::moveActionCleanup(QObject *action)
{
	setMoveInProgress(false);
	updateStates();

	if (action) {
		startedMapper_->removeMappings(action);
		cancelledMapper_->removeMappings(action);
		failedMapper_->removeMappings(action);
		succeededMapper_->removeMappings(action);

		actionCleanup(action);
	}
}

void AMPseudoMotorControl::calibrationActionCleanup(QObject *action)
{
	setCalibrationInProgress(false);
	updateStates();

	if (action) {
		calibrationStartedMapper_->removeMappings(action);
		calibrationCancelledMapper_->removeMappings(action);
		calibrationFailedMapper_->removeMappings(action);
		calibrationSucceededMapper_->removeMappings(action);

		actionCleanup(action);
	}
}

void AMPseudoMotorControl::actionCleanup(QObject *action)
{
	if (action) {
		action->disconnect();
		action->deleteLater();
	}
}
