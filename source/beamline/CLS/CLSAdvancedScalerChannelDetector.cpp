#include "CLSAdvancedScalerChannelDetector.h"

#include "beamline/CLS/CLSSIS3820Scaler.h"

CLSAdvancedScalerChannelDetector::CLSAdvancedScalerChannelDetector(const QString &name, const QString &description, CLSSIS3820Scaler *scaler, int channelIndex, QObject *parent) :
	CLSBasicScalerChannelDetector(name, description, scaler, channelIndex, parent)
{
	readMode_ = AMDetectorDefinitions::SingleRead;
	onScalerConnectedConfirmReadMode(scaler_->isConnected());
	switchingReadModes_ = false;

	continuousData_.reserve(1000);
	continuousSize_ = 0;

	connect(scaler_, SIGNAL(readingChanged()), this, SLOT(onReadingChanged()));
	connect(scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnectedConfirmReadMode(bool)));
	connect(scaler_, SIGNAL(scansPerBufferChanged(int)), this, SLOT(onScansPerBufferChanged(int)));
	connect(scaler_, SIGNAL(totalScansChanged(int)), this, SLOT(onTotalScansChanged(int)));
}

bool CLSAdvancedScalerChannelDetector::lastContinuousReading(double *outputValues) const{
	if(continuousData_.count() == 0)
		return false;

	for(int x = 0; x < continuousData_.count(); x++)
		outputValues[x] = continuousData_.at(x);

	return true;
}

int CLSAdvancedScalerChannelDetector::lastContinuousSize() const{
	return continuousSize_;
}

const double* CLSAdvancedScalerChannelDetector::data() const{
	if(readMode_ == AMDetectorDefinitions::ContinuousRead)
		return continuousData_.constData();
	return data_;
}

bool CLSAdvancedScalerChannelDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode){

	if(readMode_ == readMode){
		emit readModeChanged(readMode_);
		return true;
	}

	readMode_ = readMode;
	if(readMode_ == AMDetectorDefinitions::SingleRead){
		connect(scaler_, SIGNAL(dwellTimeChanged(double)), this, SLOT(onModeSwitchSignal()));
		connect(scaler_, SIGNAL(scansPerBufferChanged(int)), this, SLOT(onModeSwitchSignal()));
		connect(scaler_, SIGNAL(totalScansChanged(int)), this, SLOT(onModeSwitchSignal()));

		switchingReadModes_ = true;
		scaler_->setScansPerBuffer(1);
		scaler_->setTotalScans(1);
		scaler_->setDwellTime(1.0);
	}
	else if(readMode_ == AMDetectorDefinitions::ContinuousRead){
		connect(scaler_, SIGNAL(dwellTimeChanged(double)), this, SLOT(onModeSwitchSignal()));
		connect(scaler_, SIGNAL(scansPerBufferChanged(int)), this, SLOT(onModeSwitchSignal()));
		connect(scaler_, SIGNAL(totalScansChanged(int)), this, SLOT(onModeSwitchSignal()));

		switchingReadModes_ = true;
		scaler_->setScansPerBuffer(1000);
		scaler_->setTotalScans(1000);
		scaler_->setDwellTime(0.005);
	}
	else
		return false;

	return false;
}

void CLSAdvancedScalerChannelDetector::onModeSwitchSignal(){
	if(switchingReadModes_){
		if(readMode_ == AMDetectorDefinitions::SingleRead && scaler_->scansPerBuffer() == 1 && scaler_->totalScans() == 1 && scaler_->dwellTime() == 1.0)
			switchingReadModes_ = false;
		else if(readMode_ == AMDetectorDefinitions::ContinuousRead && scaler_->scansPerBuffer() == 1000 && scaler_->totalScans() == 1000 && scaler_->dwellTime() == 0.005)
			switchingReadModes_ = false;

		if(!switchingReadModes_){
			disconnect(scaler_, SIGNAL(dwellTimeChanged(double)), this, SLOT(onModeSwitchSignal()));
			disconnect(scaler_, SIGNAL(scansPerBufferChanged(int)), this, SLOT(onModeSwitchSignal()));
			disconnect(scaler_, SIGNAL(totalScansChanged(int)), this, SLOT(onModeSwitchSignal()));

			emit readModeChanged(readMode_);
		}
	}
}

void CLSAdvancedScalerChannelDetector::onScalerScanningChanged(bool isScanning){
	if(isScanning)
		setAcquiring();
}

void CLSAdvancedScalerChannelDetector::onReadingChanged(){
	if(readMode_ == AMDetectorDefinitions::SingleRead)
		data_[0] = singleReading();
	else if(readMode_ == AMDetectorDefinitions::ContinuousRead){
		QVector<int> allIntReadings = scaler_->reading();
		int enabledChannelCount = scaler_->enabledChannelCount();
		int totalDataCount = allIntReadings.at(0)/enabledChannelCount;

		continuousData_.clear();
		continuousSize_ = totalDataCount;
		for(int x = 0; x < totalDataCount; x++)
			continuousData_.append(allIntReadings.at(x*enabledChannelCount + channelIndex_ + 1));

		//qDebug() << "\n\n\nMy data is " << continuousData_;
	}

	if(isAcquiring())
		setAcquisitionSucceeded();
	checkReadyForAcquisition();
}

void CLSAdvancedScalerChannelDetector::onScansPerBufferChanged(int scansPerBuffer){
	Q_UNUSED(scansPerBuffer)
	onScalerConnectedConfirmReadMode(scaler_->isConnected());
}

void CLSAdvancedScalerChannelDetector::onTotalScansChanged(int totalScans){
	Q_UNUSED(totalScans)
	onScalerConnectedConfirmReadMode(scaler_->isConnected());
}

bool CLSAdvancedScalerChannelDetector::triggerChannelAcquisition(){
	disconnect(this, SIGNAL(readModeChanged(AMDetectorDefinitions::ReadMode)), this, SLOT(triggerChannelAcquisition()));
	if(!isConnected() || scaler_->isContinuous())
		return false;

	scaler_->setScanning(true);
	return true;
}

void CLSAdvancedScalerChannelDetector::onScalerConnectedConfirmReadMode(bool connected){
	if(connected){
		if(scaler_->scansPerBuffer() == 1000 && scaler_->totalScans() == 1000)
			readMode_ = AMDetectorDefinitions::ContinuousRead;
		else
			readMode_ = AMDetectorDefinitions::SingleRead;
	}
}

bool CLSAdvancedScalerChannelDetector::acquireImplementation(AMDetectorDefinitions::ReadMode readMode){
	if(!isConnected())
		return false;

	if(readMode_ != readMode){
		connect(this, SIGNAL(readModeChanged(AMDetectorDefinitions::ReadMode)), this, SLOT(triggerChannelAcquisition()));
		setReadMode(readMode_);
		return true;
	}
	else
		return triggerChannelAcquisition();
}

