#include "CLSAMDSScaler.h"

#include "beamline/AMPVControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMDetectorTriggerSource.h"
#include "beamline/AMCurrentAmplifier.h"
#include "source/beamline/CLS/CLSSIS3820ScalerAcquisitionMode.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMControlWaitAction.h"
#include "util/AMErrorMonitor.h"

#include <QStringBuilder>

#include "actions3/actions/CLSSIS3820ScalerDarkCurrentMeasurementAction.h"

#include "source/appController/AMDSClientAppController.h"
#include "source/ClientRequest/AMDSClientRequestSupport.h"
#include "source/DataElement/AMDSEventDataSupport.h"
#include "source/DataHolder/AMDSDataHolderSupport.h"
#include "source/Connection/AMDSServer.h"
#include "source/ClientRequest/AMDSClientDataRequest.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "source/DataHolder/AMDSScalarDataHolder.h"


// CLSSIS3820Scalar
/////////////////////////////////////////////

CLSAMDSScaler::CLSAMDSScaler(const QString &baseName, const QString &amdsBufferName, QObject *parent) :
	QObject(parent)
{
	connectedOnce_ = false;

	triggerSourceTriggered_ = false;
	triggerSource_ = new AMDetectorTriggerSource(QString("%1TriggerSource").arg(baseName), this);
	connect(triggerSource_, SIGNAL(triggered(AMDetectorDefinitions::ReadMode)), this, SLOT(onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode)));

	dwellTimeSourceRequested_ = false;
	dwellTimeSource_ = new AMDetectorDwellTimeSource(QString("%1DwellTimeSource").arg(baseName), this);
	connect(dwellTimeSource_, SIGNAL(setDwellTime(double)), this, SLOT(onDwellTimeSourceSetDwellTime(double)));

	CLSAMDSScalerChannel *tmpChannel;
	for (int x = 0; x < 32; x++) {
		tmpChannel = new CLSAMDSScalerChannel(baseName, x, this);
		scalerChannels_.append(tmpChannel);
		connect(tmpChannel, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
		connect(tmpChannel, SIGNAL(sensitivityChanged()), this, SIGNAL(sensitivityChanged()));
	}

	startControl_ = new AMSinglePVControl("Start", QString("%1:start").arg(baseName),this, 0.5);
	dwellTimeControl_ = new AMSinglePVControl("DwellTime", QString("%1:dwellTime").arg(baseName), this, 0.05);
	dwellModeControl_ = new AMSinglePVControl("DwellMode", QString("%1:dwellMode").arg(baseName), this, 0.5);
	dwellStateControl_ = new AMSinglePVControl("DwellState", QString("%1:dwellState").arg(baseName), this, 0.5);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(startControl_);
	allControls_->addControl(dwellTimeControl_);
	allControls_->addControl(dwellModeControl_);
	allControls_->addControl(dwellStateControl_);

	amdsBufferName_ = amdsBufferName;
	AMDSClientRequestSupport::registerClientRequestClass();
	AMDSDataHolderSupport::registerDataHolderClass();
	AMDSEventDataSupport::registerEventDataObjectClass();
	lastContinuousDataRequest_ = 0;
	continuousDataWindowSeconds_ = 20;
	pollingRateMilliSeconds_ = 1;

	connect(startControl_, SIGNAL(valueChanged(double)), this, SLOT(onStartControlValueChanged(double)));
	connect(dwellTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onDwellTimeControlValueChanged(double)));
	connect(dwellModeControl_, SIGNAL(valueChanged(double)), this, SLOT(onDwellModeControlValueChanged(double)));
	connect(dwellStateControl_, SIGNAL(valueChanged(double)), this, SLOT(onDwellStateControlValueChanged(double)));

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
}

CLSAMDSScaler::~CLSAMDSScaler(){}

