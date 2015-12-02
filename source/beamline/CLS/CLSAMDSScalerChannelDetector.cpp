#include "CLSAMDSScalerChannelDetector.h"

#include "beamline/CLS/CLSAMDSScaler.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "util/AMScalerCountAnalyser.h"

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

AMDetectorContinuousMotionRangeData CLSAMDSScalerChannelDetector::retrieveContinuousMotionRangeData(const QList<QVector<qint32> > &baseData, int expectedDuration, int threshold)
{
	AMDetectorContinuousMotionRangeData retVal;
	QVector<double> base0AsDouble = QVector<double>(baseData.at(0).count());
	QVector<double> base1AsDouble = QVector<double>(baseData.at(1).count());
	for(int x = 0, size = base0AsDouble.count(); x < size; x++){
		base0AsDouble[x] = double(baseData.at(0).at(x));
		base1AsDouble[x] = double(baseData.at(1).at(x));
	}
	AMScalerCountAnalyser base0Analyzer(base0AsDouble, threshold, 2);
	AMScalerCountAnalyser base1Analyzer(base1AsDouble, threshold, 2);

	qDebug() << "Scaler base[0] analyzer sees: " << base0Analyzer.toString();
	qDebug() << "Scaler base[1] analyzer sees: " << base1Analyzer.toString();

	double bestPercentDifference = 1.0;
	double tempPercentDifference;
	for(int x = 0, size = base0Analyzer.periodsOfInterest().count(); x < size; x++){
		int tempDuration = base0Analyzer.periodsOfInterest().at(x).second - base0Analyzer.periodsOfInterest().at(x).first;
		tempPercentDifference = ( fabs(double(tempDuration)-double(expectedDuration))/double(expectedDuration) );
		if(tempPercentDifference < bestPercentDifference){
			bestPercentDifference = tempPercentDifference;
			retVal.setMotionStartIndex(base0Analyzer.periodsOfInterest().at(x).first);
			retVal.setMotionEndIndex(base0Analyzer.periodsOfInterest().at(x).second);
			retVal.setListIndex(0);
		}
	}
	for(int x = 0, size = base1Analyzer.periodsOfInterest().count(); x < size; x++){
		int tempDuration = base1Analyzer.periodsOfInterest().at(x).second - base1Analyzer.periodsOfInterest().at(x).first;
		tempPercentDifference = ( fabs(double(tempDuration)-double(expectedDuration))/double(expectedDuration) );
		if(tempPercentDifference < bestPercentDifference){
			bestPercentDifference = tempPercentDifference;
			retVal.setMotionStartIndex(base1Analyzer.periodsOfInterest().at(x).first);
			retVal.setMotionEndIndex(base1Analyzer.periodsOfInterest().at(x).second);
			retVal.setListIndex(1);
		}
	}

	return retVal;
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
