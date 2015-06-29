#include "AMPIC887Controller.h"
#include "AMGCS2CommandFactory.h"
#include "PI_GCS2_DLL.h"
#include "GCS2Commands/AMGCS2AsyncCommand.h"

AMPIC887Controller::AMPIC887Controller(const QString& name, const QString& hostname)
{
	hostname_ = hostname;
	name_ = name;
	connect();
}


bool AMPIC887Controller::interpretAndRunCommand(const QString &commandText)
{
	AMGCS2Command* command = AMGCS2CommandFactory::buildCommand(commandText);

	if(command) {

		return runCommand(command);

	} else {
		lastError_ = "Command could not be parsed";
		return false;
	}

	AMGCS2AsyncCommand* asyncCommand = qobject_case<AMGCS2AsyncCommand*>(command);
	if(!asyncCommand) {
		delete command;
	} else {
		asyncCommand->deleteLater();
	}
}

bool AMPIC887Controller::runCommand(AMGCS2Command *command)
{
	bool success = false;

	if(!command) {
		lastError_ = "Cannot run null command.";
		success = false;
	} else {

		runCommand(command);

		success = command->wasSuccessful();

		if(!success) {
			lastError_ = command->lastError();
		} else {
			lastOutputString_ = command->outputString();
		}
	}

	return success;
}

bool AMPIC887Controller::connect()
{
	id_ = PI_ConnectTCPIP(hostname_.toStdString().c_str(), CONTROLLER_PORT);
	return connectionEstablished();
}

void AMPIC887Controller::disconnect()
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

QString AMPIC887Controller::lastError() const
{
	return lastError_;
}

QString AMPIC887Controller::lastOutputString() const
{
	return lastOutputString_;
}



