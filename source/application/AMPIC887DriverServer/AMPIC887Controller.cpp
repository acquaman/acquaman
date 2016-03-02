#include "AMPIC887Controller.h"
#include "PI_GCS2_DLL.h"
#include "GCS2Commands/AMGCS2AsyncCommand.h"
#include "GCS2Commands/AMGCS2InitializeControllerStateCommand.h"

#include "GCS2Commands/AMGCS2SetCommandLevelCommand.h"
#include "GCS2Commands/AMGCS2SetCycleTimeCommand.h"
#include "GCS2Commands/AMGCS2SetRecordTriggerSourceCommand.h"
#include "GCS2Commands/AMGCS2SetDataRecorderConfigurationCommand.h"
#include "GCS2Commands/AMGCS2SetLowSoftLimitsCommand.h"
#include "GCS2Commands/AMGCS2SetHighSoftLimitsCommand.h"
#include "GCS2Commands/AMGCS2SetPivotPointCommand.h"
#include "GCS2Commands/AMGCS2SetSoftLimitsStatusCommand.h"
#include "GCS2Commands/AMGCS2SetStepSizeCommand.h"
#include "GCS2Commands/AMGCS2SetSystemVelocityCommand.h"
#include "GCS2Commands/AMGCS2SetServoModeCommand.h"
#include "GCS2Commands/AMGCS2SetRecordRateCommand.h"

#include "GCS2Commands/AMGCS2GetCurrentPositionCommand.h"
#include "GCS2Commands/AMGCS2AsyncGetRecordedDataValuesCommand.h"
#include "GCS2Commands/AMGCS2AsyncMoveCommand.h"
#include "GCS2Commands/AMGCS2AsyncMoveRelativeCommand.h"
#include "GCS2Commands/AMGCS2AsyncReferenceMoveCommand.h"
#include "GCS2Commands/AMGCS2StopCommand.h"
#include "GCS2Commands/AMGCS2HaltSmoothlyCommand.h"
#include "GCS2Commands/AMGCS2GetRecordedDataValuesCommand.h"
#include "AMPIC887DataRecorderConfiguration.h"

#include "AMGCS2Support.h"
#include <math.h>

AMPIC887Controller::AMPIC887Controller(const QString& name, const QString& hostname)
	:QObject(0)
{
	hostname_ = hostname;
	name_ = name;
	connectToController();
	isBusy_ = false;
	currentPositionRefreshRequired_ = false;
	controllerState_ = new AMPIC887ControllerState();
	lastRecordedPositionData_ = QVector<AMPIC887HexapodPosition>(1024, AMPIC887HexapodPosition());
	xMotions_ = 0;
	yMotions_ = 0;
	zMotions_ = 0;
	uMotions_ = 0;
	vMotions_ = 0;
	wMotions_ = 0;
	isDataRecorderActive_ = false;
	if(connectionEstablished()) {
		initializeControllerStateData();
	} else {
		lastError_ = "No connection obtained with the controller";
	}
	positionUpdateTimer_.setInterval(50);
	connect(&positionUpdateTimer_, SIGNAL(timeout()), this, SLOT(onPositionUpdateTimerInterval()));
}

AMPIC887Controller::~AMPIC887Controller()
{
	if(controllerState_ != 0) {
		delete controllerState_;
	}
}

QString AMPIC887Controller::lastError() const
{
	return lastError_;
}

bool AMPIC887Controller::connectToController()
{
	id_ = PI_ConnectTCPIP(hostname_.toStdString().c_str(), CONTROLLER_PORT);
	bool success = connectionEstablished();

	return success;
}

void AMPIC887Controller::disconnectFromController()
{
	if(id_ < 0) {
		return;
	}

	PI_CloseConnection(id_);
	id_ = -1;
}

bool AMPIC887Controller::connectionEstablished() const
{
	return id_ >= 0;
}

bool AMPIC887Controller::isInValidState() const
{
	return connectionEstablished() &&
			controllerState_->isDataValid();
}

QString AMPIC887Controller::name() const
{
	return name_;
}

QString AMPIC887Controller::hostname() const
{
	return hostname_;
}

int AMPIC887Controller::id() const
{
	return id_;
}

QString AMPIC887Controller::status() const
{
	QString connectionStatus;
	if(!connectionEstablished()) {
		connectionStatus = "Not Connected";
	} else {
		connectionStatus = QString("Connected with id %1").arg(id_);
	}

	return QString("%1\t\t\t%2\t\t\t%3\n%4")
			.arg(name_)
			.arg(hostname_)
			.arg(connectionStatus)
			.arg(controllerState_->statusString());
}

bool AMPIC887Controller::isBusy() const
{
	return isBusy_;
}

void AMPIC887Controller::clearErrorMessage()
{
	isBusy_ = true;

	errorClearingTimer_.singleShot(30, this, SLOT(onErrorClearingTimerTimedOut()));
}

bool AMPIC887Controller::isDataRecorderActive() const
{
	return isDataRecorderActive_;
}

void AMPIC887Controller::setDataRecorderActive(bool isActive)
{
	if(isDataRecorderActive_ != isActive) {

		isDataRecorderActive_ = isActive;
		emit dataRecorderActiveStateChanged(isActive);
	}
}

AMPIC887DataRecorderConfiguration AMPIC887Controller::recordConfig(int tableId) const
{
	return controllerState_->dataRecorderState()->recordConfig(tableId);
}

