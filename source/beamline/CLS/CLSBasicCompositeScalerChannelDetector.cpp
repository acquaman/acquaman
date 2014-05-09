#include "CLSBasicCompositeScalerChannelDetector.h"

#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/AMDetectorTriggerSource.h"

 CLSBasicCompositeScalerChannelDetector::~CLSBasicCompositeScalerChannelDetector(){}
CLSBasicCompositeScalerChannelDetector::CLSBasicCompositeScalerChannelDetector(const QString &name, const QString &description, CLSSIS3820Scaler *scaler, int channelIndex1, int channelIndex2, QObject *parent)
	: AMDetector(name, description, parent)
{
	scaler_ = scaler;
	channelIndex1_ = channelIndex1;
	channelIndex2_ = channelIndex2;

	connect(scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected(bool)));
	connect(scaler_, SIGNAL(scanningChanged(bool)), this, SLOT(onScalerScanningChanged(bool)));
}

int CLSBasicCompositeScalerChannelDetector::size(int axisNumber) const{
	Q_UNUSED(axisNumber)

	return 0;
}

double CLSBasicCompositeScalerChannelDetector::acquisitionTime() const{
	if(isConnected())
		return scaler_->dwellTime();
	return -1;
}

double CLSBasicCompositeScalerChannelDetector::acquisitionTimeTolerance() const
{
	if (isConnected())
		return scaler_->dwellTimeTolerance();

	return -1;
}

QString CLSBasicCompositeScalerChannelDetector::synchronizedDwellKey() const{
	return scaler_->synchronizedDwellKey();
}

#include "beamline/AMBeamline.h"
AMDetectorTriggerSource* CLSBasicCompositeScalerChannelDetector::detectorTriggerSource(){
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->triggerSource();
	return scaler_->triggerSource();
}

AMDetectorDwellTimeSource* CLSBasicCompositeScalerChannelDetector::detectorDwellTimeSource(){
	if(currentlySynchronizedDwell())
		return AMBeamline::bl()->synchronizedDwellTime()->dwellTimeSource();
	return scaler_->dwellTimeSource();
}

bool CLSBasicCompositeScalerChannelDetector::data(double *outputValues) const
{
	outputValues[0] = singleReading();
	return true;
}

AMNumber CLSBasicCompositeScalerChannelDetector::reading(const AMnDIndex &indexes) const{
	if(!isConnected())
		return AMNumber(AMNumber::Null);
	// We want an "invalid" AMnDIndex for this 0D detector
	if(indexes.isValid())
		return AMNumber(AMNumber::DimensionError);

	return scaler_->channelAt(channelIndex1_)->reading() + scaler_->channelAt(channelIndex2_)->reading();
}

AMNumber CLSBasicCompositeScalerChannelDetector::singleReading() const{
	return reading(AMnDIndex());
}

bool CLSBasicCompositeScalerChannelDetector::lastContinuousReading(double *outputValues) const{
	Q_UNUSED(outputValues);
	return false;
}

bool CLSBasicCompositeScalerChannelDetector::setAcquisitionTime(double seconds){
	if(!isConnected())
		return false;

	if( !(fabs(scaler_->dwellTime()-seconds) < 0.001) ){
		scaler_->setDwellTime(seconds);
		return true;
	}

	return false;
}

bool CLSBasicCompositeScalerChannelDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode){
	Q_UNUSED(readMode)

	return false;
}

void CLSBasicCompositeScalerChannelDetector::onScalerConnected(bool connected){
	setConnected(connected);

	checkReadyForAcquisition();
}

void CLSBasicCompositeScalerChannelDetector::onScalerScanningChanged(bool isScanning){
	if(isScanning)
		setAcquiring();
	else{

		if(isAcquiring())
			setAcquisitionSucceeded();
		checkReadyForAcquisition();
	}
}

bool CLSBasicCompositeScalerChannelDetector::triggerScalerAcquisition(bool isContinuous){
	disconnect(scaler_, SIGNAL(continuousChanged(bool)), this, SLOT(triggerScalerAcquisition(bool)));
	if(isContinuous)
		return false;

	scaler_->setScanning(true);
	return true;
}

bool CLSBasicCompositeScalerChannelDetector::initializeImplementation(){
	//setInitializing();
	setInitialized();
	return true;
}

bool CLSBasicCompositeScalerChannelDetector::acquireImplementation(AMDetectorDefinitions::ReadMode readMode){
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

bool CLSBasicCompositeScalerChannelDetector::cleanupImplementation(){
	setCleanedUp();
	return true;
}

void CLSBasicCompositeScalerChannelDetector::checkReadyForAcquisition(){
	if(isConnected()){
		if(!isReadyForAcquisition())
			setReadyForAcquisition();
		if(scaler_->isContinuous())
			setAcquiring();
	}
	else if(!isNotReadyForAcquisition())
		setNotReadyForAcquisition();
}
