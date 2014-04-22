#include "AMBasicControlDetectorEmulator.h"

 AMBasicControlDetectorEmulator::~AMBasicControlDetectorEmulator(){}
AMBasicControlDetectorEmulator::AMBasicControlDetectorEmulator(const QString &name, const QString &description, AMControl *control, AMControl *statusControl, double statusAcquiringValue, double statusNotAcquiringValue, AMDetectorDefinitions::ReadMethod readMethod, QObject *parent) :
	AMDetector(name, description, parent)
{
	waitingForNewData_ = false;
	waitingForStatusChange_ = false;

	statusAcquiringValue_ = statusAcquiringValue;
	statusNotAcquiringValue_ = statusNotAcquiringValue;

	readMethod_ = readMethod;

	allControls_ = new AMControlSet(this);
	control_ = control;
	statusControl_ = statusControl;

	if(control_)
		allControls_->addControl(control_);
	if(statusControl_)
		allControls_->addControl(statusControl_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
	connect(allControls_, SIGNAL(controlSetTimedOut()), this, SLOT(onControlsTimedOut()));

	if(control_)
		connect(control_, SIGNAL(valueChanged(double)), this, SLOT(onControlValueChanged(double)));
	if(statusControl_)
		connect(statusControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusControlValueChanged(double)));
}

AMNumber AMBasicControlDetectorEmulator::reading(const AMnDIndex &indexes) const
{
	if(!isConnected())
		return AMNumber(AMNumber::Null);
	// We want an "invalid" AMnDIndex for this 0D detector
	if(indexes.isValid())
		return AMNumber(AMNumber::DimensionError);

	return control_->value();
}

AMNumber AMBasicControlDetectorEmulator::singleReading() const{
	return reading(AMnDIndex());
}

bool AMBasicControlDetectorEmulator::data(double *outputValues) const
{
	outputValues[0] = control_->value();
	return true;
}

AMAction3* AMBasicControlDetectorEmulator::createTriggerAction(AMDetectorDefinitions::ReadMode readMode){
	if(readMethod() != AMDetectorDefinitions::RequestRead)
		return 0;
	return AMDetector::createTriggerAction(readMode);
}

void AMBasicControlDetectorEmulator::onControlsConnected(bool connected){
	if(connected)
		setConnected(connected);
	else
		setConnected(false);

	checkReadyForAcquisition();
}

void AMBasicControlDetectorEmulator::onControlsTimedOut(){
	setConnected(false);
}

void AMBasicControlDetectorEmulator::onControlValueChanged(double value){
	Q_UNUSED(value)
	emit newValuesAvailable();
	if(waitingForNewData_){
		waitingForNewData_ = false;
		setAcquisitionSucceeded();
		checkReadyForAcquisition();
	}
}

void AMBasicControlDetectorEmulator::onStatusControlValueChanged(double value){
	Q_UNUSED(value)
	if(waitingForStatusChange_ && statusControl_ && statusControl_->withinTolerance(statusNotAcquiringValue_)){
		waitingForStatusChange_ = false;
		setAcquisitionSucceeded();
		checkReadyForAcquisition();
	}
}

bool AMBasicControlDetectorEmulator::initializeImplementation(){
	setInitialized();
	return true;
}

bool AMBasicControlDetectorEmulator::acquireImplementation(AMDetectorDefinitions::ReadMode readMode){
	if(!isConnected() || readMode != AMDetectorDefinitions::SingleRead)
		return false;

	setAcquiring();
	if(readMethod() == AMDetectorDefinitions::ImmediateRead){
		setAcquisitionSucceeded();
		checkReadyForAcquisition();
	}
	else if(readMethod() == AMDetectorDefinitions::WaitRead)
		waitingForNewData_ = true;
	else if((readMethod() == AMDetectorDefinitions::RequestRead) && statusControl_ )
		waitingForStatusChange_ = true;
	return true;
}

bool AMBasicControlDetectorEmulator::cleanupImplementation(){
	setCleanedUp();
	return true;
}

void AMBasicControlDetectorEmulator::checkReadyForAcquisition(){
	if(isConnected())
		setReadyForAcquisition();
	else
		setNotReadyForAcquisition();
}