bool AMPIC887Controller::setRecordConfig(int tableId,
										   const AMPIC887DataRecorderConfiguration &recorderConfig)
{
	QHash<int, AMPIC887DataRecorderConfiguration> singleValue;
	singleValue.insert(tableId, recorderConfig);

	return setRecordConfigs(singleValue);
}

QHash<int, AMPIC887DataRecorderConfiguration> AMPIC887Controller::recordConfigs() const
{
	return controllerState_->dataRecorderState()->recordConfigs();
}

bool AMPIC887Controller::setRecordConfigs(const QHash<int, AMPIC887DataRecorderConfiguration>& recordConfigs)
{
	AMGCS2SetDataRecorderConfigurationCommand setRecordConfigCommand(recordConfigs);
	runCommand(&setRecordConfigCommand);

	if(setRecordConfigCommand.runningState() != AMGCS2Command::Succeeded) {
		setError(setRecordConfigCommand.lastError());
		return false;
	}

	controllerState_->dataRecorderState()->setRecordConfigs(recordConfigs);

	return true;
}

const QVector<AMPIC887HexapodPosition>& AMPIC887Controller::lastRecordedPositionData() const
{
	return lastRecordedPositionData_;
}

AMGCS2::AxisMovementStatuses AMPIC887Controller::movementStatuses() const
{
	AMGCS2::AxisMovementStatuses movements;

	if(xMotions_ > 0) {
		movements |= AMGCS2::XAxisIsMoving;
	}

	if(yMotions_ > 0) {
		movements |= AMGCS2::YAxisIsMoving;
	}

	if(zMotions_ > 0) {
		movements |= AMGCS2::ZAxisIsMoving;
	}

	if(uMotions_ > 0) {
		movements |= AMGCS2::UAxisIsMoving;
	}

	if(vMotions_ > 0) {
		movements |= AMGCS2::VAxisIsMoving;
	}

	if(wMotions_ > 0) {
		movements |= AMGCS2::WAxisIsMoving;
	}

	return movements;
}

bool AMPIC887Controller::isMoving() const
{
	return xMotions_ > 0 ||
			yMotions_ > 0 ||
			zMotions_ > 0 ||
			uMotions_ > 0 ||
			vMotions_ > 0 ||
			wMotions_ > 0;
}

QString AMPIC887Controller::availableParametersString() const
{
	return controllerState_->availableParameters();
}

AMGCS2::ControllerCommandLevel AMPIC887Controller::commandLevel() const
{
	return controllerState_->currentCommandLevel();
}

bool AMPIC887Controller::setCommandLevel(AMGCS2::ControllerCommandLevel commandLevel, const QString &password)
{
	if(controllerState_->currentCommandLevel() == commandLevel) {
		return true;
	}

	AMGCS2SetCommandLevelCommand setCommandLevelCommand(commandLevel, password);
	runCommand(&setCommandLevelCommand);

	if(setCommandLevelCommand.runningState() != AMGCS2Command::Succeeded) {
		setError(setCommandLevelCommand.lastError());
		return false;
	}

	controllerState_->setCurrentCommandLevel(commandLevel);
	return true;
}

double AMPIC887Controller::currentPosition(AMGCS2::Axis axis)
{
	if(currentPositionRefreshRequired_) {
		refreshCurrentPositions();
	}

	return controllerState_->hexapodState()->currentPosition(axis);
}

AMPIC887AxisMap<double> AMPIC887Controller::currentPositions(const AMPIC887AxisCollection &axes)
{
	if(currentPositionRefreshRequired_) {
		refreshCurrentPositions();
	}

	return controllerState_->hexapodState()->currentPositions().matchingValues(axes);
}

double AMPIC887Controller::targetPosition(AMGCS2::Axis axis)
{
	return controllerState_->hexapodState()->targetPosition(axis);
}

AMPIC887AxisMap<double> AMPIC887Controller::targetPositions(const AMPIC887AxisCollection& axes)
{
	return controllerState_->hexapodState()->targetPositions().matchingValues(axes);
}

double AMPIC887Controller::cycleTime() const
{
	return controllerState_->hexapodState()->cycleTime();
}

bool AMPIC887Controller::setCycleTime(double cycleTime)
{
	if(controllerState_->hexapodState()->cycleTime() == cycleTime) {
		return true;
	}

	AMGCS2SetCycleTimeCommand setCycleTimeCommand(cycleTime);
	runCommand(&setCycleTimeCommand);

	if(setCycleTimeCommand.runningState() != AMGCS2Command::Succeeded) {
		setError(setCycleTimeCommand.lastError());
		return false;
	}

	controllerState_->hexapodState()->setCycleTime(cycleTime);
	return true;
}

QString AMPIC887Controller::identificationString() const
{
	return controllerState_->identificationString();
}

double AMPIC887Controller::lowSoftLimit(AMGCS2::Axis axis) const
{
	return controllerState_->hexapodState()->lowSoftLimit(axis);
}

bool AMPIC887Controller::setLowSoftLimit(AMGCS2::Axis axis, double lowSoftLimit)
{
	AMPIC887AxisMap<double> singleValue;
	singleValue.insert(axis, lowSoftLimit);
	return setLowSoftLimits(singleValue);
}

