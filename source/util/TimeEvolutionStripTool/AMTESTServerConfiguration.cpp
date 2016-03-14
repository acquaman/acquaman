#include "AMTESTServerConfiguration.h"

#include <QStringList>

AMTESTServerConfiguration::AMTESTServerConfiguration()
{
	hostName_ = "";
	portNumber_ = -1;
}

AMTESTServerConfiguration::AMTESTServerConfiguration(const QString &serverIdentifier)
{
	QStringList identifierPieces = serverIdentifier.split(":");

	if (identifierPieces.size() == 2){

		hostName_ = identifierPieces.first();
		portNumber_ = quint16(identifierPieces.last().toInt());
	}

	else {

		hostName_ = "";
		portNumber_ = -1;
	}
}

AMTESTServerConfiguration::AMTESTServerConfiguration(const QString &hostName, quint16 portNumber)
{
	hostName_ = hostName;
	portNumber_ = portNumber;
}

AMTESTServerConfiguration::~AMTESTServerConfiguration()
{

}

bool AMTESTServerConfiguration::isValid() const
{
	return !hostName_.isEmpty() && portNumber_ > 0;
}

QString AMTESTServerConfiguration::serverIdentifier() const
{
	if (isValid())
		return QString("%1:%2").arg(hostName_).arg(portNumber_);

	return "";
}

AMTESTServerConfiguration &AMTESTServerConfiguration::operator =(const AMTESTServerConfiguration &other)
{
	hostName_ = other.hostName();
	portNumber_ = other.portNumber();

	return *this;
}

bool AMTESTServerConfiguration::operator ==(const AMTESTServerConfiguration &other)
{
	return hostName_ == other.hostName() && portNumber_ == other.portNumber();
}

bool AMTESTServerConfiguration::operator !=(const AMTESTServerConfiguration &other)
{
	return !this->operator ==(other);
}

