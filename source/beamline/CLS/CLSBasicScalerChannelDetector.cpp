#include "CLSBasicScalerChannelDetector.h"

#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/AMDetectorTriggerSource.h"

CLSBasicScalerChannelDetector::CLSBasicScalerChannelDetector(const QString &name, const QString &description, CLSSIS3820Scaler *scaler, int channelIndex, QObject *parent) :
	AMDetector(name, description, parent)
{
	scaler_ = scaler;
	channelIndex_ = channelIndex;

	data_ = new double[1];
	data_[0] = 0;

	connect(scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected(bool)));
	connect(scaler_, SIGNAL(scanningChanged(bool)), this, SLOT(onScalerScanningChanged(bool)));
}

int CLSBasicScalerChannelDetector::size(int axisNumber) const{
	Q_UNUSED(axisNumber)

	return 0;
}

double CLSBasicScalerChannelDetector::acquisitionTime() const{
	if(isConnected())
		return scaler_->dwellTime();
	return -1;
}

QString CLSBasicScalerChannelDetector::synchronizedDwellKey() const{
	return scaler_->synchronizedDwellKey();
}

#include "beamline/AMBeamline.h"
AMDetectorTriggerSource* CLSBasicScalerChannelDetector::detectorTriggerSource(){
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->triggerSource();
	return scaler_->triggerSource();
}

AMDetectorDwellTimeSource* CLSBasicScalerChannelDetector::detectorDwellTimeSource(){
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->dwellTimeSource();
	return scaler_->dwellTimeSource();
}

const double* CLSBasicScalerChannelDetector::data() const{
	return data_;
}

AMNumber CLSBasicScalerChannelDetector::reading(const AMnDIndex &indexes) const{
	if(!isConnected())
		return AMNumber(AMNumber::Null);
	// We want an "invalid" AMnDIndex for this 0D detector
	if(indexes.isValid())
		return AMNumber(AMNumber::DimensionError);

	return scaler_->channelAt(channelIndex_)->reading();
}

AMNumber CLSBasicScalerChannelDetector::singleReading() const{
	return reading(AMnDIndex());
}

bool CLSBasicScalerChannelDetector::lastContinuousReading(double *outputValues) const{
	Q_UNUSED(outputValues);
	return false;
}

bool CLSBasicScalerChannelDetector::setAcquisitionTime(double seconds){
	if(!isConnected())
		return false;

	if( !(fabs(scaler_->dwellTime()-seconds) < 0.001) ){
		scaler_->setDwellTime(seconds);
		return true;
	}

	return false;
}

bool CLSBasicScalerChannelDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode){
	Q_UNUSED(readMode)

	return false;
}

void CLSBasicScalerChannelDetector::onScalerConnected(bool connected){
	setConnected(connected);

	checkReadyForAcquisition();
}

void CLSBasicScalerChannelDetector::onScalerScanningChanged(bool isScanning){
	if(isScanning)
		setAcquiring();
	else{
		data_[0] = singleReading();

		setAcquisitionSucceeded();
		checkReadyForAcquisition();
	}
}

bool CLSBasicScalerChannelDetector::triggerScalerAcquisition(bool isContinuous){
	disconnect(scaler_, SIGNAL(continuousChanged(bool)), this, SLOT(triggerScalerAcquisition(bool)));
	if(isContinuous)
		return false;

	scaler_->setScanning(true);
	return true;
}

bool CLSBasicScalerChannelDetector::initializeImplementation(){
	//setInitializing();
	setInitialized();
	return true;
}

bool CLSBasicScalerChannelDetector::acquireImplementation(AMDetectorDefinitions::ReadMode readMode){
	if(!isConnected() || readMode != AMDetectorDefinitions::SingleRead)
		return false;

	if(scaler_->isContinuous()){
		connect(scaler_, SIGNAL(continuousChanged(bool)), this, SLOT(triggerScalerAcquisition(bool)));
		scaler_->setContinuous(false);
		return true;
	}
	else
		return triggerScalerAcquisition(scaler_->isContinuous());

}

bool CLSBasicScalerChannelDetector::cleanupImplementation(){
	setCleanedUp();
	return true;
}

void CLSBasicScalerChannelDetector::checkReadyForAcquisition(){
	if(isConnected() && !scaler_->isContinuous())
		setReadyForAcquisition();
	else
		setNotReadyForAcquisition();
}