AMPIC887AxisMap<double> AMPIC887Controller::lowSoftLimits(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->lowSoftLimits().matchingValues(axes);
}

bool AMPIC887Controller::setLowSoftLimits(const AMPIC887AxisMap<double>& lowSoftLimits)
{
	if(controllerState_->hexapodState()->lowSoftLimits() == lowSoftLimits) {
		return true;
	}

	AMGCS2SetLowSoftLimitsCommand setLowSoftLimitsCommand(lowSoftLimits);
	runCommand(&setLowSoftLimitsCommand);

	if(setLowSoftLimitsCommand.runningState() != AMGCS2Command::Succeeded) {
		setError(setLowSoftLimitsCommand.lastError());
		return false;
	}

	controllerState_->hexapodState()->setLowSoftLimits(lowSoftLimits);
	return true;
}

double AMPIC887Controller::highSoftLimit(AMGCS2::Axis axis) const
{
	return controllerState_->hexapodState()->highSoftLimit(axis);
}

bool AMPIC887Controller::setHighSoftLimit(AMGCS2::Axis axis, double highSoftLimit)
{
	AMPIC887AxisMap<double> singleValue;
	singleValue.insert(axis, highSoftLimit);
	return setHighSoftLimits(singleValue);
}

AMPIC887AxisMap<double> AMPIC887Controller::highSoftLimits(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->highSoftLimits().matchingValues(axes);
}

bool AMPIC887Controller::setHighSoftLimits(const AMPIC887AxisMap<double> &highSoftLimits)
{
	if(controllerState_->hexapodState()->highSoftLimits() == highSoftLimits) {
		return true;
	}

	AMGCS2SetHighSoftLimitsCommand setHighSoftLimitCommand(highSoftLimits);
	runCommand(&setHighSoftLimitCommand);

	if(setHighSoftLimitCommand.runningState() != AMGCS2Command::Succeeded) {
		setError(setHighSoftLimitCommand.lastError());
		return false;
	}

	controllerState_->hexapodState()->setHighSoftLimits(highSoftLimits);
	return true;
}

bool AMPIC887Controller::softLimitStatus(AMGCS2::Axis axis) const
{
	return controllerState_->hexapodState()->softLimitState(axis);
}

bool AMPIC887Controller::setSoftLimitStatus(AMGCS2::Axis axis, bool softLimitState)
{
	AMPIC887AxisMap<bool> singleValue;
	singleValue.insert(axis, softLimitState);
	return setSoftLimitStatuses(singleValue);
}

AMPIC887AxisMap<bool> AMPIC887Controller::softLimitStatuses(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->softLimitStates().matchingValues(axes);
}

bool AMPIC887Controller::setSoftLimitStatuses(const AMPIC887AxisMap<bool> &softLimitStates)
{
	if(controllerState_->hexapodState()->softLimitStates() == softLimitStates) {
		return true;
	}

	AMGCS2SetSoftLimitsStatusCommand setSoftLimitStatesCommand(softLimitStates);
	runCommand(&setSoftLimitStatesCommand);

	if(setSoftLimitStatesCommand.runningState() != AMGCS2Command::Succeeded) {
		setError(setSoftLimitStatesCommand.lastError());
		return false;
	}

	controllerState_->hexapodState()->setSoftLimitStates(softLimitStates);
	return true;
}

bool AMPIC887Controller::limitSwitchStatus(AMGCS2::Axis axis) const
{
	return controllerState_->hexapodState()->limitSwitchState(axis);
}

AMPIC887AxisMap<bool> AMPIC887Controller::limitSwitchStatuses(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->limitSwitchStates().matchingValues(axes);
}

double AMPIC887Controller::minCommandablePosition(AMGCS2::Axis axis) const
{
	return controllerState_->hexapodState()->minCommandablePosition(axis);
}

AMPIC887AxisMap<double> AMPIC887Controller::minCommandablePositions(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->minCommandablePositions().matchingValues(axes);
}

double AMPIC887Controller::maxCommandablePosition(AMGCS2::Axis axis) const
{
	return controllerState_->hexapodState()->maxCommandablePosition(axis);
}

AMPIC887AxisMap<double> AMPIC887Controller::maxCommandablePositions(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->maxCommandablePositions().matchingValues(axes);
}

bool AMPIC887Controller::onTargetState(AMGCS2::Axis axis)
{
	double currentAxisPosition = currentPosition(axis);
	double currentTargetPosition = targetPosition(axis);

	return qAbs(currentAxisPosition - currentTargetPosition) < AXIS_POSITION_TOLERANCE;
}

AMPIC887AxisMap<bool> AMPIC887Controller::onTargetStates(const AMPIC887AxisCollection &axes)
{
	AMPIC887AxisMap<bool> returnStates;
	foreach(AMGCS2::Axis currentAxis, axes) {
		returnStates.insert(currentAxis, onTargetState(currentAxis));
	}

	return returnStates;
}

double AMPIC887Controller::pivotPoint(AMGCS2::Axis axis) const
{
	return controllerState_->hexapodState()->pivotPoint(axis);
}

bool AMPIC887Controller::setPivotPoint(AMGCS2::Axis axis, double pivotPoint)
{
	AMPIC887AxisMap<double> singleValue;
	singleValue.insert(axis, pivotPoint);
	return setPivotPoints(singleValue);
}

AMPIC887AxisMap<double> AMPIC887Controller::pivotPoints(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->pivotPoints().matchingValues(axes);
}

