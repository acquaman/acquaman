#ifndef AMTESTSERVERCONNECTION_H
#define AMTESTSERVERCONNECTION_H

#include <QObject>

#include <QStringList>

#include "util/TimeEvolutionStripTool/AMTESTServerConfiguration.h"

class AMDSServer;
class AMDSClientRequest;
class AMDSClientIntrospectionRequest;
class AMDSClientContinuousDataRequest;
class AMTESTDataModel;

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
	/// Returns whether the server is active which combines connectedToServer() and whether we have valid data models.
	bool serverIsActive() const { return connectedToServer() && !dataModels_.isEmpty(); }

	/// Convenience getter for the host name.
	QString hostName() const { return serverConfiguration_.hostName(); }
	/// Convenience getter for the port number.
	quint16 portNumber() const { return serverConfiguration_.portNumber(); }

	/// Returns the list of data models.
	QList<AMTESTDataModel *> dataModels() const { return dataModels_; }
	/// Returns the data model at the given index.  Returns 0 if index is out of bounds.
	AMTESTDataModel *dataModelAt(int index) const;
	/// Returns the data model from the given name.  Returns 0 otherwise.
	AMTESTDataModel *dataModelFromName(const QString &name) const;

signals:
	/// Notifier that the server is connecting.
	void serverConnecting();
	/// Notifier that the server was connected.
	void serverConnected();
	/// Notifier that the server was disconnected.
	void serverDisconnected();
	/// Notifier that there was an error.  Passes the error string.
	void serverError(const QString &);
	/// Notifier that data models have been created.
	void dataModelsCreated(AMTESTServerConnection *);
	/// Notifier that the data models have been deleted.  Passes a list of names of models removed.
	void dataModelsDeleted(const QStringList &);

public slots:
	/// Attempts to connect to a data server with the provided configuration.
	void connectToServer();
	/// Disconnects from the data server.
	void disconnectFromServer();
	/// Start continuous data acquisition.  Time is in milliseconds.
	void startContinuousDataRequest(quint64 time);
	/// Stop continuous data acquisition for this connection.
	void stopContinuousDataRequest();

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
	void onServerError(const QString &serverIdentifier, int code, const QString &socketKey, const QString &errorString);

protected:
	/// Adds connections for the scaler (which is specific at the moment).
	void configureScaler(AMDSClientIntrospectionRequest *introspectionRequest);
	/// Adds connections based on the information in the provided introspection request.
	void configureDataModels(AMDSClientIntrospectionRequest *introspectionRequest);
	/// Removes all the data models after the connection has been lost.
	void removeAllDataModels();
	/// Goes through the continuous client data request to get the data for the scaler.
	void retrieveScalerDataFromContinuousRequest(AMDSClientContinuousDataRequest *continuousDataRequest);
	/// Goes through the continuous client data request to get the data from the server.
	void retrieveDataFromContinuousRequest(AMDSClientContinuousDataRequest *continuousDataRequest);
	/// Sends the request to renew the handshake with the continuous data request.
	void sendHandShakeContinuousDataRequest();

	/// Holds the human readable name.
	QString name_;
	/// Holds the server configuration.
	AMTESTServerConfiguration serverConfiguration_;
	/// Holds the last error string.
	QString lastErrorString_;
	/// Holds whether we should be connected or not.
	bool connectedToServer_;
	/// Holds the list of data models that can be visualized by the strip tool.
	QList<AMTESTDataModel *> dataModels_;
};

#endif // AMTESTSERVERCONNECTION_H
