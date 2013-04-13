/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "CLSSIS3820Scaler.h"
#include "beamline/AMPVControl.h"
#include "actions/AMBeamlineControlMoveAction.h"
#include "beamline/AMDetectorTriggerSource.h"

#include "actions3/actions/AMControlMoveAction3.h"

// CLSSIS3820Scalar
/////////////////////////////////////////////

CLSSIS3820Scaler::CLSSIS3820Scaler(const QString &baseName, QObject *parent) :
	QObject(parent)
{
	connectedOnce_ = false;

	triggerSource_ = new AMDetectorTriggerSource(QString("%1TriggerSource").arg(baseName), this);
	connect(triggerSource_, SIGNAL(triggered(AMDetectorDefinitions::ReadMode)), this, SLOT(onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode)));
	dwellTimeSource_ = new AMDetectorDwellTimeSource(QString("%1DwellTimeSource").arg(baseName), this);
	connect(dwellTimeSource_, SIGNAL(setDwellTime(double)), this, SLOT(onDwellTimeSourceSetDwellTime(double)));

	synchronizedDwellKey_ = QString("%1:startScan NPP NMS").arg(baseName);

	CLSSIS3820ScalerChannel *tmpChannel;
	for(int x = 0; x < 32; x++){
		tmpChannel = new CLSSIS3820ScalerChannel(baseName, x, this);
		scalerChannels_.append(tmpChannel);
		connect(tmpChannel, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
	}

	startToggle_ = new AMPVControl("Start/Scanning", baseName+":startScan", baseName+":startScan", QString(), this, 0.1);
	continuousToggle_ = new AMPVControl("Continuous", baseName+":continuous", baseName+":continuous", QString(), this, 0.1);
	dwellTime_ = new AMPVControl("DwellTime", baseName+":delay", baseName+":delay", QString(), this, 0.1);
	scanPerBuffer_ = new AMPVControl("ScanPerBuffer", baseName+":nscan", baseName+":nscan", QString(), this, 0.5);
	totalScans_ = new AMPVControl("TotalScans", baseName+":scanCount", baseName+":scanCount", QString(), this, 0.5);

	reading_ = new AMReadOnlyPVControl("Reading", baseName+":scan", this);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(startToggle_);
	allControls_->addControl(continuousToggle_);
	allControls_->addControl(dwellTime_);
	allControls_->addControl(scanPerBuffer_);
	allControls_->addControl(totalScans_);
	allControls_->addControl(reading_);

	connect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(onScanningToggleChanged()));
	connect(continuousToggle_, SIGNAL(valueChanged(double)), this, SLOT(onContinuousToggleChanged()));
	connect(dwellTime_, SIGNAL(valueChanged(double)), this, SLOT(onDwellTimeChanged(double)));
	connect(scanPerBuffer_, SIGNAL(valueChanged(double)), this, SLOT(onScanPerBufferChanged(double)));
	connect(totalScans_, SIGNAL(valueChanged(double)), this, SLOT(onTotalScansChanged(double)));
	connect(reading_, SIGNAL(valueChanged(double)), this, SIGNAL(readingChanged()));
	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
}

bool CLSSIS3820Scaler::isConnected() const{

	bool retVal = true;

	for(int x = 0; x < scalerChannels_.count(); x++)
		retVal &= scalerChannels_.at(x)->isConnected();

	return retVal && allControls_->isConnected();
}

bool CLSSIS3820Scaler::isScanning() const{

	if(isConnected() && startToggle_->withinTolerance(1))
		return true;

	return false;
}

bool CLSSIS3820Scaler::isContinuous() const{

	if(isConnected() && continuousToggle_->withinTolerance(1))
		return true;

	return false;
}

double CLSSIS3820Scaler::dwellTime() const{

	if(isConnected())
		return dwellTime_->value()/1000;

	return -1;
}

int CLSSIS3820Scaler::scansPerBuffer() const{

	if(isConnected())
		return (int)scanPerBuffer_->value();

	return -1;
}

int CLSSIS3820Scaler::totalScans() const{

	if(isConnected())
		return (int)totalScans_->value();

	return -1;
}

QVector<int> CLSSIS3820Scaler::reading() const{

	if(isConnected())
		return qobject_cast<AMReadOnlyPVControl*>(reading_)->readPV()->lastIntegerValues();

	return QVector<int>();
}

CLSSIS3820ScalerChannel* CLSSIS3820Scaler::channelAt(int index){
	return scalerChannels_.at(index);
}

AMOrderedList<CLSSIS3820ScalerChannel*> CLSSIS3820Scaler::channels(){
	return scalerChannels_;
}

AMDetectorTriggerSource* CLSSIS3820Scaler::triggerSource(){
	return triggerSource_;
}

