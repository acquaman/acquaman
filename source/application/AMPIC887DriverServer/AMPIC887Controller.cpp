#include "AMPIC887Controller.h"
#include "AMGCS2CommandFactory.h"
#include "PI_GCS2_DLL.h"
#include "GCS2Commands/AMGCS2AsyncCommand.h"
#include "GCS2Commands/AMGCS2InitializeControllerStateCommand.h"

#include "GCS2Commands/AMGCS2SetCycleTimeCommand.h"
#include "GCS2Commands/AMGCS2SetRecordTriggerSourceCommand.h"
#include "GCS2Commands/AMGCS2SetDataRecorderConfigurationCommand.h"

#include "GCS2Commands/AMGCS2GetCurrentPositionCommand.h"
#include "GCS2Commands/AMGCS2AsyncMoveCommand.h"
#include "GCS2Commands/AMGCS2AsyncMoveRelativeCommand.h"
#include "GCS2Commands/AMGCS2AsyncReferenceMoveCommand.h"
#include "AMGCS2Support.h"

AMPIC887Controller::AMPIC887Controller(const QString& name, const QString& hostname)
	:QObject(0)
{
	hostname_ = hostname;
	name_ = name;
	connectToController();
	isBusy_ = false;
	currentPositionRefreshRequired_ = false;
	controllerState_ = new AMPIC887ControllerState();
	xMotions_ = 0;
	yMotions_ = 0;
	zMotions_ = 0;
	uMotions_ = 0;
	vMotions_ = 0;
	wMotions_ = 0;
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

bool AMPIC887Controller::isInitialized() const
{
	return connectionEstablished() &&
			controllerState_->isAllInitialized();
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

QFlags<AMGCS2::AxisMovementStatus> AMPIC887Controller::movementStatus() const
{
	QFlags<AMGCS2::AxisMovementStatus> movements = QFlags<AMGCS2::AxisMovementStatus>();

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
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain available parameters for an uninitialized controller");
		return QString();
	}

	return controllerState_->availableParameters();
}

AMGCS2::ControllerCommandLevel AMPIC887Controller::commandLevel() const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain command level for an uninitialized controller");
		return AMGCS2::UnknownCommandLevel;
	}

	return controllerState_->currentCommandLevel();
}

double AMPIC887Controller::currentPosition(AMGCS2::Axis axis)
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain current position of an uninitialized controller");
		return 0;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain current position of an unknown axis");
		return 0;
	}

	if(currentPositionRefreshRequired_) {
		AMGCS2GetCurrentPositionCommand currentPositionCommand;
		runCommand(&currentPositionCommand);

		if(currentPositionCommand.runningState() != AMGCS2Command::Succeeded) {
			emit errorEncountered("Cannot obtain current position from controller");
			return 0;
		}

		QHash<AMGCS2::Axis, double> currentPositions =
				currentPositionCommand.axisPositions();

		QList<AMGCS2::Axis> allAxes = currentPositions.keys();

		foreach(AMGCS2::Axis currentAxis, allAxes) {
			controllerState_->hexapodState()->axisState(currentAxis)
					->setCurrentPosition(currentPositions.value(currentAxis));
		}

		if(!isMoving()) {
			currentPositionRefreshRequired_ = false;
		}
	}

	return controllerState_->hexapodState()->axisState(axis)->currentPosition();
}

double AMPIC887Controller::targetPosition(AMGCS2::Axis axis)
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain target position of uninitialized axis");
		return 0;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain target position of unknown axis");
		return 0;
	}

	return controllerState_->hexapodState()->axisState(axis)->targetPosition();
}

double AMPIC887Controller::cycleTime() const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain cycle time of uninitialized controller");
		return 0;
	}

	return controllerState_->hexapodState()->cycleTime();
}

AMGCS2::DataRecordOption AMPIC887Controller::recordOption(int tableId) const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain record option from uninitialized controller");
		return AMGCS2::UnknownRecordOption;
	}

	if(tableId < 1 || tableId > RECORD_TABLE_COUNT) {
		emit errorEncountered(QString("Cannot obtain record option from invalid record table (%1)")
							  .arg(tableId));
		return AMGCS2::UnknownRecordOption;
	}

	return controllerState_->dataRecorderState()->stateAt(tableId)->recordOption();
}