bool AMPIC887Controller::setPivotPoints(const AMPIC887AxisMap<double> &pivotPoints)
{
	AMPIC887AxisCollection rotationalAxes = AMPIC887AxisCollection(AMPIC887AxisCollection::RotationalAxes);
	AMPIC887AxisMap<double> rotationalPositions = currentPositions(rotationalAxes);

	bool areAllAtOrigin = true;
	foreach(AMGCS2::Axis currentAxis, rotationalAxes) {

		areAllAtOrigin &= ( qAbs(rotationalPositions.value(currentAxis)) < AXIS_POSITION_TOLERANCE );
	}

	if(isMoving()) {
		setError("Cannot set pivot points while axes are in motion");
	} else if (!areAllAtOrigin) {
		setError("Cannot set pivot point when U, V, W axes are not all at 0");
	} else {

		if(controllerState_->hexapodState()->pivotPoints() == pivotPoints) {
			return true;
		}

		AMGCS2SetPivotPointCommand setPivotPointCommand(pivotPoints);
		runCommand(&setPivotPointCommand);

		if(setPivotPointCommand.runningState() != AMGCS2Command::Succeeded) {
			setError(setPivotPointCommand.lastError());
			return false;
		}

		controllerState_->hexapodState()->setPivotPoints(pivotPoints);

		return true;
	}

	return false;
}

AMGCS2::PositionUnits AMPIC887Controller::positionUnits(AMGCS2::Axis axis) const
{
	return controllerState_->hexapodState()->positionUnits(axis);
}

AMPIC887AxisMap<AMGCS2::PositionUnits> AMPIC887Controller::positionUnits(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->positionUnits().matchingValues(axes);
}

QList<double> AMPIC887Controller::recordedData(int offset, int numberOfDataPoints, int tableId)
{
	AMGCS2GetRecordedDataValuesCommand recordedDataCommand(tableId, numberOfDataPoints, offset);
	runCommand(&recordedDataCommand);

	if(recordedDataCommand.runningState() == AMGCS2Command::Succeeded) {

		return recordedDataCommand.dataValues();
	}

	return QList<double>();
}

QString AMPIC887Controller::recorderOptionsString() const
{
	return controllerState_->dataRecorderState()->availableParameters();
}

double AMPIC887Controller::recordRate() const
{
	return controllerState_->dataRecorderState()->recordRate();
}

bool AMPIC887Controller::setRecordRate(double recordRate)
{
	if(controllerState_->dataRecorderState()->recordRate() ==  recordRate) {
		return true;
	}

	AMGCS2SetRecordRateCommand setRecordRateCommand(recordRate);

	runCommand(&setRecordRateCommand);

	if(setRecordRateCommand.runningState() != AMGCS2Command::Succeeded) {
		setError(setRecordRateCommand.lastError());
		return false;
	}

	controllerState_->dataRecorderState()->setRecordRate(recordRate);
	emit recordRateChanged(recordRate);
	return true;
}

AMGCS2::DataRecordTrigger AMPIC887Controller::recordTrigger() const
{
	return controllerState_->dataRecorderState()->recordTrigger();
}

bool AMPIC887Controller::setRecordTrigger(AMGCS2::DataRecordTrigger recordTrigger)
{
	if(controllerState_->dataRecorderState()->recordTrigger() == recordTrigger) {
		return true;
	}

	AMGCS2SetRecordTriggerSourceCommand setRecordTriggerCommand(recordTrigger);
	runCommand(&setRecordTriggerCommand);

	if(setRecordTriggerCommand.runningState() != AMGCS2Command::Succeeded) {
		setError(setRecordTriggerCommand.lastError());
		return false;
	}

	controllerState_->dataRecorderState()->setRecordTrigger(recordTrigger);

	return true;
}

bool AMPIC887Controller::isAxisReferenced(AMGCS2::Axis axis) const
{
	return controllerState_->hexapodState()->referencedState(axis);
}

AMPIC887AxisMap<bool> AMPIC887Controller::axisReferencedStates(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->referencedStates().matchingValues(axes);
}

bool AMPIC887Controller::areAxesReferenced() const
{
	return controllerState_->hexapodState()->areAllAxesReferenced();
}

bool AMPIC887Controller::isInServoMode() const
{
	return controllerState_->hexapodState()->isInServoMode();
}

bool AMPIC887Controller::setServoMode(bool servoMode)
{
	if(controllerState_->hexapodState()->isInServoMode() == servoMode) {
		return true;
	}

	AMGCS2SetServoModeCommand setServoModeCommand(servoMode);
	runCommand(&setServoModeCommand);

	if(setServoModeCommand.runningState() != AMGCS2Command::Succeeded) {
		setError(setServoModeCommand.lastError());
		return false;
	}

	controllerState_->hexapodState()->setServoMode(servoMode);
	return true;
}

double AMPIC887Controller::stepSize(AMGCS2::Axis axis)
{
	return controllerState_->hexapodState()->stepSize(axis);
}

bool AMPIC887Controller::setStepSize(AMGCS2::Axis axis, double stepSize)
{
	AMPIC887AxisMap<double> singleValue;
	singleValue.insert(axis, stepSize);
	return setStepSizes(singleValue);
}

AMPIC887AxisMap<double> AMPIC887Controller::stepSizes(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->stepSizes().matchingValues(axes);
}

