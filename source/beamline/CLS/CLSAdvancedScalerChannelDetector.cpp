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


#include "CLSAdvancedScalerChannelDetector.h"

#include "beamline/CLS/CLSSIS3820Scaler.h"

#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"


CLSScalerChannelDetector::CLSScalerChannelDetector(const QString &name, const QString &description, CLSSIS3820Scaler *scaler, int channelIndex, QObject *parent) :
	AMDetector(name, description, parent)
{
	scaler_ = scaler;
	channelIndex_ = channelIndex;

	connect(scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected(bool)));
}

CLSScalerChannelDetector::~CLSScalerChannelDetector()
{
}

int CLSScalerChannelDetector::size(int axisNumber) const
{
	Q_UNUSED(axisNumber)
	return 0;
}

double CLSScalerChannelDetector::acquisitionTime() const
{
	if(isConnected())
		return scaler_->dwellTime();
	return -1;
}

double CLSScalerChannelDetector::acquisitionTimeTolerance() const
{
	if(isConnected())
		return scaler_->dwellTimeTolerance();
	return -1;
}

QString CLSScalerChannelDetector::synchronizedDwellKey() const
{
	return QString();
}

AMDetectorTriggerSource* CLSScalerChannelDetector::detectorTriggerSource()
{
	return 0;
}

AMDetectorDwellTimeSource* CLSScalerChannelDetector::detectorDwellTimeSource()
{
	return 0;
}

AMDetectorDefinitions::ReadMode CLSScalerChannelDetector::readMode() const
{
	return readMode_;
}

AMNumber CLSScalerChannelDetector::reading(const AMnDIndex& indexes) const
{
	if(!isConnected())
		return AMNumber(AMNumber::Null);
	// We want an "invalid" AMnDIndex for this 0D detector
	if(indexes.isValid())
		return AMNumber(AMNumber::DimensionError);

	return scaler_->channelAt(channelIndex_)->reading();
}

AMNumber CLSScalerChannelDetector::singleReading() const
{
	return reading(AMnDIndex());
}

bool CLSScalerChannelDetector::data(double *outputValues) const
{
	outputValues[0] = singleReading();
	return true;
}

AMDSClientDataRequest* CLSScalerChannelDetector::lastContinuousData(double seconds) const
{
	if(isConnected())
		return scaler_->lastContinuousDataRequest();
	return 0;
}

bool CLSScalerChannelDetector::setContinuousDataWindow(double continuousDataWindowSeconds)
{
	if(isConnected())
		return scaler_->setContinuousDataWindow(continuousDataWindowSeconds);
	return false;
}

QString CLSScalerChannelDetector::amdsBufferName() const
{
	if(isConnected())
		return scaler_->amdsBufferName();
	return QString();
}

int CLSScalerChannelDetector::amdsPollingBaseTimeMilliseconds() const
{
	if(isConnected())
		return scaler_->amdsPollingBaseTimeMilliseconds();
	return -1;
}

int CLSScalerChannelDetector::channelIndex() const
{
	if(isConnected())
		return channelIndex_;
	return -1;
}

int CLSScalerChannelDetector::enabledChannelIndex() const
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

bool CLSScalerChannelDetector::setAcquisitionTime(double seconds)
{
	if(!isConnected())
		return false;

	scaler_->setDwellTime(seconds);
	emit acquisitionTimeChanged(seconds);
	return true;
}

bool CLSScalerChannelDetector::setReadMode(AMDetectorDefinitions::ReadMode readMode)
{
	readMode_ = readMode;
	emit readModeChanged(readMode_);

	return true;
}

void CLSScalerChannelDetector::onScalerConnected(bool connected)
{
	setConnected(connected);

	checkReadyForAcquisition();
}

void CLSScalerChannelDetector::onScalerScanningChanged(bool isScanning)
{
	// REQUIRES IMPLEMENTATION WHEN WE HAVE THE DWELL/TRIGGER GOING FOR THE AMDS-SCALER
}

void CLSScalerChannelDetector::onScalerAMDSDataReady()
{
	qDebug() << "ScalerAMDSDataReady in CLSScalerChannelDetector";
	disconnect(scaler_, SIGNAL(amdsDataReady()), this, SLOT(onScalerAMDSDataReady()));
	setAcquisitionSucceeded();
	setReadyForAcquisition();
}

bool CLSScalerChannelDetector::initializeImplementation()
{
	setInitialized();
	return true;
}