bool CLSAMDSScaler::isConnected() const{

	bool retVal = true;

	for(int x = 0; x < scalerChannels_.count(); x++)
		retVal &= scalerChannels_.at(x)->isConnected();

	return retVal && allControls_->isConnected();
}

QString CLSAMDSScaler::amdsBufferName() const
{
	return amdsBufferName_;
}

void CLSAMDSScaler::configAMDSServer(const QString &amdsServerIdentifier)
{
	amdsServerIdentifier_ = amdsServerIdentifier;

	qDebug() << "Looking for AMDS_SCALER server by identifier " << amdsServerIdentifier;
	AMDSServer *scalerAMDSServer = AMDSClientAppController::clientAppController()->getServerByServerIdentifier(amdsServerIdentifier_);
	if (scalerAMDSServer) {
		qDebug() << "AMDS_SCALER Returned a valid server, connect to it " << scalerAMDSServer->hostName();
		connect(scalerAMDSServer, SIGNAL(requestDataReady(AMDSClientRequest*)), this, SLOT(onRequestDataReady(AMDSClientRequest*)));
		connect(AMDSClientAppController::clientAppController(), SIGNAL(serverError(int,bool,QString,QString)), this, SLOT(onServerError(int,bool,QString,QString)));
	}
}

bool CLSAMDSScaler::retrieveBufferedData(double /*seconds*/)
{
	AMDSClientAppController *clientAppController = AMDSClientAppController::clientAppController();
	AMDSServer *scalerAMDSServer = clientAppController->getServerByServerIdentifier(amdsServerIdentifier_);
	if (scalerAMDSServer && !amdsBufferName_.isEmpty() && scalerAMDSServer->bufferNames().contains(amdsBufferName_)){
		double dataRequestSize = continuousDataWindowSeconds_*1000/((double)pollingRateMilliSeconds_);
		qDebug() << "AMDS_SCALER calculated data request of size " << dataRequestSize;

		return clientAppController->requestClientData(scalerAMDSServer->hostName(), scalerAMDSServer->portNumber(), amdsBufferName_, quint64(dataRequestSize), quint64(dataRequestSize), true, false);
	}
	else
		return false;
}

AMDSClientRelativeCountPlusCountDataRequest* CLSAMDSScaler::lastContinuousDataRequest() const
{
	return lastContinuousDataRequest_;
}

bool CLSAMDSScaler::setContinuousDataWindow(double continuousDataWindowSeconds)
{
	continuousDataWindowSeconds_ = continuousDataWindowSeconds;
	return true;
}

int CLSAMDSScaler::amdsPollingBaseTimeMilliseconds() const
{
	return pollingRateMilliSeconds_;
}

bool CLSAMDSScaler::isStarted() const{

	return isConnected() && startControl_->withinTolerance(1);
}

double CLSAMDSScaler::dwellMode() const
{
	if(isConnected())
		return dwellModeControl_->value();
	return -1;
}

double CLSAMDSScaler::dwellTime() const
{
	if(isConnected())
		return dwellTimeControl_->value();

	return -1;
}

double CLSAMDSScaler::dwellTimeTolerance() const
{
	if (isConnected())
		return dwellTimeControl_->tolerance();

	return -1;
}

double CLSAMDSScaler::dwellState() const
{
	if(isConnected())
		return dwellStateControl_->value();
	return -1;
}

int CLSAMDSScaler::enabledChannelCount() const{
	int retVal = 0;
	for(int x = 0; x < scalerChannels_.count(); x++)
		if(scalerChannels_.at(x)->isEnabled())
			retVal++;

	return retVal;
}

CLSAMDSScalerChannel* CLSAMDSScaler::channelAt(int index) const{
	return scalerChannels_.at(index);
}

AMOrderedList<CLSAMDSScalerChannel*> CLSAMDSScaler::channels() const{
	return scalerChannels_;
}

AMDetectorTriggerSource* CLSAMDSScaler::triggerSource() const{
	return triggerSource_;
}