bool AMPIC887Controller::setStepSizes(const AMPIC887AxisMap<double> &stepSizes)
{
	if(controllerState_->hexapodState()->stepSizes() == stepSizes) {
		return true;
	}

	AMGCS2SetStepSizeCommand setStepSizeCommand(stepSizes);
	runCommand(&setStepSizeCommand);

	if(setStepSizeCommand.runningState() != AMGCS2Command::Succeeded) {
		setError(setStepSizeCommand.lastError());
		return false;
	}

	controllerState_->hexapodState()->setStepSizes(stepSizes);
	return true;
}

double AMPIC887Controller::systemVelocity() const
{
	return controllerState_->hexapodState()->velocity();
}

bool AMPIC887Controller::setSystemVelocity(double systemVelocity)
{
	if(controllerState_->hexapodState()->velocity() == systemVelocity) {
		return true;
	}

	AMGCS2SetSystemVelocityCommand setSystemVelocityCommand(systemVelocity);
	runCommand(&setSystemVelocityCommand);

	if(setSystemVelocityCommand.runningState() != AMGCS2Command::Succeeded) {
		setError(setSystemVelocityCommand.lastError());
		return false;
	}

	controllerState_->hexapodState()->setVelocity(systemVelocity);
	emit systemVelocityChanged(systemVelocity);
	return true;
}

bool AMPIC887Controller::stop()
{
	AMGCS2StopCommand stopCommand;
	runCommand(&stopCommand);

	if(stopCommand.runningState() != AMGCS2Command::Succeeded) {

		emit errorEncountered(QString("Failed to stop controller with message: %1")
							  .arg(stopCommand.lastError()));
		return false;
	}

	return true;
}

bool AMPIC887Controller::haltSmoothly(const AMPIC887AxisCollection& axes)
{
	AMGCS2HaltSmoothlyCommand haltCommand(axes);
	runCommand(&haltCommand);

	if(haltCommand.runningState() != AMGCS2Command::Succeeded) {
		setError(haltCommand.lastError());
		return false;
	}

	return true;
}

void AMPIC887Controller::move(const AMPIC887AxisMap<double>& axisPositions)
{
	if(isMoving()) {

		setError("Cannot perform movement. Hexapod is already in motion");

	} else {

		AMPIC887AxisCollection axesToMove = axisPositions.axes();
		foreach(AMGCS2::Axis currentAxis, axesToMove) {
			if(!controllerState_->hexapodState()->referencedState(currentAxis)) {
				setError(QString("Cannot move axis %1: Axis not referenced")
						 .arg(AMGCS2Support::axisToCharacter(currentAxis)));
				return;
			}
		}

		AMGCS2AsyncMoveCommand* asyncMoveCommand = new AMGCS2AsyncMoveCommand(axisPositions);

		connect(asyncMoveCommand, SIGNAL(started(AMGCS2AsyncCommand*)),
				this, SLOT(onAsyncMoveStarted(AMGCS2AsyncCommand*)));
		connect(asyncMoveCommand, SIGNAL(failed(AMGCS2AsyncCommand*)),
				this, SLOT(onAsyncMoveFailed(AMGCS2AsyncCommand*)));
		connect(asyncMoveCommand, SIGNAL(succeeded(AMGCS2AsyncCommand*)),
				this, SLOT(onAsyncMoveSucceeded(AMGCS2AsyncCommand*)));

		runCommand(asyncMoveCommand);
	}
}

void AMPIC887Controller::moveRelative(const AMPIC887AxisMap<double>& relativePositions)
{
	if(isMoving()) {

		setError("Cannot perform movement. Hexapod is already in motion");

	} else {

		AMPIC887AxisCollection axesToMove = relativePositions.axes();
		foreach(AMGCS2::Axis currentAxis, axesToMove) {
			if(!controllerState_->hexapodState()->referencedState(currentAxis)) {
				setError(QString("Cannot move axis %1: Axis not referenced")
						 .arg(AMGCS2Support::axisToCharacter(currentAxis)));
				return;
			}
		}

		AMGCS2AsyncMoveRelativeCommand* asyncMoveRelativeCommand =
				new AMGCS2AsyncMoveRelativeCommand(relativePositions);

		connect(asyncMoveRelativeCommand, SIGNAL(started(AMGCS2AsyncCommand*)),
				this, SLOT(onAsyncMoveRelativeStarted(AMGCS2AsyncCommand*)));
		connect(asyncMoveRelativeCommand, SIGNAL(failed(AMGCS2AsyncCommand*)),
				this, SLOT(onAsyncMoveRelativeFailed(AMGCS2AsyncCommand*)));
		connect(asyncMoveRelativeCommand, SIGNAL(succeeded(AMGCS2AsyncCommand*)),
				this, SLOT(onAsyncMoveRelativeSucceeded(AMGCS2AsyncCommand*)));

		runCommand(asyncMoveRelativeCommand);
	}
}

