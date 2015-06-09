#include "AMPIC887Controller.h"
#include "PI_GCS2_DLL.h"

AMPIC887Controller::AMPIC887Controller(const QString& name, const QString& hostname)
{
	hostname_ = hostname;
	name_ = name;
	connect();
}

void AMPIC887Controller::runCommand(AMGCS2Command* command)
{
	if(command) {
		command->setControllerId(id_);
		command->run();
	}
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