AMDetectorDwellTimeSource* CLSAMDSScaler::dwellTimeSource() const{
	return dwellTimeSource_;
}

AMAction3* CLSAMDSScaler::createStartAction3(bool setStarted){
	if(!isConnected())
		return 0; //NULL

	AMAction3 *action = AMActionSupport::buildControlMoveAction(startControl_, setStarted ? 1 : 0);

	if(!action)
		return 0; //NULL

	return action;
}

AMAction3* CLSAMDSScaler::createDwellModeAction3(double dwellMode){
	if(!isConnected())
		return 0; //NULL

	AMAction3 *action = AMActionSupport::buildControlMoveAction(dwellModeControl_, dwellMode);
	if(!action)
		return 0; //NULL

	return action;
}

AMAction3* CLSAMDSScaler::createDwellTimeAction3(double dwellTime)
{
	if(!isConnected())
		return 0; //NULL

	AMAction3 *action = 0;

	if (!dwellTimeControl_->withinTolerance(dwellTime))
		action = AMActionSupport::buildControlMoveAction(dwellTimeControl_, dwellTime);

	return action;
}


AMAction3* CLSAMDSScaler::createWaitForDwellFinishedAction(double timeoutTime)
{
	if(!isConnected())
		return 0; //NULL

	AMControlInfo setpoint = dwellStateControl_->toInfo();
	setpoint.setValue(0);
	AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(setpoint, timeoutTime , AMControlWaitActionInfo::MatchEqual);
	AMControlWaitAction *action = new AMControlWaitAction(actionInfo, dwellStateControl_);

	if(!action)
		return 0; //NULL

	return action;
}

AMAction3* CLSAMDSScaler::createMeasureDarkCurrentAction(int secondsDwell)
{
	Q_UNUSED(secondsDwell)
	return 0;
//	return new CLSAMDSScalerDarkCurrentMeasurementAction(new CLSAMDSScalerDarkCurrentMeasurementActionInfo(secondsDwell));
}

QMap<QString, QVector<qint32> > CLSAMDSScaler::retrieveScalerData(const QMap<int, QString> &scalerChannelIndexMap, AMDSClientDataRequest *scalerClientDataRequest)
{
	QMap<QString, QVector<qint32> > retVal;

	AMDSLightWeightScalarDataHolder *asScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(scalerClientDataRequest->data().at(0));
	if(!asScalarDataHolder)
		return retVal;

	int scalerDataCount = scalerClientDataRequest->data().count();
	QMap<int, QString>::const_iterator i = scalerChannelIndexMap.constBegin();
	while(i != scalerChannelIndexMap.constEnd()){
		retVal.insert(i.value(), QVector<qint32>(scalerDataCount));
		i++;
	}

	for(int x = 0; x < scalerDataCount; x++){
		asScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(scalerClientDataRequest->data().at(x));
		if(asScalarDataHolder){
			QVector<qint32> oneVector = asScalarDataHolder->dataArray().constVectorQint32();
			for(int y = 0, ySize = oneVector.count(); y < ySize; y++)
				if(scalerChannelIndexMap.contains(y))
					(retVal[scalerChannelIndexMap.value(y)])[x] = oneVector.at(y);
		}
	}

	return retVal;
}