AMDetectorDwellTimeSource* CLSSIS3820Scaler::dwellTimeSource(){
	return dwellTimeSource_;
}

AMBeamlineActionItem* CLSSIS3820Scaler::createStartAction(bool setScanning) {

	if(!isConnected())
		return 0; //NULL

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(startToggle_);

	if(!action)
		return 0; //NULL

	action->setSetpoint(setScanning == true ? 1 : 0);

	return action;
}

AMAction3* CLSSIS3820Scaler::createStartAction3(bool setScanning){
	if(!isConnected())
		return 0; //NULL

	AMControlInfo setpoint = startToggle_->toInfo();
	setpoint.setValue(setScanning == true ? 1 : 0);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);

	AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, startToggle_);

	if(!action)
		return 0; //NULL

	return action;
}

AMBeamlineActionItem* CLSSIS3820Scaler::createContinuousEnableAction(bool enableContinuous) {

	if(!isConnected())
		return 0; //NULL

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(continuousToggle_);

	if(!action)
		return 0; //NULL

	action->setSetpoint(enableContinuous == true ? 1 : 0);

	return action;
}

AMAction3* CLSSIS3820Scaler::createContinuousEnableAction3(bool enableContinuous){
	if(!isConnected())
		return 0; //NULL

	AMControlInfo setpoint = continuousToggle_->toInfo();
	setpoint.setValue(enableContinuous == true ? 1 : 0);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);

	AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, continuousToggle_);

	if(!action)
		return 0; //NULL

	return action;
}

AMBeamlineActionItem* CLSSIS3820Scaler::createDwellTimeAction(double dwellTime) {

	if(!isConnected())
		return 0; //NULL

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(dwellTime_);

	if(!action)
		return 0; //NULL

	action->setSetpoint(dwellTime*1000);

	return action;
}

AMAction3* CLSSIS3820Scaler::createDwellTimeAction3(double dwellTime) {
	if(!isConnected())
		return 0; //NULL

	AMControlInfo setpoint = dwellTime_->toInfo();
	setpoint.setValue(dwellTime*1000);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);

	AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, dwellTime_);

	if(!action)
		return 0; //NULL

	return action;
}

AMBeamlineActionItem* CLSSIS3820Scaler::createScansPerBufferAction(int scansPerBuffer) {

	if(!isConnected())
		return 0; //NULL

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(scanPerBuffer_);

	if(!action)
		return 0; //NULL

	action->setSetpoint(scansPerBuffer);

	return action;
}

AMAction3* CLSSIS3820Scaler::createScansPerBufferAction3(int scansPerBuffer) {
	if(!isConnected())
		return 0; //NULL

	AMControlInfo setpoint = scanPerBuffer_->toInfo();
	setpoint.setValue(scansPerBuffer);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);

	AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, scanPerBuffer_);

	if(!action)
		return 0; //NULL

	return action;
}

AMBeamlineActionItem* CLSSIS3820Scaler::createTotalScansAction(int totalScans) {

	if(!isConnected())
		return 0; //NULL

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(totalScans_);

	if(!action)
		return 0; //NULL

	action->setSetpoint(totalScans);

	return action;
}

AMAction3* CLSSIS3820Scaler::createTotalScansAction3(int totalScans) {
	if(!isConnected())
		return 0; //NULL

	AMControlInfo setpoint = totalScans_->toInfo();
	setpoint.setValue(totalScans);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);

	AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, totalScans_);

	if(!action)
		return 0; //NULL

	return action;
}

void CLSSIS3820Scaler::setScanning(bool isScanning){

	if(!isConnected())
		return;

	if(isScanning && startToggle_->withinTolerance(0))
		startToggle_->move(1);

	else if(!isScanning && startToggle_->withinTolerance(1))
		startToggle_->move(0);
}

void CLSSIS3820Scaler::setContinuous(bool isContinuous){

	if(!isConnected())
		return;

	if(isContinuous && continuousToggle_->withinTolerance(0))
		continuousToggle_->move(1);

	else if(!isContinuous && continuousToggle_->withinTolerance(1))
		continuousToggle_->move(0);
}

void CLSSIS3820Scaler::setDwellTime(double dwellTime){

	if(!isConnected())
		return;

	if(!dwellTime_->withinTolerance(dwellTime*1000))
		dwellTime_->move(dwellTime*1000);
}

void CLSSIS3820Scaler::setScansPerBuffer(int scansPerBuffer){

	if(!isConnected())
		return;

	if(!scanPerBuffer_->withinTolerance((double)scansPerBuffer))
		scanPerBuffer_->move(scansPerBuffer);
}

void CLSSIS3820Scaler::setTotalScans(int totalScans){

	if(!isConnected())
		return;

	if(!totalScans_->withinTolerance((double)totalScans))
		totalScans_->move(totalScans);
}

