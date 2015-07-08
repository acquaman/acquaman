#include "AMPIC887Controller.h"
#include "AMGCS2CommandFactory.h"
#include "PI_GCS2_DLL.h"
#include "GCS2Commands/AMGCS2AsyncCommand.h"
#include "GCS2Commands/AMGCS2InitializeControllerStateCommand.h"
#include "GCS2Commands/AMGCS2AsyncMoveCommand.h"
#include "AMGCS2Support.h"

AMPIC887Controller::AMPIC887Controller(const QString& name, const QString& hostname)
	:QObject(0)
{
	hostname_ = hostname;
	name_ = name;
	connectToController();
	isBusy_ = false;
	lastMoveCommand_ = 0;
	controllerState_ = new AMPIC887ControllerState();
	if(connectionEstablished()) {
		initializeControllerStateData();
	}
}

AMPIC887Controller::~AMPIC887Controller()
{
	if(controllerState_ != 0) {
		delete controllerState_;
	}
}

void AMPIC887Controller::interpretAndRunCommand(const QString &commandText)
{
	AMGCS2Command* command = AMGCS2CommandFactory::buildCommand(commandText);

	if(!command) {
		emit errorEncountered("Command could not be parsed");
		return;
	}

	if(command->commandType() == AMGCS2Command::Asynchronous) {

		AMGCS2AsyncCommand* asyncCommand = dynamic_cast<AMGCS2AsyncCommand*>(command);

		if(asyncCommand) {
			connect(asyncCommand, SIGNAL(succeeded(AMGCS2AsyncCommand*)), this, SLOT(onAsyncCommandSucceeded(AMGCS2AsyncCommand*)));
			connect(asyncCommand, SIGNAL(failed(AMGCS2AsyncCommand*)), this, SLOT(onAsyncCommandFailed(AMGCS2AsyncCommand*)));
			runCommand(asyncCommand);
		}
	} else {

		runCommand(command);

		if(command->runningState() == AMGCS2Command::Succeeded) {

			QString outputString = command->outputString();

			if(!outputString.isEmpty()) {
				emit output(outputString);
			}
		} else {
			emit errorEncountered(command->lastError());
		}

		delete command;
	}
}