void AMPIC887Controller::referenceMove(const AMPIC887AxisCollection& axes)
{
	if(isMoving()) {

		setError("Cannot perform reference move. Hexapod is already in motion");

	} else {

		AMGCS2AsyncReferenceMoveCommand* asyncReferenceMoveCommand =
				new AMGCS2AsyncReferenceMoveCommand(axes);

		connect(asyncReferenceMoveCommand, SIGNAL(started(AMGCS2AsyncCommand*)),
				this, SLOT(onAsyncReferenceMoveStarted(AMGCS2AsyncCommand*)));
		connect(asyncReferenceMoveCommand, SIGNAL(failed(AMGCS2AsyncCommand*)),
				this, SLOT(onAsyncReferenceMoveFailed(AMGCS2AsyncCommand*)));
		connect(asyncReferenceMoveCommand, SIGNAL(succeeded(AMGCS2AsyncCommand*)),
				this, SLOT(onAsyncReferenceMoveSucceeded(AMGCS2AsyncCommand*)));

		runCommand(asyncReferenceMoveCommand);
	}
}

void AMPIC887Controller::initializeControllerStateData()
{
	if(!connectionEstablished()) {
		setError("Cannot initialize data for controller, no connection established");
	} else {

		AMGCS2InitializeControllerStateCommand initializeCommand =
				AMGCS2InitializeControllerStateCommand(controllerState_);

		runCommand(&initializeCommand);
	}
}

void AMPIC887Controller::onPositionUpdateTimerInterval()
{
	emit positionUpdate(currentPositions(AMPIC887AxisCollection()));
}

void AMPIC887Controller::onAsyncMoveStarted(AMGCS2AsyncCommand *command)
{
	currentPositionRefreshRequired_ = true;
	AMGCS2AsyncMoveCommand* moveCommand = qobject_cast<AMGCS2AsyncMoveCommand*>(command);

	if(moveCommand) {


		AMPIC887AxisCollection axesMoving = moveCommand->targetPositions().axes();

		foreach(AMGCS2::Axis currentAxis, axesMoving) {

			controllerState_->hexapodState()->setTargetPosition(currentAxis, moveCommand->targetPositions().value(currentAxis));


			switch (currentAxis) {
			case AMGCS2::XAxis:
				++xMotions_;
				break;
			case AMGCS2::YAxis:
				++yMotions_;
				break;
			case AMGCS2::ZAxis:
				++zMotions_;
				break;
			case AMGCS2::UAxis:
				++uMotions_;
				break;
			case AMGCS2::VAxis:
				++vMotions_;
				break;
			case AMGCS2::WAxis:
				++wMotions_;
				break;
			default:
				break;
			}
		}

		emit moveStarted(movementStatuses());
		positionUpdateTimer_.start();

		if(isDataRecorderActive_) {
			QTimer::singleShot(calculateDataRetrievalDelay(), this, SLOT(startAsyncDataRetrieval()));
		}
	}
}

void AMPIC887Controller::onAsyncMoveSucceeded(AMGCS2AsyncCommand *command)
{
	AMGCS2AsyncMoveCommand* moveCommand = qobject_cast<AMGCS2AsyncMoveCommand*>(command);

	if(moveCommand) {

		positionUpdateTimer_.stop();
		// update position one last time
		onPositionUpdateTimerInterval();
		AMPIC887AxisCollection axesMoving = moveCommand->targetPositions().axes();

		foreach(AMGCS2::Axis currentAxis, axesMoving) {
			switch (currentAxis) {
			case AMGCS2::XAxis:
				--xMotions_;
				break;
			case AMGCS2::YAxis:
				--yMotions_;
				break;
			case AMGCS2::ZAxis:
				--zMotions_;
				break;
			case AMGCS2::UAxis:
				--uMotions_;
				break;
			case AMGCS2::VAxis:
				--vMotions_;
				break;
			case AMGCS2::WAxis:
				--wMotions_;
				break;
			default:
				break;
			}
		}
		if(!isDataRecorderActive_) {
			emit moveComplete();
		}
		emit targetPositionChanged(controllerState_->hexapodState()->targetPositions());
	}

	command->deleteLater();
}

void AMPIC887Controller::onAsyncMoveFailed(AMGCS2AsyncCommand *command)
{
	AMGCS2AsyncMoveCommand* moveCommand = qobject_cast<AMGCS2AsyncMoveCommand*>(command);

	if(moveCommand) {
		setError(QString("Move failed with message: %1").arg(command->lastError()));
		positionUpdateTimer_.stop();
		emit moveFailed(movementStatuses());
		AMPIC887AxisCollection axesMoving = moveCommand->targetPositions().axes();
		foreach(AMGCS2::Axis currentAxis, axesMoving) {
			switch (currentAxis) {
			case AMGCS2::XAxis:
				--xMotions_;
				break;
			case AMGCS2::YAxis:
				--yMotions_;
				break;
			case AMGCS2::ZAxis:
				--zMotions_;
				break;
			case AMGCS2::UAxis:
				--uMotions_;
				break;
			case AMGCS2::VAxis:
				--vMotions_;
				break;
			case AMGCS2::WAxis:
				--wMotions_;
				break;
			default:
				break;
			}
		}
	}
	updateStateOnStop();
	command->deleteLater();
}

