#include "AMTESTServerConnection.h"

#include "appController/AMDSClientAppController.h"
#include "Connection/AMDSServer.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "ClientRequest/AMDSClientIntrospectionRequest.h"
#include "ClientRequest/AMDSClientContinuousDataRequest.h"
#include "DataElement/AMDSBufferGroupInfo.h"
#include "DataHolder/AMDSScalarDataHolder.h"

#include "util/TimeEvolutionStripTool/AMTESTDataModel.h"
#include "util/TimeEvolutionStripTool/AMTESTSeriesDataModel.h"

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
	removeAllDataModels();
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

			bool isScaler = false;
			QList<AMDSBufferGroupInfo> bufferGroups = introspection->bufferGroupInfos();

			for (int i = 0, size = bufferGroups.size(); i < size; i++){

				AMDSBufferGroupInfo info = bufferGroups.at(i);

				if (info.name().contains("Scaler"))
					isScaler = true;
			}

			// The scaler needs to be handled specifically for the time being.
			if (isScaler)
				configureScaler(introspection);
		}

		break;
	}

	case AMDSClientRequestDefinitions::Continuous:{

		AMDSClientContinuousDataRequest *continuous = qobject_cast<AMDSClientContinuousDataRequest *>(request);

		if (continuous){

			bool isScaler = continuous->bufferName().contains("Scaler");

			if (isScaler)
				retrieveScalerDataFromContinuousRequest(continuous);
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

void AMTESTServerConnection::configureScaler(AMDSClientIntrospectionRequest *introspectionRequest)
{
	// The scaler requires special treament.  But it still creates a list of 1D series data models.
	int numberOfEnabledElements = introspectionRequest->bufferGroupInfos().first().size(0);

	for (int i = 0; i < numberOfEnabledElements; i++){

		AMTESTSeriesDataModel *seriesData = new AMTESTSeriesDataModel(QString("Channel %1").arg(i+1), 5*60, 1000, this);
		dataModels_ << seriesData;
	}

	emit dataModelsCreated(this);
}

void AMTESTServerConnection::removeAllDataModels()
{
	QStringList modelNames;

	foreach (AMTESTDataModel *model, dataModels_){

		modelNames << model->name();
		model->deleteLater();
	}

	dataModels_.clear();
	emit dataModelsDeleted(modelNames);
}

void AMTESTServerConnection::retrieveScalerDataFromContinuousRequest(AMDSClientContinuousDataRequest *continuousDataRequest)
{
	qDebug() << "Retrieving data holders from continuous request: " << continuousDataRequest->data().size() << continuousDataRequest->startTime() ;
	int bufferSize = continuousDataRequest->data().size();

	if (bufferSize > 0){

		QList<QVector<qreal> > enabledChannels;
		AMDSLightWeightScalarDataHolder *scalerDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(continuousDataRequest->data().first());

		if (scalerDataHolder)
			for (int i = 0, size = scalerDataHolder->dataArray().size(); i < size; i++)
				enabledChannels << QVector<qreal>(bufferSize, 0);

		for (int i = 0; i < bufferSize; i++){

			AMDSLightWeightScalarDataHolder *scalerDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(continuousDataRequest->data().at(i));

			if (scalerDataHolder){

				QVector<qint32> currentDataHolder = scalerDataHolder->dataArray().constVectorQint32();

				for (int j = 0, dataHolderSize = currentDataHolder.size(); j < dataHolderSize; j++)
					enabledChannels[j][i] = qreal(currentDataHolder.at(j));
			}
		}

		for (int i = 0, size = enabledChannels.size(); i < size; i++)
			dataModels_.at(i)->addData(enabledChannels.at(i));
	}
}