bool AMPIC887Controller::connectToController()
{
	id_ = PI_ConnectTCPIP(hostname_.toStdString().c_str(), CONTROLLER_PORT);
	bool success = connectionEstablished();
	if(success) {
		PI_SetErrorCheck(id_, 0);
	}

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


void AMPIC887Controller::move(const QHash<AMGCS2::Axis, double> &axisPositions)
{
	if(!controllerState_->isAllInitialized()) {
		emit errorEncountered("Cannot perform move: Controller not initialized");
		return;
	}

	foreach(AMGCS2::Axis currentAxis, axisPositions.keys()) {

		if(currentAxis == AMGCS2::UnknownAxis) {
			emit errorEncountered("Cannot perform move: Unknown axis specified");
			return;
		}

		if(!controllerState_->hexapodState()->axisState(currentAxis)->isReferenced()) {
			emit errorEncountered(QString("Cannot perform move: Axis %1 not referenced")
								  .arg(AMGCS2Support::axisToCharacter(currentAxis)));
			return;
		}
	}

	AMGCS2AsyncMoveCommand* moveCommand = new AMGCS2AsyncMoveCommand(axisPositions);
	connect(moveCommand, SIGNAL(succeeded(AMGCS2AsyncCommand*)), this, SLOT(onAsyncCommandSucceeded(AMGCS2AsyncCommand*)));
	connect(moveCommand, SIGNAL(failed(AMGCS2AsyncCommand*)), this, SLOT(onAsyncCommandFailed(AMGCS2AsyncCommand*)));

	runCommand(moveCommand);
}

void AMPIC887Controller::referenceMove(const QList<AMGCS2::Axis> &axes)
{
}

void AMPIC887Controller::moveRelative(const QHash<AMGCS2::Axis, double> &axisRelativePositions)
{
}

double AMPIC887Controller::targetPosition(AMGCS2::Axis axis) const
{
	if(!controllerState_->isAllInitialized()) {
		emit errorEncountered("Cannot obtain target position: Controller not initialized");
		return 0;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain target position: Unknown axis");
		return 0;
	}

	return controllerState_->hexapodState()->axisState(axis)->targetPosition();
}

double AMPIC887Controller::currentPosition(AMGCS2::Axis axis) const
{
	return 0;
}

double AMPIC887Controller::cycleTime() const
{
	if(!controllerState_->isAllInitialized()) {
		emit errorEncountered("Cannot obtain cycle time: Controller not initialized");
		return 0;
	}

	return controllerState_->hexapodState()->cycleTime();
}

void AMPIC887Controller::setCycleTime(double cycleTime)
{
}

AMGCS2::DataRecordTrigger AMPIC887Controller::recordTrigger() const
{
	if(!controllerState_->isAllInitialized()) {
		emit errorEncountered("Cannot obtain record trigger: Controller not initialized");
		return AMGCS2::UnknownRecordTrigger;
	}

	return controllerState_->dataRecorderState()->recordTrigger();
}

void AMPIC887Controller::setRecordTrigger(AMGCS2::DataRecordTrigger trigger)
{

}

AMGCS2::DataRecordSource AMPIC887Controller::recordSource(int tableId) const
{
	if(!controllerState_->isAllInitialized()) {
		emit errorEncountered("Cannot obtain record source: Controller not initialized");
		return AMGCS2::UnknownDataRecordSource;
	}

	if(tableId < 1 && tableId > RECORD_TABLE_COUNT) {
		emit errorEncountered(QString("Cannot obtain record source: Table id %1 does not exist")
							  .arg(tableId));
		return AMGCS2::UnknownDataRecordSource;
	}

	return controllerState_->dataRecorderState()->stateAt(tableId)->recordSource();
}

void AMPIC887Controller::setRecordSource(int tableId, AMGCS2::DataRecordSource source)
{

}

AMGCS2::DataRecordOption AMPIC887Controller::recordOption(int tableId) const
{
	if(!controllerState_->isAllInitialized()) {
		emit errorEncountered("Cannot obtain record option: Controller not initialized");
		return AMGCS2::UnknownRecordOption;
	}

	if(tableId < 1 && tableId > RECORD_TABLE_COUNT) {
		emit errorEncountered(QString("Cannot obtain record option: Table id %1 does not exist")
							  .arg(tableId));
		return AMGCS2::UnknownRecordOption;
	}

	return controllerState_->dataRecorderState()->stateAt(tableId)->recordOption();
}

void AMPIC887Controller::setRecordOption(int tableId, AMGCS2::DataRecordOption option)
{
}

double AMPIC887Controller::lowSoftLimit(AMGCS2::Axis axis) const
{
	if(!controllerState_->isAllInitialized()) {
		emit errorEncountered("Cannot obtain low soft limit: Controller not initialized");
		return 0;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain low soft limit: Unknown axis");
		return 0;
	}

	return controllerState_->hexapodState()->axisState(axis)->lowSoftLimit();
}

void AMPIC887Controller::setLowSoftLimit(AMGCS2::Axis axis, double lowSoftLimit)
{
}

double AMPIC887Controller::highSoftLimit(AMGCS2::Axis axis) const
{
	if(!controllerState_->isAllInitialized()) {
		emit errorEncountered("Cannot obtain high soft limit: Controller not initialized");
		return 0;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain high soft limit: Unknown axis");
		return 0;
	}

	return controllerState_->hexapodState()->axisState(axis)->highSoftLimit();
}

void AMPIC887Controller::setHighSoftLimit(AMGCS2::Axis axis, double highSoftLimit)
{
}

bool AMPIC887Controller::areSoftLimitsActive(AMGCS2::Axis axis) const
{
	if(!controllerState_->isAllInitialized()) {
		emit errorEncountered("Cannot obtain soft limit state: Controller not initialized");
		return 0;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain soft limit state: Unknown axis");
		return 0;
	}

	return controllerState_->hexapodState()->axisState(axis)->areSoftLimitsActive();
}

void AMPIC887Controller::setSoftLimitsActive(AMGCS2::Axis axis, bool active)
{
}

double AMPIC887Controller::minCommandablePosition(AMGCS2::Axis axis) const
{
	if(!controllerState_->isAllInitialized()) {
		emit errorEncountered("Cannot obtain min commandable position: Controller not initialized");
		return 0;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain min commandable position: Unknown axis");
		return 0;
	}

	return controllerState_->hexapodState()->axisState(axis)->minCommandablePosition();
}

double AMPIC887Controller::maxCommandablePosition(AMGCS2::Axis axis) const
{
	if(!controllerState_->isAllInitialized()) {
		emit errorEncountered("Cannot obtain max commandable position: Controller not initialized");
		return 0;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain max commandable position: Unknown axis");
		return 0;
	}

	return controllerState_->hexapodState()->axisState(axis)->maxCommandablePosition();
}

QFlags<AMGCS2::AxisMovementStatus> AMPIC887Controller::movingStatuses() const
{
	return QFlags<AMGCS2::AxisMovementStatus>();
}

double AMPIC887Controller::pivotPoint(AMGCS2::Axis axis) const
{
	if(!controllerState_->isAllInitialized()) {
		emit errorEncountered("Cannot obtain pivot point: Controller not initialized");
		return 0;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain pivot point: Unknown axis");
		return 0;
	}

	if(axis == AMGCS2::UAxis ||
			axis == AMGCS2::VAxis ||
			axis == AMGCS2::WAxis) {

		emit errorEncountered(QString("Cannot obtain pivot point: Rotational axis %1 has no pivot point")
							  .arg(AMGCS2Support::axisToCharacter(axis)));
		return 0;
	}

	return controllerState_->hexapodState()->axisState(axis)->pivotPoint();
}

void AMPIC887Controller::setPivotPoint(AMGCS2::Axis axis, double pivotPoint) const
{
}

QList<int> AMPIC887Controller::recordedData(int tableId, int offset, int count)
{
	if(!controllerState_->isAllInitialized()) {
		emit errorEncountered("Could not obtain recorded data: Controller not initialized");
		return QList<int>();
	}

	if(tableId < 0 || tableId > RECORD_TABLE_COUNT) {
		emit errorEncountered(QString("Could not obtain recorded data: Table id %1 not valid")
							  .arg(tableId));
		return QList<int>();
	}

	if(offset < 0) {
		emit errorEncountered(QString("Could not obtain recorded data: Offset %1 not valid")
							  .arg(offset));
		return QList<int>();
	}

	// Todo: obtain count and data. Clear the current data and add this instead.

	return controllerState_->dataRecorderState()->stateAt(tableId)->mid(offset, count);
}

bool AMPIC887Controller::isAxisReferenced(AMGCS2::Axis axis) const
{
	if(!controllerState_->isAllInitialized()) {
		emit errorEncountered("Cannot obtain referenced state: Controller not initialized");
		return false;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain referenced state: Unknown axis");
		return false;
	}

	return controllerState_->hexapodState()->axisState(axis)->isReferenced();
}

bool AMPIC887Controller::areAllAxesReferenced() const
{
	if(!controllerState_->isAllInitialized()) {
		emit errorEncountered("Cannot obtain referenced state: Controller not initialized");
		return false;
	}

	return controllerState_->hexapodState()->areAllAxesReferenced();
}

bool AMPIC887Controller::isInServoMode() const
{
	if(!controllerState_->isAllInitialized()) {
		emit errorEncountered("Cannot obtain servo mode state: Controller not initialized");
		return false;
	}

	return controllerState_->hexapodState()->isInServoMode();
}

void AMPIC887Controller::setServoMode(bool servoModeActive)
{
}

double AMPIC887Controller::stepSize(AMGCS2::Axis axis) const
{
	if(!controllerState_->isAllInitialized()) {
		emit errorEncountered("Cannot obtain step size: Controller not initialized");
		return false;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain step size: Unknown axis");
		return false;
	}

	return controllerState_->hexapodState()->axisState(axis)->stepSize();
}

double AMPIC887Controller::systemVelocity() const
{
	return 0;
}

void AMPIC887Controller::stop() const
{
}

void AMPIC887Controller::haltSmoothly() const
{
}

bool AMPIC887Controller::isMoveSafe(const QHash<AMGCS2::Axis, double> &axisPositions) const
{
	return false;
}

void AMPIC887Controller::onAsyncCommandFailed(AMGCS2AsyncCommand *command)
{
	emit errorEncountered(command->lastError());
	command->deleteLater();
}

void AMPIC887Controller::onAsyncCommandSucceeded(AMGCS2AsyncCommand *command)
{
	QString outputText = command->outputString();

	if(!outputText.isEmpty()) {
		emit output(outputText);
	}
	command->deleteLater();
}

void AMPIC887Controller::onErrorClearingTimerTimedOut()
{
	int dummyValue;
	PI_qERR(id_, &dummyValue);
	isBusy_ = false;
}

void AMPIC887Controller::initializeControllerStateData()
{
	AMGCS2InitializeControllerStateCommand initializeCommand =
			AMGCS2InitializeControllerStateCommand(controllerState_);

	runCommand(&initializeCommand);
}

void AMPIC887Controller::runCommand(AMGCS2Command *command)
{
	if(command) {
		command->setController(this);
		command->run();
	}
}





