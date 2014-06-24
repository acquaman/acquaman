#include "CLSBasicScalerChannelDetector.h"

#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/AMDetectorTriggerSource.h"
#include "actions3/actions/AMDoDarkCurrentCorrectionAction.h"
#include "actions3/AMListAction3.h"


CLSBasicScalerChannelDetector::CLSBasicScalerChannelDetector(const QString &name, const QString &description, CLSSIS3820Scaler *scaler, int channelIndex, QObject *parent) :
	AMDetector(name, description, parent)
{
	scaler_ = scaler;
	channelIndex_ = channelIndex;

	connect(scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected(bool)));
	connect(scaler_, SIGNAL(scanningChanged(bool)), this, SLOT(onScalerScanningChanged(bool)));
    connect(scaler_, SIGNAL(newDarkCurrentCorrectionValue()), this, SLOT(onScalerDarkCurrentValueChanged()) );
    connect( scaler_, SIGNAL(newDarkCurrentMeasurementTime(double)), this, SLOT(onScalerDarkCurrentTimeChanged(double)) );
    connect( scaler_, SIGNAL(sensitivityChanged()), this, SLOT(onScalerSensitivityChanged()) );

    connect( this, SIGNAL(newDarkCurrentMeasurementValueReady(double)), scaler_, SIGNAL(newDarkCurrentMeasurementValue(double)) );
}

CLSBasicScalerChannelDetector::~CLSBasicScalerChannelDetector(){}

int CLSBasicScalerChannelDetector::size(int axisNumber) const{
	Q_UNUSED(axisNumber)

	return 0;
}

double CLSBasicScalerChannelDetector::acquisitionTime() const{
	if(isConnected())
		return scaler_->dwellTime();
	return -1;
}

double CLSBasicScalerChannelDetector::acquisitionTimeTolerance() const
{
	if (isConnected())
		return scaler_->dwellTimeTolerance();

	return -1;
}

QString CLSBasicScalerChannelDetector::synchronizedDwellKey() const{
	return scaler_->synchronizedDwellKey();
}

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

bool CLSBasicScalerChannelDetector::data(double *outputValues) const
{
	outputValues[0] = singleReading();
	return true;
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

AMAction3* CLSBasicScalerChannelDetector::createDarkCurrentCorrectionActions(double dwellTime){
    if (dwellTime <= 0)
        return 0;

    AMListAction3* darkCurrentCorrectionActions = new AMListAction3(new AMListActionInfo3("BasicScalerChannelDetector Dark Current Correction", "BasicScalerChannelDetector Dark Current Correction"), AMListAction3::Sequential);
    darkCurrentCorrectionActions->addSubAction(AMBeamline::bl()->createTurnOffBeamActions());
    AMDoDarkCurrentCorrectionActionInfo *actionInfo = new AMDoDarkCurrentCorrectionActionInfo(scaler_, dwellTime);
    AMDoDarkCurrentCorrectionAction *action = new AMDoDarkCurrentCorrectionAction(actionInfo);
    darkCurrentCorrectionActions->addSubAction(action);
    return darkCurrentCorrectionActions;
}

bool CLSBasicScalerChannelDetector::setAcquisitionTime(double seconds){
	if(!isConnected())
		return false;

	scaler_->setDwellTime(seconds);
	emit acquisitionTimeChanged(seconds);
	return true;
}

bool CLSBasicScalerChannelDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode){
	Q_UNUSED(readMode)

	return false;
}

void CLSBasicScalerChannelDetector::onScalerConnected(bool connected){
	setConnected(connected);

	checkReadyForAcquisition();
}

void CLSBasicScalerChannelDetector::onScalerScanningChanged(bool isScanning)
{
	if(isScanning)
		setAcquiring();

	else{

		if(isAcquiring())
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


void CLSBasicScalerChannelDetector::onScalerDarkCurrentTimeChanged(double dwellSeconds) {
    setAsDarkCurrentMeasurementTime(dwellSeconds);
}

void CLSBasicScalerChannelDetector::onScalerDarkCurrentValueChanged() {
    setAsDarkCurrentMeasurementValue();
}

void CLSBasicScalerChannelDetector::onScalerSensitivityChanged() {
    setRequiresNewDarkCurrentMeasurement(true);
}


bool CLSBasicScalerChannelDetector::initializeImplementation(){
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
	if(isConnected()){
		if(!isReadyForAcquisition())
			setReadyForAcquisition();
		if(scaler_->isContinuous())
			setAcquiring();
	}
	else if(!isNotReadyForAcquisition())
		setNotReadyForAcquisition();
}
