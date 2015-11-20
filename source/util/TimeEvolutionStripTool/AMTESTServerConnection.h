#ifndef AMTESTSERVERCONNECTION_H
#define AMTESTSERVERCONNECTION_H

#include <QObject>

#include "util/TimeEvolutionStripTool/AMTESTServerConfiguration.h"

class AMDSServer;
class AMDSClientRequest;

#define AMTESTSERVERCONNECTION_CONNECTING_TO_SERVER 324100
#define AMTESTSERVERCONNECTION_CONNECTED_TO_SERVER 324101
#define AMTESTSERVERCONNECTION_SERVER_ERROR 324102
#define AMTESTSERVERCONNECTION_DISCONNECTING_FROM_SERVER 324103

/// This class handles the connection between a data server and the time evolution strip tool.
class AMTESTServerConnection : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  Takes a human readable name and server configuration.
	explicit AMTESTServerConnection(const QString &name, const AMTESTServerConfiguration &configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~AMTESTServerConnection();

	/// Returns the name of this connection.
	QString name() const { return name_; }
	/// Returns the server configuration for this connection.
	AMTESTServerConfiguration serverConfiguration() const { return serverConfiguration_; }
	/// Returns whether we should be connected to the server or not.
	bool connectedToServer() const { return connectedToServer_; }

	/// Convenience getter for the host name.
	QString hostName() const { return serverConfiguration_.hostName(); }
	/// Convenience getter for the port number.
	quint16 portNumber() const { return serverConfiguration_.portNumber(); }

signals:
	/// Notifier that the server is connecting.
	void serverConnecting();
	/// Notifier that the server was connected.
	void serverConnected();
	/// Notifier that the server was disconnected.
	void serverDisconnected();
	/// Notifier that there was an error.  Passes the error string.
	void serverError(const QString &);

public slots:
	/// Attempts to connect to a data server with the provided configuration.
	void connectToServer();
	/// Disconnects from the data server.
	void disconnectFromServer();

	/// Convenience setter for the host name.
	void setHostName(const QString &newHostName);
	/// Convenience setter for the port number.
	void setPortNumber(quint16 newPortNumber);

protected slots:
	/// Handles listening to the new server connection signal from the client app controller.
	void onNewServerConnection(const QString &serverIdentifier);
	/// Handles the client data requests.
	void onClientDataRequest(AMDSClientRequest *request);
	/// Handles the server error.
	void onServerError(AMDSServer *server, int code, const QString &socketKey, const QString &errorString);

protected:
	/// Holds the human readable name.
	QString name_;
	/// Holds the server configuration.
	AMTESTServerConfiguration serverConfiguration_;
	/// Holds the last error string.
	QString lastErrorString_;
	/// Holds whether we should be connected or not.
	bool connectedToServer_;
};

#endif // AMTESTSERVERCONNECTION_H
