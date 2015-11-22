#include "CLSAMDSScalerChannelDetector.h"

#include "beamline/CLS/CLSAMDSScaler.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"

CLSAMDSScalerChannelDetector::CLSAMDSScalerChannelDetector(const QString &name, const QString &description, CLSAMDSScaler *scaler, int channelIndex, QObject *parent) :
	AMDetector(name, description, parent)
{
	scaler_ = scaler;
	channelIndex_ = channelIndex;

	connect(scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected(bool)));
	connect(scaler_, SIGNAL(dwellStateChanged(double)), this, SLOT(onScalerDwellStateChanged(double)));
}

CLSAMDSScalerChannelDetector::~CLSAMDSScalerChannelDetector()
{
}

int CLSAMDSScalerChannelDetector::size(int axisNumber) const
{
	Q_UNUSED(axisNumber)
	return 0;
}

double CLSAMDSScalerChannelDetector::acquisitionTime() const
{
	if(isConnected())
		return scaler_->dwellTime();
	return -1;
}

double CLSAMDSScalerChannelDetector::acquisitionTimeTolerance() const
{
	if(isConnected())
		return scaler_->dwellTimeTolerance();
	return -1;
}

QString CLSAMDSScalerChannelDetector::synchronizedDwellKey() const
{
	return QString();
}

AMDetectorTriggerSource* CLSAMDSScalerChannelDetector::detectorTriggerSource()
{
	return 0;
}

AMDetectorDwellTimeSource* CLSAMDSScalerChannelDetector::detectorDwellTimeSource()
{
	return 0;
}

AMDetectorDefinitions::ReadMode CLSAMDSScalerChannelDetector::readMode() const
{
	return readMode_;
}

AMNumber CLSAMDSScalerChannelDetector::reading(const AMnDIndex& indexes) const
{
	if(!isConnected())
		return AMNumber(AMNumber::Null);
	// We want an "invalid" AMnDIndex for this 0D detector
	if(indexes.isValid())
		return AMNumber(AMNumber::DimensionError);

	return scaler_->channelAt(channelIndex_)->reading();
}

AMNumber CLSAMDSScalerChannelDetector::singleReading() const
{
	return reading(AMnDIndex());
}

bool CLSAMDSScalerChannelDetector::data(double *outputValues) const
{
	outputValues[0] = singleReading();
	return true;
}

AMDSClientDataRequest* CLSAMDSScalerChannelDetector::lastContinuousData(double seconds) const
{
	if(isConnected())
		return scaler_->lastContinuousDataRequest();
	return 0;
}

bool CLSAMDSScalerChannelDetector::setContinuousDataWindow(double continuousDataWindowSeconds)
{
	if(isConnected())
		return scaler_->setContinuousDataWindow(continuousDataWindowSeconds);
	return false;
}

QString CLSAMDSScalerChannelDetector::amdsBufferName() const
{
	if(isConnected())
		return scaler_->amdsBufferName();
	return QString();
}

int CLSAMDSScalerChannelDetector::amdsPollingBaseTimeMilliseconds() const
{
	if(isConnected())
		return scaler_->amdsPollingBaseTimeMilliseconds();
	return -1;
}

int CLSAMDSScalerChannelDetector::channelIndex() const
{
	if(isConnected())
		return channelIndex_;
	return -1;
}

int CLSAMDSScalerChannelDetector::enabledChannelIndex() const
{
	if(!isConnected())
		return -1;

	int disabledLowerChannels = 0;
	for(int x = 0, size = channelIndex_; x < size; x++){
		if(!scaler_->channelAt(x)->isEnabled())
			disabledLowerChannels++;
	}
	return channelIndex_ - disabledLowerChannels;
}

bool CLSAMDSScalerChannelDetector::setAcquisitionTime(double seconds)
{
	if(!isConnected())
		return false;

	scaler_->setDwellTime(seconds);
	emit acquisitionTimeChanged(seconds);
	return true;
}

bool CLSAMDSScalerChannelDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode)
{
	readMode_ = readMode;
	emit readModeChanged(readMode_);

	return true;
}

void CLSAMDSScalerChannelDetector::onScalerConnected(bool connected)
{
	setConnected(connected);

	checkReadyForAcquisition();
}


void CLSAMDSScalerChannelDetector::onScalerDwellStateChanged(double dwellState)
{
//	qDebug() << "Detected the scaler dwell state changed to " << dwellState;
	if(readMode_ == AMDetectorDefinitions::SingleRead){
//		qDebug() << "In the singleRead branch";
		if( fabs(dwellState - 1.0) < 0.1 ){
//			qDebug() << "Looks like we should setAcquiring";
			setAcquiring();
		}
		else if( fabs(dwellState - 0.0) < 0.1 ){
//			qDebug() << "Looks like we should setAcquisitionSucceeded";
			setAcquisitionSucceeded();
			checkReadyForAcquisition();
		}
	}
}

void CLSAMDSScalerChannelDetector::onScalerAMDSDataReady()
{
	qDebug() << "AMDS_SCALER ScalerAMDSDataReady in CLSAMDSScalerChannelDetector";
	disconnect(scaler_, SIGNAL(amdsDataReady()), this, SLOT(onScalerAMDSDataReady()));
	setAcquisitionSucceeded();
	setReadyForAcquisition();
}

bool CLSAMDSScalerChannelDetector::initializeImplementation()
{
	setInitialized();
	return true;
}

bool CLSAMDSScalerChannelDetector::acquireImplementation(AMDetectorDefinitions::ReadMode readMode)
{
	qDebug() << "CLSAMDSScalerChannelDetector asked for acquireImplementation with read mode " << readMode;
	if(!isConnected())
		return false;

	if(readMode == AMDetectorDefinitions::ContinuousRead){
		qDebug() << "ContinuousRead in CLSAMDSScalerChannelDetector";
		readMode_ = AMDetectorDefinitions::ContinuousRead;

		connect(scaler_, SIGNAL(amdsDataReady()), this, SLOT(onScalerAMDSDataReady()));
		setAcquiring();
		scaler_->retrieveBufferedData(1);
		return true;
	}
	else if(readMode_ == AMDetectorDefinitions::SingleRead){
		qDebug() << "Current dwell state is " << scaler_->dwellState();
		if( fabs(scaler_->dwellState() - 0.0) < 0.1){
			qDebug() << "Passed test, set started";
			scaler_->setStarted(true);
			return true;
		}
		return false;
	}
}

bool CLSAMDSScalerChannelDetector::cleanupImplementation()
{
	setCleanedUp();
	return true;
}

void CLSAMDSScalerChannelDetector::checkReadyForAcquisition()
{
	if(isConnected())
		if(!isReadyForAcquisition())
			setReadyForAcquisition();
}