void AMPIC887Controller::onAsyncMoveRelativeStarted(AMGCS2AsyncCommand *command)
{
	currentPositionRefreshRequired_ = true;
	AMGCS2AsyncMoveRelativeCommand* moveRelativeCommand = qobject_cast<AMGCS2AsyncMoveRelativeCommand*>(command);

	if(moveRelativeCommand) {


		AMPIC887AxisCollection axesMoving = moveRelativeCommand->relativeTargetPositions().axes();

		foreach(AMGCS2::Axis currentAxis, axesMoving) {

			double currentTarget = controllerState_->hexapodState()->targetPosition(currentAxis);
			double relativeTarget = moveRelativeCommand->relativeTargetPositions().value(currentAxis);
			controllerState_->hexapodState()->setTargetPosition(currentAxis, currentTarget + relativeTarget);

			switch (currentAxis) {
			case AMGCS2::XAxis:
				++xMotions_;
				break;
			case AMGCS2::YAxis:
				++yMotions_;
				break;
			case AMGCS2::ZAxis:
				++zMotions_;
				break;
			case AMGCS2::UAxis:
				++uMotions_;
				break;
			case AMGCS2::VAxis:
				++vMotions_;
				break;
			case AMGCS2::WAxis:
				++wMotions_;
				break;
			default:
				break;
			}
		}


		emit moveStarted(movementStatuses());
		positionUpdateTimer_.start();
		if(isDataRecorderActive_) {
			QTimer::singleShot(calculateDataRetrievalDelay(), this, SLOT(startAsyncDataRetrieval()));
		}
	}
}

void AMPIC887Controller::onAsyncMoveRelativeSucceeded(AMGCS2AsyncCommand *command)
{
	AMGCS2AsyncMoveRelativeCommand* moveRelativeCommand = qobject_cast<AMGCS2AsyncMoveRelativeCommand*>(command);

	if(moveRelativeCommand) {

		positionUpdateTimer_.stop();		
		// update position one last time
		onPositionUpdateTimerInterval();

		AMPIC887AxisCollection axesMoving = moveRelativeCommand->relativeTargetPositions().axes();
		foreach(AMGCS2::Axis currentAxis, axesMoving) {
			switch (currentAxis) {
			case AMGCS2::XAxis:
				--xMotions_;
				break;
			case AMGCS2::YAxis:
				--yMotions_;
				break;
			case AMGCS2::ZAxis:
				--zMotions_;
				break;
			case AMGCS2::UAxis:
				--uMotions_;
				break;
			case AMGCS2::VAxis:
				--vMotions_;
				break;
			case AMGCS2::WAxis:
				--wMotions_;
				break;
			default:
				break;
			}
		}
		if(!isDataRecorderActive_) {
			emit moveComplete();
		}
		emit targetPositionChanged(controllerState_->hexapodState()->targetPositions());
	}

	command->deleteLater();
}

void AMPIC887Controller::onAsyncMoveRelativeFailed(AMGCS2AsyncCommand *command)
{
	AMGCS2AsyncMoveRelativeCommand* moveRelativeCommand = qobject_cast<AMGCS2AsyncMoveRelativeCommand*>(command);

	if(moveRelativeCommand) {
		positionUpdateTimer_.stop();
		emit moveFailed(movementStatuses());
		AMPIC887AxisCollection axesMoving = moveRelativeCommand->relativeTargetPositions().axes();
		foreach(AMGCS2::Axis currentAxis, axesMoving) {
			switch (currentAxis) {
			case AMGCS2::XAxis:
				--xMotions_;
				break;
			case AMGCS2::YAxis:
				--yMotions_;
				break;
			case AMGCS2::ZAxis:
				--zMotions_;
				break;
			case AMGCS2::UAxis:
				--uMotions_;
				break;
			case AMGCS2::VAxis:
				--vMotions_;
				break;
			case AMGCS2::WAxis:
				--wMotions_;
				break;
			default:
				break;
			}
		}
	}
	updateStateOnStop();
	command->deleteLater();
}

// observing the bahviour of the controller shows that the statuses of the
// axes when a reference move is under way are all or nothing (i.e. if
// a reference move is started on X, the referenced status of all axes
// is set to unreferenced, and the movement status of all axes is set to
// moving).
void AMPIC887Controller::onAsyncReferenceMoveStarted(AMGCS2AsyncCommand *)
{
	currentPositionRefreshRequired_ = true;
	++xMotions_;
	++yMotions_;
	++zMotions_;
	++uMotions_;
	++vMotions_;
	++wMotions_;

	controllerState_->hexapodState()->setReferencedState(AMGCS2::XAxis, false);
	controllerState_->hexapodState()->setReferencedState(AMGCS2::YAxis, false);
	controllerState_->hexapodState()->setReferencedState(AMGCS2::ZAxis, false);
	controllerState_->hexapodState()->setReferencedState(AMGCS2::UAxis, false);
	controllerState_->hexapodState()->setReferencedState(AMGCS2::VAxis, false);
	controllerState_->hexapodState()->setReferencedState(AMGCS2::WAxis, false);

	controllerState_->hexapodState()->setTargetPosition(AMGCS2::XAxis, 0);
	controllerState_->hexapodState()->setTargetPosition(AMGCS2::YAxis, 0);
	controllerState_->hexapodState()->setTargetPosition(AMGCS2::ZAxis, 0);
	controllerState_->hexapodState()->setTargetPosition(AMGCS2::UAxis, 0);
	controllerState_->hexapodState()->setTargetPosition(AMGCS2::VAxis, 0);
	controllerState_->hexapodState()->setTargetPosition(AMGCS2::WAxis, 0);

	emit moveStarted(movementStatuses());
	positionUpdateTimer_.start();
	if(isDataRecorderActive_) {
		QTimer::singleShot(calculateDataRetrievalDelay(), this, SLOT(startAsyncDataRetrieval()));
	}
}