AMGCS2::DataRecordSource AMPIC887Controller::recordSource(int tableId) const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain record source from uninitialized controller");
		return AMGCS2::UnknownDataRecordSource;
	}

	if(tableId < 1 || tableId > RECORD_TABLE_COUNT) {
		emit errorEncountered(QString("Cannot obtain record source from invalid record table (%1)")
							  .arg(tableId));
		return AMGCS2::UnknownDataRecordSource;
	}

	return controllerState_->dataRecorderState()->stateAt(tableId)->recordSource();
}

QString AMPIC887Controller::identificationString() const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain device identification from uninitialized controller");
		return QString();
	}

	return controllerState_->identificationString();
}

double AMPIC887Controller::lowSoftLimit(AMGCS2::Axis axis) const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain low soft limit from uninitialized controller");
		return 0;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain low soft limit of unknown axis");
		return 0;
	}

	return controllerState_->hexapodState()->axisState(axis)->lowSoftLimit();
}

double AMPIC887Controller::highSoftLimit(AMGCS2::Axis axis) const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain high soft limit from uninitialized controller");
		return 0;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain high soft limit of unknown axis");
		return 0;
	}

	return controllerState_->hexapodState()->axisState(axis)->highSoftLimit();
}

bool AMPIC887Controller::softLimitStatus(AMGCS2::Axis axis) const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain soft limit status from uninitialized controller");
		return false;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain soft limit status of unknown axis");
		return false;
	}

	return controllerState_->hexapodState()->axisState(axis)->areSoftLimitsActive();
}

double AMPIC887Controller::minCommandablePosition(AMGCS2::Axis axis) const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain min commandable position from uninitialized controller");
		return 0;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain min commandable position of unknown axis");
		return 0;
	}

	return controllerState_->hexapodState()->axisState(axis)->minCommandablePosition();
}


double AMPIC887Controller::maxCommandablePosition(AMGCS2::Axis axis) const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain min commandable position from uninitialized controller");
		return 0;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain min commandable position of unknown axis");
		return 0;
	}

	return controllerState_->hexapodState()->axisState(axis)->maxCommandablePosition();
}

bool AMPIC887Controller::onTargetState(AMGCS2::Axis axis)
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain on target state from uninitialized controller");
		return 0;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain on target state of unknown axis");
		return 0;
	}

	double currentAxisPosition = currentPosition(axis);
	double currentTargetPosition = targetPosition(axis);

	double epsilon = 0.0001;
	return qAbs(currentAxisPosition - currentTargetPosition) < epsilon;
}

double AMPIC887Controller::pivotPoint(AMGCS2::Axis axis) const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain pivot point from uninitialized controller");
		return 0;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain pivot point of unknown axis");
		return 0;
	}

	if(axis == AMGCS2::UAxis ||
			axis == AMGCS2::VAxis ||
			axis == AMGCS2::WAxis) {

		emit errorEncountered("Cannot obtain pivot point of rotational axis");
		return 0;
	}

	return controllerState_->hexapodState()->axisState(axis)->pivotPoint();
}

AMGCS2::PositionUnits AMPIC887Controller::positionUnits(AMGCS2::Axis axis) const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain position units from uninitialized controller");
		return AMGCS2::UnknownPositionUnit;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain position units of unknown axis");
		return AMGCS2::UnknownPositionUnit;
	}

	return controllerState_->hexapodState()->axisState(axis)->positionUnits();
}

QList<int> AMPIC887Controller::recordedData(int tableId) const
{
	// Todo: need to get size of last recording for this table, load it in
	// another thread, and return this data somehow. This might require us to
	// premtively load the data into the record table by paying attention to
	// the record trigger and filling it up when it's done each time.
	return QList<int>();
}

