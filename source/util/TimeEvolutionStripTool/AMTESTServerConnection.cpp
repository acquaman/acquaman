#include "AMTESTServerConnection.h"

#include "appController/AMDSClientAppController.h"
#include "Connection/AMDSServer.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "ClientRequest/AMDSClientIntrospectionRequest.h"
#include "DataElement/AMDSBufferGroupInfo.h"

#include "util/AMErrorMonitor.h"

AMTESTServerConnection::AMTESTServerConnection(const QString &name, const AMTESTServerConfiguration &configuration, QObject *parent)
	: QObject(parent)
{
	name_ = name;
	serverConfiguration_ = configuration;
	lastErrorString_ = "";
	connectedToServer_ = false;

	connect(AMDSClientAppController::clientAppController(), SIGNAL(newServerConnected(QString)), this, SLOT(onNewServerConnection(QString)));
}

AMTESTServerConnection::~AMTESTServerConnection()
{

}

void AMTESTServerConnection::connectToServer()
{
	AMErrorMon::information(this, AMTESTSERVERCONNECTION_CONNECTING_TO_SERVER, QString("Connecting to the %1 server...").arg(name_));
	emit serverConnecting();
	connectedToServer_ = true;
	AMDSClientAppController::clientAppController()->connectToServer(serverConfiguration_.hostName(), serverConfiguration_.portNumber());
}

void AMTESTServerConnection::disconnectFromServer()
{
	AMErrorMon::information(this, AMTESTSERVERCONNECTION_DISCONNECTING_FROM_SERVER, QString("Disconnecting from the %1 server...").arg(name_));
	AMDSClientAppController::clientAppController()->disconnectWithServer(serverConfiguration_.serverIdentifier());
	connectedToServer_ = false;
	emit serverDisconnected();
}

void AMTESTServerConnection::setHostName(const QString &newHostName)
{
	serverConfiguration_.setHostName(newHostName);
}

void AMTESTServerConnection::setPortNumber(quint16 newPortNumber)
{
	serverConfiguration_.setPortNumber(newPortNumber);
}

void AMTESTServerConnection::onNewServerConnection(const QString &serverIdentifier)
{
	AMTESTServerConfiguration newConnection(serverIdentifier);

	if (serverConfiguration_ == newConnection){

		AMErrorMon::information(this, AMTESTSERVERCONNECTION_CONNECTED_TO_SERVER, QString("Connected to the %1 server...").arg(name_));
		AMDSServer *server = AMDSClientAppController::clientAppController()->getServerByServerIdentifier(serverIdentifier);
		connect(server, SIGNAL(requestDataReady(AMDSClientRequest*)), this, SLOT(onClientDataRequest(AMDSClientRequest*)));
		connect(server, SIGNAL(AMDSServerError(AMDSServer*,int,QString,QString)), this, SLOT(onServerError(AMDSServer*,int,QString,QString)));
		emit serverConnected();
	}
}

void AMTESTServerConnection::onClientDataRequest(AMDSClientRequest *request)
{
	switch(request->requestType()){

	case AMDSClientRequestDefinitions::Introspection:{

		AMDSClientIntrospectionRequest *introspection = qobject_cast<AMDSClientIntrospectionRequest *>(request);

		if (introspection){

			QList<AMDSBufferGroupInfo> bufferGroups = introspection->bufferGroupInfos();

			for (int i = 0, size = bufferGroups.size(); i < size; i++)
				qDebug() << bufferGroups.at(i).name();
		}
		break;
	}
	default:
		break;
	}
}

void AMTESTServerConnection::onServerError(AMDSServer *server, int code, const QString &socketKey, const QString &errorString)
{
	Q_UNUSED(server)
	Q_UNUSED(code)
	Q_UNUSED(socketKey)

	lastErrorString_ = errorString;
	AMErrorMon::alert(this, AMTESTSERVERCONNECTION_SERVER_ERROR, QString("%1 had the following error: (%2)").arg(name_).arg(lastErrorString_));
	emit serverError(lastErrorString_);
}