bool CLSScalerChannelDetector::acquireImplementation(AMDetectorDefinitions::ReadMode readMode)
{
	qDebug() << "CLSScalerChannelDetector asked for acquireImplementation with read mode " << readMode;
	if(!isConnected())
		return false;

	if(readMode == AMDetectorDefinitions::ContinuousRead){
		qDebug() << "ContinuousRead in CLSScalerChannelDetector";
		readMode_ = AMDetectorDefinitions::ContinuousRead;

		connect(scaler_, SIGNAL(amdsDataReady()), this, SLOT(onScalerAMDSDataReady()));
		setAcquiring();
		scaler_->retrieveBufferedData(1);
	}
	else{
		// REQUIRES IMPLEMENTATION WHEN WE HAVE THE DWELL/TRIGGER GOING FOR THE AMDS-SCALER
//		if(readMode_ != readMode){
//			connect(this, SIGNAL(readModeChanged(AMDetectorDefinitions::ReadMode)), this, SLOT(triggerChannelAcquisition()));
//			setReadMode(readMode_);
//			return true;
//		}
//		else
//			return triggerChannelAcquisition();
	}
}

bool CLSScalerChannelDetector::cleanupImplementation()
{
	setCleanedUp();
	return true;
}

void CLSScalerChannelDetector::checkReadyForAcquisition()
{
	if(isConnected())
		if(!isReadyForAcquisition())
			setReadyForAcquisition();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

CLSAdvancedScalerChannelDetector::~CLSAdvancedScalerChannelDetector(){}

bool CLSAdvancedScalerChannelDetector::data(double *outputValues) const
{
//	if(readMode_ == AMDetectorDefinitions::ContinuousRead)
//		return lastContinuousReading(outputValues);

	outputValues[0] = singleReading();

	return true;
}

AMDSClientDataRequest* CLSAdvancedScalerChannelDetector::lastContinuousData(double seconds) const
{
	return scaler_->lastContinuousDataRequest();
}

int CLSAdvancedScalerChannelDetector::enabledChannelIndex() const
{
	int disabledLowerChannels = 0;
	for(int x = 0, size = channelIndex_; x < size; x++){
		if(!scaler_->channelAt(x)->isEnabled())
			disabledLowerChannels++;
	}
	return channelIndex_ - disabledLowerChannels;
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
	else if(readMode_== AMDetectorDefinitions::SingleRead){

		if(isAcquiring())
			setAcquisitionSucceeded();
		checkReadyForAcquisition();
	}
}

void CLSAdvancedScalerChannelDetector::onReadingChanged(){

	if(readMode_ == AMDetectorDefinitions::ContinuousRead){
//		QVector<int> allIntReadings = scaler_->reading();
//		int enabledChannelCount = scaler_->enabledChannelCount();
//		int totalDataCount = allIntReadings.at(0)/enabledChannelCount;

//		continuousData_.clear();
//		continuousSize_ = totalDataCount;
//		for(int x = 0; x < totalDataCount; x++)
//			continuousData_.append(allIntReadings.at(x*enabledChannelCount + channelIndex_ + 1));
	}

	if(isAcquiring() && readMode_ == AMDetectorDefinitions::SingleRead)
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

void CLSAdvancedScalerChannelDetector::onScalerAMDSDataReady()
{
	qDebug() << "ScalerAMDSDataReady in CLSAdvancedScalerChannelDetector";
	disconnect(scaler_, SIGNAL(amdsDataReady()), this, SLOT(onScalerAMDSDataReady()));
	setAcquisitionSucceeded();
	setReadyForAcquisition();
}

bool CLSAdvancedScalerChannelDetector::acquireImplementation(AMDetectorDefinitions::ReadMode readMode)
{
	qDebug() << "CLSAdvancedScalerChannelDetector asked for acquireImplementation with read mode " << readMode;
	if(!isConnected())
		return false;

	if(readMode == AMDetectorDefinitions::ContinuousRead){
		qDebug() << "ContinuousRead in CLSAdvancedScalerChannelDetector";
		readMode_ = AMDetectorDefinitions::ContinuousRead;

		connect(scaler_, SIGNAL(amdsDataReady()), this, SLOT(onScalerAMDSDataReady()));
		setAcquiring();
		scaler_->retrieveBufferedData(1);
	}

	if(readMode_ != readMode){
		connect(this, SIGNAL(readModeChanged(AMDetectorDefinitions::ReadMode)), this, SLOT(triggerChannelAcquisition()));
		setReadMode(readMode_);
		return true;
	}
	else
		return triggerChannelAcquisition();
}

void CLSAdvancedScalerChannelDetector::checkReadyForAcquisition(){
	if(isConnected()){
		if(!isReadyForAcquisition())
			setReadyForAcquisition();
	}
}