QString AMPIC887Controller::recorderOptionsString() const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain recorder options from uninitialized controller");
		return QString();
	}

	return controllerState_->dataRecorderState()->recordOptionsString();

}

AMGCS2::DataRecordTrigger AMPIC887Controller::recordTrigger() const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain record trigger from uninitialized controller");
		return AMGCS2::UnknownRecordTrigger;
	}

	return controllerState_->dataRecorderState()->recordTrigger();
}

bool AMPIC887Controller::isAxisReferenced(AMGCS2::Axis axis) const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain referenced state from uninitialized controller");
		return false;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain referenced state of unknown axis");
		return false;
	}

	return controllerState_->hexapodState()->axisState(axis)->isReferenced();
}

bool AMPIC887Controller::areAxesReferenced() const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain referenced state from uninitialized controller");
		return false;
	}

	return controllerState_->hexapodState()->areAllAxesReferenced();
}

bool AMPIC887Controller::isInServoMode() const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain servo mode from uninitialized controller");
		return false;
	}

	return controllerState_->hexapodState()->isInServoMode();
}

double AMPIC887Controller::stepSize(AMGCS2::Axis axis)
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain step size from uninitialized controller");
		return 0;
	}

	if(axis == AMGCS2::UnknownAxis) {
		emit errorEncountered("Cannot obtain step size of unknown axis");
		return 0;
	}

	return controllerState_->hexapodState()->axisState(axis)->stepSize();
}

double AMPIC887Controller::systemVelocity() const
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot obtain system velocity from uninitialized controller");
		return 0;
	}

	return controllerState_->hexapodState()->velocity();
}

void AMPIC887Controller::stop()
{

}

void AMPIC887Controller::haltSmoothly()
{
}

