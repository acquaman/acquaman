#include "AMPIC887Controller.h"
#include "AMGCS2CommandFactory.h"
#include "PI_GCS2_DLL.h"
#include "GCS2Commands/AMGCS2AsyncCommand.h"
#include "GCS2Commands/AMGCS2InitializeControllerStateCommand.h"

AMPIC887Controller::AMPIC887Controller(const QString& name, const QString& hostname)
	:QObject(0)
{
	hostname_ = hostname;
	name_ = name;
	connectToController();
	isBusy_ = false;

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
}

void AMPIC887Controller::referenceMove(const QList<AMGCS2::Axis> &axes)
{
}

void AMPIC887Controller::moveRelative(const QHash<AMGCS2::Axis, double> &axisRelativePositions)
{
}

double AMPIC887Controller::targetPosition(AMGCS2::Axis axis) const
{
	return 0;
}

double AMPIC887Controller::currentPosition(AMGCS2::Axis axis) const
{
	return 0;
}

double AMPIC887Controller::cycleTime() const
{
	return 0;
}

void AMPIC887Controller::setCycleTime(double cycleTime)
{
}

AMGCS2::DataRecordTrigger AMPIC887Controller::recordTrigger() const
{
	return AMGCS2::UnknownRecordTrigger;
}

void AMPIC887Controller::setRecordTrigger(AMGCS2::DataRecordTrigger trigger)
{

}

AMGCS2::DataRecordSource AMPIC887Controller::recordSource(int tableId) const
{
	return AMGCS2::UnknownDataRecordSource;
}

void AMPIC887Controller::setRecordSource(int tableId, AMGCS2::DataRecordSource source)
{
}

AMGCS2::DataRecordOption AMPIC887Controller::recordOption(int tableId) const
{
	return AMGCS2::UnknownRecordOption;
}

void AMPIC887Controller::setRecordOption(int tableId, AMGCS2::DataRecordOption option)
{
}

double AMPIC887Controller::lowSoftLimit(AMGCS2::Axis axis) const
{
	return 0;
}

void AMPIC887Controller::setLowSoftLimit(AMGCS2::Axis axis, double lowSoftLimit)
{
}

double AMPIC887Controller::highSoftLimit(AMGCS2::Axis axis) const
{
	return 0;
}

void AMPIC887Controller::setHighSoftLimit(AMGCS2::Axis axis, double highSoftLimit)
{
}

bool AMPIC887Controller::areSoftLimitsActive(AMGCS2::Axis axis) const
{
	return false;
}

void AMPIC887Controller::setSoftLimitsActive(AMGCS2::Axis axis, bool active)
{
}

double AMPIC887Controller::minCommandablePosition(AMGCS2::Axis axis) const
{
	return 0;
}

double AMPIC887Controller::maxCommandablePosition(AMGCS2::Axis axis) const
{
	return 0;
}

QFlags<AMGCS2::AxisMovementStatus> AMPIC887Controller::movingStatuses() const
{
	return QFlags<AMGCS2::AxisMovementStatus>();
}

double AMPIC887Controller::pivotPoint(AMGCS2::Axis axis) const
{
	return 0;
}

void AMPIC887Controller::setPivotPoint(AMGCS2::Axis axis, double pivotPoint) const
{
}

QList<int> AMPIC887Controller::recordedData(int tableId, int offset, int count)
{
	return QList<int>();
}

bool AMPIC887Controller::isAxisReferenced(AMGCS2::Axis axis) const
{
	return false;
}

bool AMPIC887Controller::areAllAxesReferenced() const
{
	return false;
}

bool AMPIC887Controller::isInServoMode() const
{
	return false;
}

void AMPIC887Controller::setServoMode(bool servoModeActive)
{
}

double AMPIC887Controller::stepSize(AMGCS2::Axis axis) const
{
	return 0;
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