void CLSSIS3820Scaler::onScanningToggleChanged(){

	if(!isConnected())
		return;

	if(startToggle_->withinTolerance(1))
		emit scanningChanged(true);

	else{
		emit scanningChanged(false);
		triggerSource_->setSucceeded();
	}
}

void CLSSIS3820Scaler::onContinuousToggleChanged(){

	if(!isConnected())
		return;

	if(continuousToggle_->withinTolerance(1))
		emit continuousChanged(true);

	else
		emit continuousChanged(false);
}

void CLSSIS3820Scaler::onDwellTimeChanged(double time)
{
	if (!isConnected())
		return;

	emit dwellTimeChanged(time/1000);
	dwellTimeSource_->setSucceeded();
}

void CLSSIS3820Scaler::onScanPerBufferChanged(double scansPerBuffer){

	if(!isConnected())
		return;

	emit scansPerBufferChanged((int)scansPerBuffer);
}

void CLSSIS3820Scaler::onTotalScansChanged(double totalScans){

//	if(!isConnected())
//		return;

	emit totalScansChanged((int)totalScans);
}

void CLSSIS3820Scaler::onConnectedChanged(){
	if(isConnected() && !connectedOnce_)
		connectedOnce_ = true;

	if(connectedOnce_)
		emit connectedChanged(isConnected());
}

void CLSSIS3820Scaler::onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode){
	if(!isConnected() || isScanning() || readMode != AMDetectorDefinitions::SingleRead)
		return;

	//setScanning(true);
	if(isContinuous()){
		connect(this, SIGNAL(continuousChanged(bool)), this, SLOT(triggerScalerAcquisition(bool)));
		setContinuous(false);
	}
	else
		triggerScalerAcquisition(isContinuous());
}

bool CLSSIS3820Scaler::triggerScalerAcquisition(bool isContinuous){
	disconnect(this, SIGNAL(continuousChanged(bool)), this, SLOT(triggerScalerAcquisition(bool)));
	if(isContinuous)
		return false;

	setScanning(true);
	return true;
}

void CLSSIS3820Scaler::onDwellTimeSourceSetDwellTime(double dwellSeconds){
	if(!isConnected() || isScanning())
		return;

	setDwellTime(dwellSeconds);
}

// CLSSIS3820ScalarChannel
/////////////////////////////////////////////

CLSSIS3820ScalerChannel::CLSSIS3820ScalerChannel(const QString &baseName, int index, QObject *parent) :
	QObject(parent)
{
	QString fullBaseName = QString("%1%2").arg(baseName).arg(index, 2, 10, QChar('0'));

	customChannelName_ = QString();

	index_ = index;

	channelEnable_ = new AMPVControl(QString("Channel%1Enable").arg(index), fullBaseName+":enable", fullBaseName+":enable", QString(), this, 0.1);
	channelReading_ = new AMReadOnlyPVControl(QString("Channel%1Reading").arg(index), fullBaseName+":fbk", this);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(channelEnable_);
	allControls_->addControl(channelReading_);

	connect(channelEnable_, SIGNAL(valueChanged(double)), this, SLOT(onChannelEnabledChanged()));
	connect(channelReading_, SIGNAL(valueChanged(double)), this, SLOT(onChannelReadingChanged(double)));
	connect(allControls_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
}

bool CLSSIS3820ScalerChannel::isConnected() const{
	return allControls_->isConnected();
}

bool CLSSIS3820ScalerChannel::isEnabled() const{

	return isConnected() && channelEnable_->withinTolerance(1);
}

int CLSSIS3820ScalerChannel::reading() const{

	if(isConnected())
		return int(channelReading_->value());

	return -1;
}

AMBeamlineActionItem* CLSSIS3820ScalerChannel::createEnableAction(bool setEnabled){

	if(!isConnected())
		return 0; //NULL

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(channelEnable_);

	if(!action)
		return 0; //NULL

	action->setSetpoint(setEnabled == true ? 1 : 0);

	return action;
}

void CLSSIS3820ScalerChannel::setEnabled(bool isEnabled){

	if(isEnabled && channelEnable_->withinTolerance(0))
		channelEnable_->move(1);

	else if(!isEnabled && channelEnable_->withinTolerance(1))
		channelEnable_->move(0);
}

void CLSSIS3820ScalerChannel::setCustomChannelName(const QString &customChannelName){
	customChannelName_ = customChannelName;
}

void CLSSIS3820ScalerChannel::onChannelEnabledChanged(){

	emit enabledChanged(channelEnable_->withinTolerance(1));
}

void CLSSIS3820ScalerChannel::onChannelReadingChanged(double reading){

	emit readingChanged((int)reading);
}
