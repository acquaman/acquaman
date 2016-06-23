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


#include "CLSBasicScalerChannelDetector.h"

#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/AMDetectorTriggerSource.h"
#include "actions3/AMListAction3.h"

CLSBasicScalerChannelDetector::CLSBasicScalerChannelDetector(const QString &name, const QString &description, CLSSIS3820Scaler *scaler, int channelIndex, QObject *parent) :
	AMDetector(name, description, parent)
{
	scaler_ = scaler;
	channelIndex_ = channelIndex;

	connect(scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected(bool)));
	connect(scaler_, SIGNAL(scanningChanged(bool)), this, SLOT(onScalerScanningChanged(bool)));
	connect(scaler_, SIGNAL(sensitivityChanged()), this, SLOT(onScalerSensitivityChanged()));
	connect(scaler_, SIGNAL(dwellTimeChanged(double)), this, SLOT(onScalerDwellTimeChanged()));
	connect(scaler_, SIGNAL(continuousChanged(bool)), this, SLOT(onContinuousChanged(bool)));

	connect(scaler_, SIGNAL(armed()), this, SIGNAL(armed()));
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

void CLSBasicScalerChannelDetector::arm(){
	scaler_->arm();
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

void CLSBasicScalerChannelDetector::onContinuousChanged(bool isContinuous)
{
	setAcquisitionSucceeded();
	setReadyForAcquisition();

	if (isContinuous)
		setAcquiring();
}

bool CLSBasicScalerChannelDetector::triggerScalerAcquisition(bool isContinuous){
	disconnect(scaler_, SIGNAL(continuousChanged(bool)), this, SLOT(triggerScalerAcquisition(bool)));

	if(isContinuous)
		return false;

	scaler_->setScanning(true);
	return true;
}

void CLSBasicScalerChannelDetector::onScalerSensitivityChanged()
{
	setDarkCurrentValidState(false);
}

void CLSBasicScalerChannelDetector::onScalerDwellTimeChanged()
{
	if (scaler_)
		setDarkCurrentValidState( darkCurrentValidState(scaler_->dwellTime()) );
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
