#include "AMTESTServerConnection.h"

#include "appController/AMDSClientAppController.h"
#include "Connection/AMDSServer.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "ClientRequest/AMDSClientIntrospectionRequest.h"
#include "ClientRequest/AMDSClientContinuousDataRequest.h"
#include "DataElement/AMDSBufferGroupInfo.h"
#include "DataHolder/AMDSScalarDataHolder.h"
#include "DataHolder/AMDSSpectralDataHolder.h"

#include "util/TimeEvolutionStripTool/AMTESTDataModel.h"
#include "util/TimeEvolutionStripTool/AMTESTSeriesDataModel.h"
#include "util/TimeEvolutionStripTool/AMTESTImageDataModel.h"

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

AMTESTDataModel *AMTESTServerConnection::dataModelAt(int index) const
{
	if (index >= 0 && index < dataModels_.size())
		return dataModels_.at(index);

	return 0;
}

AMTESTDataModel *AMTESTServerConnection::dataModelFromName(const QString &name) const
{
	foreach (AMTESTDataModel *dataModel, dataModels_)
		if (dataModel->name() == name)
			return dataModel;

	return 0;
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

void AMTESTServerConnection::startContinuousDataRequest(quint64 time)
{
	foreach (AMTESTDataModel *dataModel, dataModels_)
		dataModel->clear();

	AMDSClientAppController *client = AMDSClientAppController::clientAppController();
	client->requestClientData(serverConfiguration_.hostName(),
				  serverConfiguration_.portNumber(),
				  client->getBufferNamesByServer(serverConfiguration_.serverIdentifier()),
				  time);
}

void AMTESTServerConnection::stopContinuousDataRequest()
{
	AMDSClientAppController *client = AMDSClientAppController::clientAppController();

	QStringList socketKeys = client->getActiveSocketKeysByServer(serverConfiguration_.serverIdentifier());

	if (!socketKeys.isEmpty()){

		client->requestClientData(serverConfiguration_.hostName(),
					  serverConfiguration_.portNumber(),
					  QStringList(),
					  0,
					  false,
					  false,
					  socketKeys.first());
	}
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
		connect(server, SIGNAL(AMDSServerError(QString,int,QString,QString)), this, SLOT(onServerError(QString,int,QString,QString)));
		emit serverConnected();
	}
}

void AMTESTServerConnection::onClientDataRequest(AMDSClientRequest *request)
{
	switch(request->requestType()){

	case AMDSClientRequestDefinitions::Introspection:{

		AMDSClientIntrospectionRequest *introspectionRequest = qobject_cast<AMDSClientIntrospectionRequest *>(request);

		if (introspectionRequest){

			bool isScaler = false;
			QList<AMDSBufferGroupInfo> bufferGroups = introspectionRequest->bufferGroupInfos();

			for (int i = 0, size = bufferGroups.size(); i < size; i++){

				AMDSBufferGroupInfo info = bufferGroups.at(i);

				if (info.name().contains("Scaler"))
					isScaler = true;
			}

			// The scaler needs to be handled specifically for the time being.
			if (isScaler)
				configureScaler(introspectionRequest);

			else
				configureDataModels(introspectionRequest);
		}

		break;
	}

	case AMDSClientRequestDefinitions::Continuous:{

		AMDSClientContinuousDataRequest *continuousRequest = qobject_cast<AMDSClientContinuousDataRequest *>(request);

		if (continuousRequest){

			bool isScaler = continuousRequest->bufferName().contains("Scaler");

			if (isScaler)
				retrieveScalerDataFromContinuousRequest(continuousRequest);

			else
				retrieveDataFromContinuousRequest(continuousRequest);

			sendHandShakeContinuousDataRequest();
		}

		break;
	}

	default:
		break;
	}

	request->deleteLater();
}

void AMTESTServerConnection::onServerError(const QString &serverIdentifier, int code, const QString &socketKey, const QString &errorString)
{
	Q_UNUSED(serverIdentifier)
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

		AMTESTSeriesDataModel *seriesData = new AMTESTSeriesDataModel(QString("Channel %1").arg(i), 5*60, 1000, this);
		dataModels_ << seriesData;
	}

	emit dataModelsCreated(this);
}

void AMTESTServerConnection::configureDataModels(AMDSClientIntrospectionRequest *introspectionRequest)
{
	foreach (AMDSBufferGroupInfo info, introspectionRequest->bufferGroupInfos()){

		if (info.rank() == 0){

//			AMTESTSeriesDataModel *seriesData = new AMTESTSeriesDataModel(QString("Channel %1").arg(i), 5*60, 1000, this);
//			dataModels_ << seriesData;
		}

		if (info.rank() == 1){

			AMTESTImageDataModel *imageData = new AMTESTImageDataModel(info.name(), 5*60, 20, info.axes().first().size(), this);
			dataModels_ << imageData;
		}
	}

	emit dataModelsCreated(this);
}

void AMTESTServerConnection::removeAllDataModels()
{
	QStringList modelNames;

	foreach (AMTESTDataModel *model, dataModels_){

		modelNames << model->name();
		model->clear();
		model->deleteLater();
	}

	dataModels_.clear();
	emit dataModelsDeleted(modelNames);
}

void AMTESTServerConnection::retrieveScalerDataFromContinuousRequest(AMDSClientContinuousDataRequest *continuousDataRequest)
{
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

void AMTESTServerConnection::retrieveDataFromContinuousRequest(AMDSClientContinuousDataRequest *continuousDataRequest)
{
	int bufferSize = continuousDataRequest->data().size();

	if (bufferSize > 0){

		foreach (AMDSDataHolder *dataHolder, continuousDataRequest->data()){

			AMDSLightWeightSpectralDataHolder *spectrumDataHolder = qobject_cast<AMDSLightWeightSpectralDataHolder *>(dataHolder);

			if (spectrumDataHolder){

				QVector<qreal> currentDataHolder = spectrumDataHolder->dataArray().asConstVectorDouble();
				dataModelFromName(continuousDataRequest->bufferName())->addData(currentDataHolder);
			}
		}
	}
}

void AMTESTServerConnection::sendHandShakeContinuousDataRequest()
{
	AMDSClientAppController *client = AMDSClientAppController::clientAppController();
	QString handShakeSocketKey = client->getActiveSocketKeysByServer(serverConfiguration_.serverIdentifier()).first();
	client->requestClientData(serverConfiguration_.hostName(),
				  serverConfiguration_.portNumber(),
				  client->getBufferNamesByServer(serverConfiguration_.serverIdentifier()),
				  0,
				  false,
				  false,
				  handShakeSocketKey);
}

