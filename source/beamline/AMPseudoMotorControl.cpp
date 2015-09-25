#include "AMPseudoMotorControl.h"
#include "util/AMErrorMonitor.h"

AMPseudoMotorControl::AMPseudoMotorControl(const QString &name, const QString &units, QObject *parent, const QString &description, AMDetectorSet *optimizationDetectors) :
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
	optimizationInProgress_ = false;

	detectors_ = 0;

	// Move action mappers.

	startedMapper_ = new QSignalMapper(this);
	connect( startedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveStarted(QObject*)) );

	cancelledMapper_ = new QSignalMapper(this);
	connect( cancelledMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveCancelled(QObject*)) );

	failedMapper_ = new QSignalMapper(this);
	connect( failedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveFailed(QObject*)) );

	succeededMapper_ = new QSignalMapper(this);
	connect( succeededMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveSucceeded(QObject*)) );

	// Calibration action mappers.

	calibrationStartedMapper_ = new QSignalMapper(this);
	connect( calibrationStartedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onCalibrationStarted()) );

	calibrationCancelledMapper_ = new QSignalMapper(this);
	connect( calibrationCancelledMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onCalibrationCancelled(QObject*)) );

	calibrationFailedMapper_ = new QSignalMapper(this);
	connect( calibrationFailedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onCalibrationFailed(QObject*)) );

	calibrationSucceededMapper_ = new QSignalMapper(this);
	connect( calibrationSucceededMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onCalibrationSucceeded(QObject*)) );

	// Optimization action mappers.

	optimizationStartedMapper_ = new QSignalMapper(this);
	connect( optimizationStartedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onOptimizationStarted()) );

	optimizationCancelledMapper_ = new QSignalMapper(this);
	connect( optimizationCancelledMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onOptimizationCancelled(QObject*)) );

	optimizationFailedMapper_ = new QSignalMapper(this);
	connect( optimizationFailedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onOptimizationFailed(QObject*)) );

	optimizationSucceededMapper_ = new QSignalMapper(this);
	connect( optimizationSucceededMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onOptimizationSucceeded(QObject*)) );

	// Current settings.

	setOptimizationDetectors(optimizationDetectors);
}

AMPseudoMotorControl::~AMPseudoMotorControl()
{

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

QString AMPseudoMotorControl::toString() const
{
	// Note this control's name.

	QString controlName = "Name: " + objectName();

	// Note this control's description.

	QString controlDescription = "Description: " + description();

	// Note this control's general connected state.

	QString controlConnected = "Connected: ";
	if (isConnected())
		controlConnected += "Yes";
	else
		controlConnected += "No";

	// Note the connected state of each of the child controls.

	controlConnected += "\n";

	int childCount = childControlCount();

	for (int childIndex = 0; childIndex < childCount; childIndex++) {
		AMControl *child = childControlAt(childIndex);

		if (child) {
			controlConnected += "\t" + child->objectName() + " connected: ";

			if (child->isConnected())
				controlConnected += "Yes";
			else
				controlConnected += "No";
		}

		if (childIndex < childCount - 1)
			controlConnected += "\n";
	}

	// Note this control's moving state.

	QString controlMoving = "Moving: ";
	if (isMoving())
		controlMoving += "Yes";
	else
		controlMoving += "No";

	// Note this control's calibrating state.

	QString controlCalibrating = "Calibrating: ";
	if (calibrationInProgress())
		controlCalibrating += "Yes";
	else
		controlCalibrating += "No";

	// Create and return complete info string.

	QString result = controlName + "\n" + controlDescription + "\n" + controlConnected + "\n" + controlMoving + "\n" + controlCalibrating;

	return result;
}

void AMPseudoMotorControl::setOptimizationDetectors(AMDetectorSet *newDetectors)
{
	if (detectors_ != newDetectors) {
		detectors_ = newDetectors;
		emit optimizationDetectorsChanged(detectors_);
	}
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

AMControl::FailureExplanation AMPseudoMotorControl::calibrate(double oldValue, double newValue)
{
	// Check that this control is connected and able to be calibrated before proceeding.

	if (!isConnected()) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_NOT_CONNECTED, QString("Failed to calibrate %1: control is not connected.").arg(name()));
		return AMControl::NotConnectedFailure;
	}

	if (!canMove()) {
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

AMControl::FailureExplanation AMPseudoMotorControl::optimize()
{
	// Check that this control is connected and able to optimize before proceeding.

	if (!isConnected()) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_NOT_CONNECTED, QString("Failed to calibrate %1: control is not connected.").arg(name()));
		return AMControl::NotConnectedFailure;
	}

	if (!canMove()) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_CANNOT_MOVE, QString("Failed to calibrate %1: control cannot move.").arg(name()));
		return AMControl::OtherFailure;
	}

	if (optimizationInProgress()) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_ALREADY_OPTIMIZING, QString("Failed to optimize %1: control is already being optimized.").arg(name()));
		return AMControl::AlreadyMovingFailure;
	}

	// Proceed with creating optimization action.

	AMAction3 *optimizationAction = createOptimizationAction();

	// Check that a valid optimization action was generated.
	// If an invalid optimization action was generated, abort the optimization.

	if (!optimizationAction) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_INVALID_OPTIMIZATION_ACTION, QString("Did not optimize %1: invalid optimization action generated.").arg(name()));
		onOptimizationStarted();
		onOptimizationFailed();
		return AMControl::LimitFailure;
	}

	// Otherwise, proceed with initializing and running the optimization action.
	// Create optimization action signal mappings.

	optimizationStartedMapper_->setMapping(optimizationAction, optimizationAction);
	connect( optimizationAction, SIGNAL(started()), optimizationStartedMapper_, SLOT(map()) );

	optimizationCancelledMapper_->setMapping(optimizationAction, optimizationAction);
	connect( optimizationAction, SIGNAL(cancelled()), optimizationCancelledMapper_, SLOT(map()) );

	optimizationFailedMapper_->setMapping(optimizationAction, optimizationAction);
	connect( optimizationAction, SIGNAL(failed()), optimizationFailedMapper_, SLOT(map()) );

	optimizationSucceededMapper_->setMapping(optimizationAction, optimizationAction);
	connect( optimizationAction, SIGNAL(succeeded()), optimizationSucceededMapper_, SLOT(map()) );

	// Run action.

	optimizationAction->start();

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

