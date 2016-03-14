#ifndef AMTESTSERVERCONFIGURATION_H
#define AMTESTSERVERCONFIGURATION_H

#include <QString>

/// This encapsulates a server configuration that can be connected to specific data servers.
class AMTESTServerConfiguration
{
public:
	/// Constructor.
	AMTESTServerConfiguration();
	/// Constructor.  Sets values based on a server identifier in the form hostName:portNumber.
	AMTESTServerConfiguration(const QString &serverIdentifier);
	/// Constructor.  Can provide a name and port number to configure immediately.
	AMTESTServerConfiguration(const QString &hostName, quint16 portNumber);
	/// Destructor.
	virtual ~AMTESTServerConfiguration();

	/// Returns whether the configuration is valid.  Validity checks to make sure that the name is not empty and port number is greater than 0.
	bool isValid() const;
	/// Returns a string representation of a server identifier used by the data server.
	QString serverIdentifier() const;

	/// Returns the host name for this configuration.
	QString hostName() const { return hostName_; }
	/// Sets the host name to the provided name for this configuration.
	void setHostName(const QString &newHostName) { hostName_ = newHostName; }
	/// Returns the port number for this configuration.
	quint16 portNumber() const { return portNumber_; }
	/// Sets the port number for this configuration.
	void setPortNumber(quint16 newPortNumber) { portNumber_ = newPortNumber; }

	/// The assignment operator.
	AMTESTServerConfiguration &operator =(const AMTESTServerConfiguration &other);
	/// The equivalence operator.
	bool operator ==(const AMTESTServerConfiguration &other);
	/// The not-equivalent operator.
	bool operator !=(const AMTESTServerConfiguration &other);

protected:
	/// Holds the host name.
	QString hostName_;
	/// Holds the port number.
	quint16 portNumber_;
};

#endif // AMTESTSERVERCONFIGURATION_H