QMap<QString, QVector<qint32> > CLSAMDSScaler::rebaseScalerData(const QMap<QString, QVector<qint32> > &scalerData, int baseTimeScale)
{
	QMap<QString, QVector<qint32> > scalerChannelRebaseVectors;
	int scalerTotalCount = scalerData.value(scalerData.keys().first()).count();
	int baseScalerTimeScale = amdsPollingBaseTimeMilliseconds();
	int rebasedTotalCount = (scalerTotalCount*baseScalerTimeScale)/baseTimeScale;
	qDebug() << "Original totalCount " << scalerTotalCount << " rebasedTotalCount " << rebasedTotalCount;

	QMap<QString, qint32> scalerChannelRunningSums;
	QMap<QString, QVector<qint32> >::const_iterator i = scalerData.constBegin();
	while(i != scalerData.constEnd()){
		scalerChannelRebaseVectors.insert(i.key(), QVector<qint32>(rebasedTotalCount, 0));
		scalerChannelRunningSums.insert(i.key(), 0);
		i++;
	}

	for(int x = 0; x < scalerTotalCount; x++){
		int tempRunningSum;
		QString channelString;
		QMap<QString, QVector<qint32> >::const_iterator j = scalerData.constBegin();
		while(j != scalerData.constEnd()){
			channelString = j.key();
			tempRunningSum = scalerChannelRunningSums.value(channelString);
			tempRunningSum += (scalerData[channelString]).at(x);
			scalerChannelRunningSums[channelString] = tempRunningSum;

			if( (((x+1)*baseScalerTimeScale) % baseTimeScale) == 0){
				int rebaseIndex = (x*baseScalerTimeScale)/baseTimeScale;
				(scalerChannelRebaseVectors[channelString])[rebaseIndex] = scalerChannelRunningSums.value(channelString);
				scalerChannelRunningSums[channelString] = 0;
			}

			j++;
		}
	}

	return scalerChannelRebaseVectors;
}

void CLSAMDSScaler::setStarted(bool start){

	if(!isConnected())
		return;

	if(start && startControl_->withinTolerance(0))
		startControl_->move(1);

	else if(!start && startControl_->withinTolerance(1))
		startControl_->move(0);
}

void CLSAMDSScaler::setDwellMode(double dwellMode){

	if(!isConnected())
		return;

	if(!dwellModeControl_->withinTolerance(dwellMode))
		dwellModeControl_->move(dwellMode);
}

void CLSAMDSScaler::setDwellTime(double dwellTime){

	if(!isConnected())
		return;
	if(!dwellTimeControl_->withinTolerance(dwellTime))
		dwellTimeControl_->move(dwellTime);
}

void CLSAMDSScaler::measureDarkCurrent(int secondsDwell)
{
	Q_UNUSED(secondsDwell)
	return;
//	AMAction3 *action = createMeasureDarkCurrentAction(secondsDwell);

//	if (action) {
//		connect( action, SIGNAL(cancelled()), action, SLOT(deleteLater()) );
//		connect( action, SIGNAL(failed()), action, SLOT(deleteLater()) );
//		connect( action, SIGNAL(succeeded()), action, SLOT(deleteLater()) );

//		action->start();
//	}
}

void CLSAMDSScaler::onStartControlValueChanged(double start)
{
	Q_UNUSED(start)

	if(!isConnected())
		return;

	emit startChanged(startControl_->withinTolerance(1));
}

void CLSAMDSScaler::onDwellModeControlValueChanged(double mode)
{
	if(!isConnected())
		return;

	emit dwellModeChanged(mode);
}

void CLSAMDSScaler::onDwellTimeControlValueChanged(double time)
{
	if (!isConnected())
		return;

	if(dwellTimeSourceRequested_){
		dwellTimeSourceRequested_ = false;
		dwellTimeSource_->setSucceeded();
	}

	emit dwellTimeChanged(time);
}

void CLSAMDSScaler::onDwellStateControlValueChanged(double state)
{
	if(!isConnected())
		return;

	if(triggerSourceTriggered_ && readModeForTriggerSource_ == AMDetectorDefinitions::SingleRead){
		triggerSourceTriggered_ = false;
		triggerSource_->setSucceeded();
	}
	emit dwellStateChanged(state);
}

void CLSAMDSScaler::onConnectedChanged(){
	if(isConnected() && !connectedOnce_) {
		connectedOnce_ = true;
	}

	if(connectedOnce_)
		emit connectedChanged(isConnected());
}

