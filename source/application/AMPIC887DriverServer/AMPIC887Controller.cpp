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

void AMPIC887Controller::runCommand(AMGCS2Command *command)
{
	if(command) {
		command->setController(this);
		command->run();
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

	return QString("%1\t\t\t%2\t\t\t%3")
			.arg(name_)
			.arg(hostname_)
			.arg(connectionStatus);
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