void AMPIC887Controller::move(const QHash<AMGCS2::Axis, double> &axisPositions)
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot move an uninitialized controller");
		return;
	}

	QList<AMGCS2::Axis> axesToMove = axisPositions.keys();
	foreach(AMGCS2::Axis currentAxis, axesToMove) {
		if(!controllerState_->hexapodState()->axisState(currentAxis)->isReferenced()) {
			emit errorEncountered(QString("Cannot move axis %1: Axis not referenced")
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

void AMPIC887Controller::moveRelative(const QHash<AMGCS2::Axis, double> &relativePositions)
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot move an uninitialized controller");
		return;
	}

	QList<AMGCS2::Axis> axesToMove = relativePositions.keys();
	foreach(AMGCS2::Axis currentAxis, axesToMove) {
		if(!controllerState_->hexapodState()->axisState(currentAxis)->isReferenced()) {
			emit errorEncountered(QString("Cannot move axis %1: Axis not referenced")
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

void AMPIC887Controller::referenceMove(const QList<AMGCS2::Axis> &axes)
{
	if(!isInitialized()) {
		emit errorEncountered("Cannot perform reference move on uninitialized controller");
		return;
	}

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

void AMPIC887Controller::onAsyncMoveStarted(AMGCS2AsyncCommand *command)
{
	currentPositionRefreshRequired_ = true;
	AMGCS2AsyncMoveCommand* moveCommand = qobject_cast<AMGCS2AsyncMoveCommand*>(command);

	if(moveCommand) {

		QList<AMGCS2::Axis> axesMoving = moveCommand->targetPositions().keys();
		foreach(AMGCS2::Axis currentAxis, axesMoving) {

			controllerState_->hexapodState()->axisState(currentAxis)->
					setTargetPosition(moveCommand->targetPositions().value(currentAxis));

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
	}
}

void AMPIC887Controller::onAsyncMoveSucceeded(AMGCS2AsyncCommand *command)
{
	AMGCS2AsyncMoveCommand* moveCommand = qobject_cast<AMGCS2AsyncMoveCommand*>(command);

	if(moveCommand) {

		QList<AMGCS2::Axis> axesMoving = moveCommand->targetPositions().keys();
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

	command->deleteLater();
}

void AMPIC887Controller::onAsyncMoveFailed(AMGCS2AsyncCommand *command)
{
	AMGCS2AsyncMoveCommand* moveCommand = qobject_cast<AMGCS2AsyncMoveCommand*>(command);

	if(moveCommand) {

		QList<AMGCS2::Axis> axesMoving = moveCommand->targetPositions().keys();
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

	command->deleteLater();
}

void AMPIC887Controller::onAsyncMoveRelativeStarted(AMGCS2AsyncCommand *command)
{
	currentPositionRefreshRequired_ = true;
	AMGCS2AsyncMoveRelativeCommand* moveRelativeCommand = qobject_cast<AMGCS2AsyncMoveRelativeCommand*>(command);

	if(moveRelativeCommand) {
		QList<AMGCS2::Axis> axesMoving = moveRelativeCommand->relativeTargetPositions().keys();
		foreach(AMGCS2::Axis currentAxis, axesMoving) {

			double currentTarget = controllerState_->hexapodState()->axisState(currentAxis)->targetPosition();
			double relativeTarget = moveRelativeCommand->relativeTargetPositions().value(currentAxis);
			controllerState_->hexapodState()->axisState(currentAxis)->
					setTargetPosition(currentTarget + relativeTarget);

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
	}
}

void AMPIC887Controller::onAsyncMoveRelativeSucceeded(AMGCS2AsyncCommand *command)
{
	AMGCS2AsyncMoveRelativeCommand* moveRelativeCommand = qobject_cast<AMGCS2AsyncMoveRelativeCommand*>(command);

	if(moveRelativeCommand) {

		QList<AMGCS2::Axis> axesMoving = moveRelativeCommand->relativeTargetPositions().keys();
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

	command->deleteLater();
}

void AMPIC887Controller::onAsyncMoveRelativeFailed(AMGCS2AsyncCommand *command)
{
	AMGCS2AsyncMoveRelativeCommand* moveRelativeCommand = qobject_cast<AMGCS2AsyncMoveRelativeCommand*>(command);

	if(moveRelativeCommand) {

		QList<AMGCS2::Axis> axesMoving = moveRelativeCommand->relativeTargetPositions().keys();
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

	controllerState_->hexapodState()->xAxisState()->setIsReferenced(false);
	controllerState_->hexapodState()->yAxisState()->setIsReferenced(false);
	controllerState_->hexapodState()->zAxisState()->setIsReferenced(false);
	controllerState_->hexapodState()->uAxisState()->setIsReferenced(false);
	controllerState_->hexapodState()->vAxisState()->setIsReferenced(false);
	controllerState_->hexapodState()->wAxisState()->setIsReferenced(false);

	controllerState_->hexapodState()->xAxisState()->setTargetPosition(0);
	controllerState_->hexapodState()->yAxisState()->setTargetPosition(0);
	controllerState_->hexapodState()->zAxisState()->setTargetPosition(0);
	controllerState_->hexapodState()->uAxisState()->setTargetPosition(0);
	controllerState_->hexapodState()->vAxisState()->setTargetPosition(0);
	controllerState_->hexapodState()->wAxisState()->setTargetPosition(0);
}

void AMPIC887Controller::onAsyncReferenceMoveSucceeded(AMGCS2AsyncCommand *command)
{
	--xMotions_;
	--yMotions_;
	--zMotions_;
	--uMotions_;
	--vMotions_;
	--wMotions_;

	controllerState_->hexapodState()->xAxisState()->setIsReferenced(true);
	controllerState_->hexapodState()->yAxisState()->setIsReferenced(true);
	controllerState_->hexapodState()->zAxisState()->setIsReferenced(true);
	controllerState_->hexapodState()->uAxisState()->setIsReferenced(true);
	controllerState_->hexapodState()->vAxisState()->setIsReferenced(true);
	controllerState_->hexapodState()->wAxisState()->setIsReferenced(true);

	command->deleteLater();
}

void AMPIC887Controller::onAsyncReferenceMoveFailed(AMGCS2AsyncCommand *command)
{
	--xMotions_;
	--yMotions_;
	--zMotions_;
	--uMotions_;
	--vMotions_;
	--wMotions_;

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