void CLSAMDSScaler::onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode)
{
	if(!isConnected())
		return;

	if(readMode == AMDetectorDefinitions::SingleRead){
		if(fabs(dwellState() - 1.0) < 0.1)
			return;
		else if(fabs(dwellMode() - 1.0) < 0.1){
			// We're in the "continuous mode on the PVs, but we should be able to fix this to get to the right state
			return;
		}

		readModeForTriggerSource_ = readMode;
		triggerSourceTriggered_ = true;
		setStarted(true);
		return;
	}
	else if(readMode == AMDetectorDefinitions::ContinuousRead){
		// Need to work with the AMDS server to do this
	}
}

void CLSAMDSScaler::onDwellTimeSourceSetDwellTime(double dwellSeconds)
{
	if(!isConnected())
		return;

	if(fabs(dwellState() - 1.0) < 0.1)
		return;

	if(!(fabs(dwellSeconds - dwellTime()) < dwellTimeTolerance()))
	{
		dwellTimeSourceRequested_ = true;
		setDwellTime(dwellSeconds);
	}
	else
		dwellTimeSource_->setSucceeded();
}

#include "source/ClientRequest/AMDSClientIntrospectionRequest.h"
/// ============= SLOTs to handle AMDSClientAppController signals =========
void CLSAMDSScaler::onRequestDataReady(AMDSClientRequest* clientRequest)
{
	AMDSClientIntrospectionRequest *introspectionRequest = qobject_cast<AMDSClientIntrospectionRequest*>(clientRequest);
	if(introspectionRequest){
		qDebug() << "Got an introspection request response";

		qDebug() << "AMDS_SCALER: All buffer names: " << introspectionRequest->getAllBufferNames();
	}

	AMDSClientRelativeCountPlusCountDataRequest *relativeCountPlusCountDataRequst = qobject_cast<AMDSClientRelativeCountPlusCountDataRequest*>(clientRequest);
	if(relativeCountPlusCountDataRequst){

		if(relativeCountPlusCountDataRequst->bufferName() == amdsBufferName_){
			lastContinuousDataRequest_ = relativeCountPlusCountDataRequst;
			connect(lastContinuousDataRequest_, SIGNAL(destroyed()), this, SLOT(onLastContinuousDataRequestDestroyed()));
			emit amdsDataReady();
		}
	}
}

void CLSAMDSScaler::onServerError(int errorCode, bool removeServer, const QString &serverIdentifier, const QString &errorMessage)
{
	Q_UNUSED(errorCode) 	Q_UNUSED(removeServer)	Q_UNUSED(serverIdentifier)	Q_UNUSED(errorMessage)
}


void CLSAMDSScaler::onLastContinuousDataRequestDestroyed()
{
	lastContinuousDataRequest_ = 0;
}





// CLSAMDSScalerChannel
/////////////////////////////////////////////

CLSAMDSScalerChannel::CLSAMDSScalerChannel(const QString &baseName, int index, QObject *parent) :
	QObject(parent)
{
	QString fullBaseName = QString("%1:%2").arg(baseName).arg(index, 2, 10, QChar('0'));

	wasConnected_ = false;

	// No SR570 or detector to start with.
	currentAmplifier_ = 0;
	voltageRange_ = AMRange();
	detector_ = 0;

	customChannelName_ = QString();

	index_ = index;

	channelVoltage_ = 10000;

	channelEnable_ = new AMSinglePVControl(QString("Channel%1Enable").arg(index), QString("%1:enable").arg(fullBaseName), this, 0.5);
	channelReading_ = new AMReadOnlyPVControl(QString("Channel%1Reading").arg(index), QString("%1:fbk").arg(fullBaseName), this);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(channelEnable_);
	allControls_->addControl(channelReading_);

	connect(channelEnable_, SIGNAL(valueChanged(double)), this, SLOT(onChannelEnabledChanged()));
	connect(channelReading_, SIGNAL(valueChanged(double)), this, SLOT(onChannelReadingChanged(double)));
	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
}

