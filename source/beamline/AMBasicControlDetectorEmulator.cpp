/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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

	controlProperty_ = Control::Value;

	setIsVisible(false);
	setHiddenFromUsers(true);

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
	if (!isConnected())
		return AMNumber(AMNumber::Null);

	// We want an "invalid" AMnDIndex for this 0D detector
	if (indexes.isValid())
		return AMNumber(AMNumber::DimensionError);

	AMNumber result = AMNumber(AMNumber::Null);

	if (controlProperty_ == Control::Value)
		result = control_->value();

	else if (controlProperty_ == Control::Setpoint)
		result = control_->setpoint();

	return result;
}

AMNumber AMBasicControlDetectorEmulator::singleReading() const{
	return reading(AMnDIndex());
}

bool AMBasicControlDetectorEmulator::data(double *outputValues) const
{
	bool result = false;

	if (controlProperty_ == Control::Value) {
		outputValues[0] = control_->value();
		result = true;

	} else if (controlProperty_ == Control::Setpoint) {
		outputValues[0] = control_->setpoint();
		result = true;
	}

	return result;
}

AMAction3* AMBasicControlDetectorEmulator::createTriggerAction(AMDetectorDefinitions::ReadMode readMode){
	if(readMethod() != AMDetectorDefinitions::RequestRead)
		return 0;
	return AMDetector::createTriggerAction(readMode);
}

void AMBasicControlDetectorEmulator::setControlProperty(Control::Property newProperty)
{
	if (controlProperty_ != newProperty) {
		controlProperty_ = newProperty;
		emit controlPropertyChanged(controlProperty_);
	}
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
