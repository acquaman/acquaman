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

#include "GCS2Commands/AMGCS2StopCommand.h"
#include "GCS2Commands/AMGCS2HaltSmoothlyCommand.h"

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
	return controllerState_->availableParameters();
}

AMGCS2::ControllerCommandLevel AMPIC887Controller::commandLevel() const
{
	return controllerState_->currentCommandLevel();
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

	return controllerState_->hexapodState()->currentPositions().values(axes);
}

double AMPIC887Controller::targetPosition(AMGCS2::Axis axis)
{
	return controllerState_->hexapodState()->targetPosition(axis);
}

AMPIC887AxisMap<double> AMPIC887Controller::targetPositions(const AMPIC887AxisCollection& axes)
{
	return controllerState_->hexapodState()->targetPositions().values(axes);
}

double AMPIC887Controller::cycleTime() const
{
	return controllerState_->hexapodState()->cycleTime();
}

AMGCS2::DataRecordOption AMPIC887Controller::recordOption(int tableId) const
{
	return controllerState_->dataRecorderState()->stateAt(tableId)->recordOption();
}

AMGCS2::DataRecordSource AMPIC887Controller::recordSource(int tableId) const
{
	return controllerState_->dataRecorderState()->stateAt(tableId)->recordSource();
}

QString AMPIC887Controller::identificationString() const
{
	return controllerState_->identificationString();
}

double AMPIC887Controller::lowSoftLimit(AMGCS2::Axis axis) const
{
	return controllerState_->hexapodState()->lowSoftLimit(axis);
}

AMPIC887AxisMap<double> AMPIC887Controller::lowSoftLimits(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->lowSoftLimits().values(axes);
}

double AMPIC887Controller::highSoftLimit(AMGCS2::Axis axis) const
{
	return controllerState_->hexapodState()->highSoftLimit(axis);
}

AMPIC887AxisMap<double> AMPIC887Controller::highSoftLimits(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->highSoftLimits().values(axes);
}

bool AMPIC887Controller::softLimitStatus(AMGCS2::Axis axis) const
{
	return controllerState_->hexapodState()->softLimitState(axis);
}

AMPIC887AxisMap<bool> AMPIC887Controller::softLimitStatuses(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->softLimitStates().values(axes);
}

double AMPIC887Controller::minCommandablePosition(AMGCS2::Axis axis) const
{
	return controllerState_->hexapodState()->minCommandablePosition(axis);
}

AMPIC887AxisMap<double> AMPIC887Controller::minCommandablePositions(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->minCommandablePositions().values(axes);
}

double AMPIC887Controller::maxCommandablePosition(AMGCS2::Axis axis) const
{
	return controllerState_->hexapodState()->maxCommandablePosition(axis);
}

AMPIC887AxisMap<double> AMPIC887Controller::maxCommandablePositions(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->maxCommandablePositions().values(axes);
}

bool AMPIC887Controller::onTargetState(AMGCS2::Axis axis)
{
	double currentAxisPosition = currentPosition(axis);
	double currentTargetPosition = targetPosition(axis);

	double epsilon = 0.0001;
	return qAbs(currentAxisPosition - currentTargetPosition) < epsilon;
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

AMPIC887AxisMap<double> AMPIC887Controller::pivotPoints(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->pivotPoints().values(axes);
}

AMGCS2::PositionUnits AMPIC887Controller::positionUnits(AMGCS2::Axis axis) const
{
	return controllerState_->hexapodState()->positionUnits(axis);
}

AMPIC887AxisMap<AMGCS2::PositionUnits> AMPIC887Controller::positionUnits(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->positionUnits().values(axes);
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
	return controllerState_->dataRecorderState()->recordOptionsString();
}

AMGCS2::DataRecordTrigger AMPIC887Controller::recordTrigger() const
{
	return controllerState_->dataRecorderState()->recordTrigger();
}

bool AMPIC887Controller::isAxisReferenced(AMGCS2::Axis axis) const
{
	return controllerState_->hexapodState()->referencedState(axis);
}

AMPIC887AxisMap<bool> AMPIC887Controller::axisReferencedStates(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->referencedStates().values(axes);
}

bool AMPIC887Controller::areAxesReferenced() const
{
	return controllerState_->hexapodState()->areAllAxesReferenced();
}

bool AMPIC887Controller::isInServoMode() const
{
	return controllerState_->hexapodState()->isInServoMode();
}

double AMPIC887Controller::stepSize(AMGCS2::Axis axis)
{
	return controllerState_->hexapodState()->stepSize(axis);
}

AMPIC887AxisMap<double> AMPIC887Controller::stepSizes(const AMPIC887AxisCollection &axes) const
{
	return controllerState_->hexapodState()->stepSizes().values(axes);
}

double AMPIC887Controller::systemVelocity() const
{
	return controllerState_->hexapodState()->velocity();
}

void AMPIC887Controller::stop()
{
	AMGCS2StopCommand stopCommand;
	runCommand(&stopCommand);

	if(stopCommand.runningState() != AMGCS2Command::Succeeded) {

		emit errorEncountered(QString("Failed to stop controller with message: %1")
							  .arg(stopCommand.lastError()));
	}
}

void AMPIC887Controller::haltSmoothly(const AMPIC887AxisCollection& axes)
{
	AMGCS2HaltSmoothlyCommand haltCommand(axes);
	runCommand(&haltCommand);

	if(haltCommand.runningState() != AMGCS2Command::Succeeded) {
		emit errorEncountered(QString("Failed to halt controller with message: %1")
							  .arg(haltCommand.lastError()));
	}
}

void AMPIC887Controller::move(const QHash<AMGCS2::Axis, double> &axisPositions)
{
	QList<AMGCS2::Axis> axesToMove = axisPositions.keys();
	foreach(AMGCS2::Axis currentAxis, axesToMove) {
		if(!controllerState_->hexapodState()->referencedState(currentAxis)) {
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
	QList<AMGCS2::Axis> axesToMove = relativePositions.keys();
	foreach(AMGCS2::Axis currentAxis, axesToMove) {
		if(!controllerState_->hexapodState()->referencedState(currentAxis)) {
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
}

void AMPIC887Controller::onAsyncReferenceMoveSucceeded(AMGCS2AsyncCommand *command)
{
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
	updateStateOnStop();
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

	// Set target position = current position
	AMPIC887AxisCollection allAxes;
	foreach(AMGCS2::Axis currentAxis, allAxes) {
		double currentPosition = controllerState_->hexapodState()->currentPosition(currentAxis);

		controllerState_->hexapodState()->setTargetPosition(currentAxis, currentPosition);
	}

}