CLSAMDSScalerChannel::~CLSAMDSScalerChannel(){}

bool CLSAMDSScalerChannel::isConnected() const
{
	if (currentAmplifier_)
		return allControls_->isConnected() && currentAmplifier_->isConnected();

	else
		return allControls_->isConnected();
}

bool CLSAMDSScalerChannel::isEnabled() const{

	return isConnected() && channelEnable_->withinTolerance(1);
}

int CLSAMDSScalerChannel::reading() const{

	if(isConnected())
		return int(channelReading_->value());

	return -1;
}

double CLSAMDSScalerChannel::voltage() const
{
	if (isConnected())
		return channelVoltage_;

	return -1.0;
}

void CLSAMDSScalerChannel::onConnectedChanged()
{
	if (wasConnected_ != isConnected())
		emit connected(wasConnected_ = isConnected());
}

AMAction3* CLSAMDSScalerChannel::createEnableAction3(bool setEnabled)
{
	Q_UNUSED(setEnabled)

	if(!isConnected())
		return 0; //NULL

	// right now we don't support enable/disable
	return 0;

//	AMAction3 *action = AMActionSupport::buildControlMoveAction(channelEnable_, setEnabled ? 1 : 0);
//	if(!action)
//		return 0; //NULL

//	return action;
}

void CLSAMDSScalerChannel::setEnabled(bool isEnabled)
{
	Q_UNUSED(isEnabled)

	// right now we don't support enable/disable

//	if(isEnabled && channelEnable_->withinTolerance(0))
//		channelEnable_->move(1);

//	else if(!isEnabled && channelEnable_->withinTolerance(1))
//		channelEnable_->move(0);
}

void CLSAMDSScalerChannel::setCustomChannelName(const QString &customChannelName)
{
	if(customChannelName_ != customChannelName){
		customChannelName_ = customChannelName;
		emit customNameChanged(customChannelName_);
	}
}

void CLSAMDSScalerChannel::onChannelEnabledChanged()
{
	emit enabledChanged(channelEnable_->withinTolerance(1));
}

void CLSAMDSScalerChannel::onChannelReadingChanged(double reading)
{
	emit readingChanged((int)reading);
}

void CLSAMDSScalerChannel::setCurrentAmplifier(AMCurrentAmplifier *amplifier)
{
	if (currentAmplifier_) {
		disconnect( currentAmplifier_, SIGNAL(isConnected(bool)), this, SLOT(onConnectedChanged()));
		disconnect( currentAmplifier_, SIGNAL(valueChanged()), this, SIGNAL(sensitivityChanged()) );
	}

	currentAmplifier_ = amplifier;
	connect( currentAmplifier_, SIGNAL(isConnected(bool)), this, SLOT(onConnectedChanged()) );
	connect( currentAmplifier_, SIGNAL(valueChanged()), this, SIGNAL(sensitivityChanged()) );
	emit currentAmplifierAttached();
}

void CLSAMDSScalerChannel::setMinimumVoltage(double min)
{
	if (voltageRange_.minimum() != min){

		voltageRange_.setMaximum(min);
		emit voltageRangeChanged(voltageRange_);
	}
}

void CLSAMDSScalerChannel::setMaximumVoltage(double max)
{
	if (voltageRange_.maximum() != max){

		voltageRange_.setMaximum(max);
		emit voltageRangeChanged(voltageRange_);
	}
}

void CLSAMDSScalerChannel::setVoltagRange(const AMRange &range)
{
	if (voltageRange_ != range){

		voltageRange_ = range;
		emit voltageRangeChanged(voltageRange_);
	}
}

void CLSAMDSScalerChannel::setVoltagRange(double min, double max)
{
	setVoltagRange(AMRange(min, max));
}

void CLSAMDSScalerChannel::setDetector(AMDetector *detector)
{
	if (detector_ != detector) {
		detector_ = detector;
		emit detectorChanged(detector_);
	}
}