void AMPseudoMotorControl::setOptimizationInProgress(bool isOptimizing)
{
	if (optimizationInProgress_ != isOptimizing) {
		optimizationInProgress_ = isOptimizing;
		emit optimizingChanged(isOptimizing);
	}
}

void AMPseudoMotorControl::updateStates()
{
	updateConnected();
	updateValue();
	updateMoving();
	updateMinimumValue();
	updateMaximumValue();
	updateCalibrating();
	updateOptimizing();
}

void AMPseudoMotorControl::updateMoving()
{
	bool isMoving = false;

	// Iterate through all children and check their moving states.

	if (!children_.isEmpty()) {
		isMoving = true;

		foreach (AMControl *child, children_) {
			isMoving && child && child->isMoving();
		}
	}

	setIsMoving(isMoving);
}

void AMPseudoMotorControl::updateCalibrating()
{
	bool isCalibrating = false;

	// Iterate through all children and check their calibrating states.

	if (!children_.isEmpty()) {
		isCalibrating = true;

		foreach (AMControl *child, children_) {
			isCalibrating && child && child->calibrationInProgress();
		}
	}

	setCalibrationInProgress(isCalibrating);
}

void AMPseudoMotorControl::updateOptimizing()
{
	bool isOptimizing = false;

	// Iterate through all children and check their optimizing states.

	if (!children_.isEmpty()) {
		isOptimizing = true;

		foreach (AMControl *child, children_) {
			isOptimizing && child && child->optimizationInProgress();
		}
	}

	setCalibrationInProgress(isOptimizing);
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

void AMPseudoMotorControl::onOptimizationStarted()
{
	setOptimizationInProgress(true);
	emit optimizationStarted();
}

void AMPseudoMotorControl::onOptimizationCancelled(QObject *action)
{
	optimizationActionCleanup(action);
	onOptimizationCancelled();
}

void AMPseudoMotorControl::onOptimizationCancelled()
{
	emit calibrationFailed(AMControl::WasStoppedFailure);
}

void AMPseudoMotorControl::onOptimizationFailed(QObject *action)
{
	optimizationActionCleanup(action);
	onOptimizationFailed();
}

void AMPseudoMotorControl::onOptimizationFailed()
{
	emit calibrationFailed(AMControl::OtherFailure);
}

void AMPseudoMotorControl::onOptimizationSucceeded(QObject *action)
{
	optimizationActionCleanup(action);
	onOptimizationSucceeded();
}

void AMPseudoMotorControl::onOptimizationSucceeded()
{
	emit optimizationSucceeded();
}

void AMPseudoMotorControl::moveActionCleanup(QObject *action)
{
	setMoveInProgress(false);
	updateMoving();

	if (action) {
		startedMapper_->removeMappings(action);
		cancelledMapper_->removeMappings(action);
		failedMapper_->removeMappings(action);
		succeededMapper_->removeMappings(action);

		actionCleanup(action);
	}
}

AMAction3* AMPseudoMotorControl::createCalibrateAction(double oldValue, double newValue)
{
	Q_UNUSED(oldValue)
	Q_UNUSED(newValue)

	return 0;
}

void AMPseudoMotorControl::calibrationActionCleanup(QObject *action)
{
	setCalibrationInProgress(false);
	updateValue();

	if (action) {
		calibrationStartedMapper_->removeMappings(action);
		calibrationCancelledMapper_->removeMappings(action);
		calibrationFailedMapper_->removeMappings(action);
		calibrationSucceededMapper_->removeMappings(action);

		actionCleanup(action);
	}
}

void AMPseudoMotorControl::optimizationActionCleanup(QObject *action)
{
	setOptimizationInProgress(false);

	if (action) {
		optimizationStartedMapper_->removeMappings(action);
		optimizationCancelledMapper_->removeMappings(action);
		optimizationFailedMapper_->removeMappings(action);
		optimizationSucceededMapper_->removeMappings(action);

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