void AMPIC887Controller::onAsyncReferenceMoveSucceeded(AMGCS2AsyncCommand *command)
{
	positionUpdateTimer_.stop();	
	// update position one last time
	onPositionUpdateTimerInterval();

	--xMotions_;
	--yMotions_;
	--zMotions_;
	--uMotions_;
	--vMotions_;
	--wMotions_;

	controllerState_->hexapodState()->setReferencedState(AMGCS2::XAxis, true);
	controllerState_->hexapodState()->setReferencedState(AMGCS2::YAxis, true);
	controllerState_->hexapodState()->setReferencedState(AMGCS2::ZAxis, true);
	controllerState_->hexapodState()->setReferencedState(AMGCS2::UAxis, true);
	controllerState_->hexapodState()->setReferencedState(AMGCS2::VAxis, true);
	controllerState_->hexapodState()->setReferencedState(AMGCS2::WAxis, true);

	if(!isDataRecorderActive_) {
		emit moveComplete();
	}
	emit targetPositionChanged(controllerState_->hexapodState()->targetPositions());

	command->deleteLater();
}

void AMPIC887Controller::onAsyncReferenceMoveFailed(AMGCS2AsyncCommand *command)
{
	positionUpdateTimer_.stop();
	emit moveFailed(movementStatuses());
	--xMotions_;
	--yMotions_;
	--zMotions_;
	--uMotions_;
	--vMotions_;
	--wMotions_;

	updateStateOnStop();

	command->deleteLater();
}

void AMPIC887Controller::onAsyncDataRetrievalSucceeded(AMGCS2AsyncCommand *command)
{
	emit moveComplete();
	command->deleteLater();
}

void AMPIC887Controller::onAsyncDataRetrievalFailed(AMGCS2AsyncCommand *command)
{
	emit moveComplete();
	command->deleteLater();
}

void AMPIC887Controller::onErrorClearingTimerTimedOut()
{
	int dummyValue;
	PI_qERR(id_, &dummyValue);
	isBusy_ = false;
	updateStateOnStop();
}

void AMPIC887Controller::startAsyncDataRetrieval()
{
	AMGCS2AsyncGetRecordedDataValuesCommand* recordedPositionDataCommand =
			new AMGCS2AsyncGetRecordedDataValuesCommand(lastRecordedPositionData_.data());

	connect(recordedPositionDataCommand, SIGNAL(succeeded(AMGCS2AsyncCommand*)),
			this, SLOT(onAsyncDataRetrievalSucceeded(AMGCS2AsyncCommand*)));
	connect(recordedPositionDataCommand, SIGNAL(failed(AMGCS2AsyncCommand*)),
			this, SLOT(onAsyncDataRetrievalFailed(AMGCS2AsyncCommand*)));

	runCommand(recordedPositionDataCommand);
}

void AMPIC887Controller::runCommand(AMGCS2Command *command)
{
	if(command) {
		command->setController(this);
		command->run();
	}
}

void AMPIC887Controller::refreshCurrentPositions()
{
	AMGCS2GetCurrentPositionCommand currentPositionCommand;
	runCommand(&currentPositionCommand);

	if(currentPositionCommand.runningState() != AMGCS2Command::Succeeded) {
		emit errorEncountered("Cannot obtain current position from controller");
		return;
	}

	AMPIC887AxisMap<double> currentPositions =
			currentPositionCommand.axisPositions();

	AMPIC887AxisCollection allAxes = currentPositions.axes();


	foreach(AMGCS2::Axis currentAxis, allAxes) {
		controllerState_->hexapodState()->setCurrentPosition(currentAxis, currentPositions.value(currentAxis));
	}

	if(!isMoving()) {
		currentPositionRefreshRequired_ = false;
	}
}

void AMPIC887Controller::updateStateOnStop()
{
	refreshCurrentPositions();
	emit targetPositionChanged(controllerState_->hexapodState()->targetPositions());
	// Set target position = current position
	AMPIC887AxisCollection allAxes;
	foreach(AMGCS2::Axis currentAxis, allAxes) {
		double currentPosition = controllerState_->hexapodState()->currentPosition(currentAxis);

		controllerState_->hexapodState()->setTargetPosition(currentAxis, currentPosition);
	}
}

void AMPIC887Controller::setError(const QString &errorMessage)
{
	lastError_ = errorMessage;
	emit errorEncountered(errorMessage);
}

int AMPIC887Controller::calculateDataRetrievalDelay()
{
	// For now we just assume that only X, Y and Z are moving.
	AMPIC887AxisMap<double> current = currentPositions(AMPIC887AxisCollection());
	AMPIC887AxisMap<double> target = targetPositions(AMPIC887AxisCollection());
	double deltaX = target.value(AMGCS2::XAxis) - current.value(AMGCS2::XAxis);
	double deltaY = target.value(AMGCS2::YAxis) - current.value(AMGCS2::YAxis);
	double deltaZ = target.value(AMGCS2::ZAxis) - current.value(AMGCS2::ZAxis);

	double distance = sqrt((deltaX * deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ));

	// We wait for the estimated time for the motion to take place (in ms) plus
	// 150ms. This was determined through experimentation to be sufficient time
	// to wait before the data was ready to be retrieved.
	return int((distance/ systemVelocity()) * 1000) + 150;

}



