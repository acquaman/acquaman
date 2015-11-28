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
//	return 0;
	return scaler_->triggerSource();
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

AMDetectorContinuousMotionRangeData CLSAMDSScalerChannelDetector::retrieveContinuousMotionRangeData(QList<QVector<qint32> > baseData, int expectedDuration, int threshold)
{
	AMDetectorContinuousMotionRangeData upRange(-1, -1, 0);
	AMDetectorContinuousMotionRangeData downRange(-1, -1, 1);
	bool foundScalerUpMovementStart = false;
	bool foundScalerUpMovementEnd = false;

	QVector<qint32> encoderUpVector = baseData.at(0);
	QVector<qint32> encoderDownVector = baseData.at(1);

	for(int x = encoderUpVector.count()-1; (x > 0) && !foundScalerUpMovementStart; x--){
		if(!foundScalerUpMovementEnd && encoderUpVector.at(x) > threshold){
			qDebug() << "Found scaler UP movement end at index " << x;
			upRange.setMotionEndIndex(x);
			foundScalerUpMovementEnd = true;
		}
		else if(foundScalerUpMovementEnd && encoderUpVector.at(x) < 1){
			foundScalerUpMovementStart = true;
			upRange.setMotionStartIndex(x);
			qDebug() << "Found scaler UP movement start index " << upRange.motionStartIndex();
		}
	}

	bool foundScalerDownMovementStart = false;
	bool foundScalerDownMovementEnd = false;
	for(int x = encoderDownVector.count()-1; (x > 0) && !foundScalerDownMovementStart; x--){
		if(!foundScalerDownMovementEnd && encoderDownVector.at(x) > 20){
			qDebug() << "Found scaler DOWN movement end at index " << x;
			downRange.setMotionEndIndex(x);
			foundScalerDownMovementEnd = true;
		}
		else if(foundScalerDownMovementEnd && encoderDownVector.at(x) < 1){
			foundScalerDownMovementStart = true;
			downRange.setMotionStartIndex(x);
			qDebug() << "Found scaler DOWN movement start index " << downRange.motionStartIndex();
		}
	}

	if(upRange.isValid() && !downRange.isValid())
		return upRange;
	else if(!upRange.isValid() && downRange.isValid())
		return downRange;
	else if(upRange.isValid() && downRange.isValid() && expectedDuration != -1){
		int upDuration = upRange.motionEndIndex() - upRange.motionStartIndex();
		int downDuration = downRange.motionEndIndex() - downRange.motionStartIndex();

		double upPercentDifference = ( fabs(double(upDuration)-double(expectedDuration))/double(expectedDuration) );
		double downPercentDifference = ( fabs(double(downDuration)-double(expectedDuration))/double(expectedDuration) );

		qDebug() << "Up Difference " << upPercentDifference;
		qDebug() << "Down Difference " << downPercentDifference;
		if(upPercentDifference < downPercentDifference)
			return upRange;
		else
			return downRange;
	}
	else
		return AMDetectorContinuousMotionRangeData();
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
	if(readMode_ == AMDetectorDefinitions::SingleRead){
		if( fabs(dwellState - 1.0) < 0.1 )
			setAcquiring();
		else if( fabs(dwellState - 0.0) < 0.1 ){
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
	if(!isConnected())
		return false;

	if(readMode == AMDetectorDefinitions::ContinuousRead){
		readMode_ = AMDetectorDefinitions::ContinuousRead;

		connect(scaler_, SIGNAL(amdsDataReady()), this, SLOT(onScalerAMDSDataReady()));
		setAcquiring();
		bool requestIssued = scaler_->retrieveBufferedData(1);
		if(!requestIssued) {
			setAcquisitionFailed();
			setReadyForAcquisition();
		}
		return requestIssued;
	}
	else if(readMode_ == AMDetectorDefinitions::SingleRead){
		if( fabs(scaler_->dwellState() - 0.0) < 0.1){
			scaler_->setStarted(true);
			return true;
		}
		return false;
	}

	return false;
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
