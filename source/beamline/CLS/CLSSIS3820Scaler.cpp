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


#include "CLSSIS3820Scaler.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMDetectorTriggerSource.h"
#include "beamline/AMCurrentAmplifier.h"
#include "beamline/CLS/CLSSIS3820ScalerModeControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMControlWaitAction.h"
#include "util/AMErrorMonitor.h"

#include <QStringBuilder>

#include "actions3/actions/CLSSIS3820ScalerDarkCurrentMeasurementAction.h"


// CLSSIS3820Scalar
/////////////////////////////////////////////

CLSSIS3820Scaler::CLSSIS3820Scaler(const QString &baseName, QObject *parent) :
	QObject(parent)
{
	connectedOnce_ = false;
	switchingReadModes_ = false;
	triggerSourceTriggered_ = false;

	triggerChannelMapper_ = new QSignalMapper(this);

	triggerSource_ = new AMDetectorTriggerSource(QString("%1TriggerSource").arg(baseName), this);
	connect(triggerSource_, SIGNAL(triggered(AMDetectorDefinitions::ReadMode)), this, SLOT(onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode)));

	dwellTimeSource_ = new AMDetectorDwellTimeSource(QString("%1DwellTimeSource").arg(baseName), this);
	connect(dwellTimeSource_, SIGNAL(setDwellTime(double)), this, SLOT(onDwellTimeSourceSetDwellTime(double)));

	synchronizedDwellKey_ = QString("%1:startScan NPP NMS").arg(baseName);

	CLSSIS3820ScalerChannel *tmpChannel;
	for (int x = 0; x < 32; x++) {
		tmpChannel = new CLSSIS3820ScalerChannel(baseName, x, this);
		scalerChannels_.append(tmpChannel);
		connect(tmpChannel, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
		connect(tmpChannel, SIGNAL(sensitivityChanged()), this, SIGNAL(sensitivityChanged()));
		connect(tmpChannel, SIGNAL(readingChanged(int)), triggerChannelMapper_, SLOT(map()));
	}

	startToggle_ = new AMPVControl("Start/Scanning", baseName+":startScan", baseName+":startScan", QString(), this, 0.1);
	dwellTime_ = new AMPVControl("DwellTime", baseName+":delay", baseName+":delay", QString(), this, 1);
	scanPerBuffer_ = new AMPVControl("ScanPerBuffer", baseName+":nscan", baseName+":nscan", QString(), this, 0.5);
	totalScans_ = new AMPVControl("TotalScans", baseName+":scanCount", baseName+":scanCount", QString(), this, 0.5);

	continuousToggle_ = new CLSSIS3820ScalerModeControl("Mode", QString(), this, "SIS3820 Scaler Mode Control");
	continuousToggle_->setScanCountControl(totalScans_);
	continuousToggle_->setNumberOfScansPerBufferControl(scanPerBuffer_);
	continuousToggle_->setStartScanControl(startToggle_);

	reading_ = new AMReadOnlyPVControl("Reading", baseName+":scan", this);

	dwellTime_->setAttemptMoveWhenWithinTolerance(false);

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
	/*
	connect(reading_, SIGNAL(valueChanged(double)), this, SIGNAL(readingChanged()));
	*/
	connect(reading_, SIGNAL(valueChanged(double)), this, SLOT(onReadingChanged(double)));
	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
}

CLSSIS3820Scaler::~CLSSIS3820Scaler(){}

bool CLSSIS3820Scaler::isConnected() const{

	bool retVal = true;

	for(int x = 0; x < scalerChannels_.count(); x++)
		retVal &= scalerChannels_.at(x)->isConnected();

	return retVal && allControls_->isConnected();
}

bool CLSSIS3820Scaler::isScanning() const{

	return isConnected() && startToggle_->withinTolerance(1);
}

bool CLSSIS3820Scaler::isContinuous() const{

	return isConnected() && continuousToggle_->withinTolerance(1);
}

double CLSSIS3820Scaler::dwellTime() const
{
	if(isConnected())
		return dwellTime_->value()/1000;

	return -1;
}

double CLSSIS3820Scaler::dwellTimeTolerance() const
{
	if (isConnected())
		return dwellTime_->tolerance();

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

int CLSSIS3820Scaler::enabledChannelCount() const{
	int retVal = 0;
	for(int x = 0; x < scalerChannels_.count(); x++)
		if(scalerChannels_.at(x)->isEnabled())
			retVal++;

	return retVal;
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

AMAction3* CLSSIS3820Scaler::createStartAction3(bool setScanning){
	if(!isConnected())
		return 0; //NULL

	AMAction3 *action = AMActionSupport::buildControlMoveAction(startToggle_, setScanning ? 1 : 0);

	if(!action)
		return 0; //NULL

	return action;
}

AMAction3* CLSSIS3820Scaler::createContinuousEnableAction3(bool enableContinuous){
	if(!isConnected())
		return 0; //NULL

	AMAction3 *action = AMActionSupport::buildControlMoveAction(continuousToggle_, enableContinuous ? CLSSIS3820ScalerModeControl::Mode::Continuous : CLSSIS3820ScalerModeControl::Mode::SingleShot);
	if(!action)
		return 0; //NULL

	return action;
}

AMAction3* CLSSIS3820Scaler::createDwellTimeAction3(double dwellTime)
{
	if(!isConnected())
		return 0; //NULL

	AMAction3 *action = 0;

	if (!dwellTime_->withinTolerance(dwellTime * 1000))
		action = AMActionSupport::buildControlMoveAction(dwellTime_, dwellTime*1000);

	return action;
}

AMAction3* CLSSIS3820Scaler::createScansPerBufferAction3(int scansPerBuffer) {
	if(!isConnected())
		return 0; //NULL

	AMAction3 *action = AMActionSupport::buildControlMoveAction(scanPerBuffer_, double(scansPerBuffer));
	if(!action)
		return 0; //NULL

	return action;
}

AMAction3* CLSSIS3820Scaler::createTotalScansAction3(int totalScans) {
	if(!isConnected())
		return 0; //NULL

	AMAction3 *action = AMActionSupport::buildControlMoveAction(totalScans_, double(totalScans));
	if(!action)
		return 0; //NULL

	return action;
}


AMAction3* CLSSIS3820Scaler::createWaitForDwellFinishedAction(double timeoutTime)
{
	if(!isConnected())
		return 0; //NULL

	AMControlInfo setpoint = startToggle_->toInfo();
	setpoint.setValue(0);
	AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(setpoint, timeoutTime , AMControlWaitActionInfo::MatchEqual);
	AMControlWaitAction *action = new AMControlWaitAction(actionInfo, startToggle_);

	if(!action)
		return 0; //NULL

	return action;
}

AMAction3* CLSSIS3820Scaler::createMeasureDarkCurrentAction(int secondsDwell)
{
	return new CLSSIS3820ScalerDarkCurrentMeasurementAction(new CLSSIS3820ScalerDarkCurrentMeasurementActionInfo(secondsDwell));
}

bool CLSSIS3820Scaler::requiresArming()
{
	return false;
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

	if(isContinuous && continuousToggle_->withinTolerance(CLSSIS3820ScalerModeControl::Mode::SingleShot))
		continuousToggle_->move(CLSSIS3820ScalerModeControl::Mode::Continuous);

	else if(!isContinuous && continuousToggle_->withinTolerance(CLSSIS3820ScalerModeControl::Mode::Continuous))
		continuousToggle_->move(CLSSIS3820ScalerModeControl::Mode::SingleShot);
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

void CLSSIS3820Scaler::measureDarkCurrent(int secondsDwell)
{
	AMAction3 *action = createMeasureDarkCurrentAction(secondsDwell);

	if (action) {
		connect( action, SIGNAL(cancelled()), action, SLOT(deleteLater()) );
		connect( action, SIGNAL(failed()), action, SLOT(deleteLater()) );
		connect( action, SIGNAL(succeeded()), action, SLOT(deleteLater()) );

		action->start();
	}
}

void CLSSIS3820Scaler::arm()
{

}

void CLSSIS3820Scaler::onScanningToggleChanged(){

	if(!isConnected())
		return;

	emit scanningChanged(startToggle_->withinTolerance(1));
}

void CLSSIS3820Scaler::onContinuousToggleChanged()
{
	if(!isConnected())
		return;

	emit continuousChanged(continuousToggle_->withinTolerance(CLSSIS3820ScalerModeControl::Mode::Continuous));
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
	emit totalScansChanged((int)totalScans);
}

void CLSSIS3820Scaler::onConnectedChanged(){
	if(isConnected() && !connectedOnce_) {
		connectedOnce_ = true;
	}

	if(connectedOnce_)
		emit connectedChanged(isConnected());
}

void CLSSIS3820Scaler::onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode){
	if(!isConnected() || isScanning())
		return;

	readModeForTriggerSource_ = readMode;
	if(isContinuous()){
		if(readModeForTriggerSource_ == readModeFromSettings())
			connect(this, SIGNAL(continuousChanged(bool)), this, SLOT(triggerScalerAcquisition(bool)));
		else
			connect(this, SIGNAL(continuousChanged(bool)), this, SLOT(ensureCorrectReadModeForTriggerSource()));
		setContinuous(false);
	}
	else if(readModeForTriggerSource_ != readModeFromSettings())
		ensureCorrectReadModeForTriggerSource();
	else
		triggerScalerAcquisition(isContinuous());
}

void CLSSIS3820Scaler::ensureCorrectReadModeForTriggerSource(){
	if(readModeForTriggerSource_ == AMDetectorDefinitions::SingleRead){
		connect(this, SIGNAL(dwellTimeChanged(double)), this, SLOT(onModeSwitchSignal()));
		connect(this, SIGNAL(scansPerBufferChanged(int)), this, SLOT(onModeSwitchSignal()));
		connect(this, SIGNAL(totalScansChanged(int)), this, SLOT(onModeSwitchSignal()));

		switchingReadModes_ = true;
		setScansPerBuffer(1);
		setTotalScans(1);
		setDwellTime(1.0);
	}
	else{
		connect(this, SIGNAL(dwellTimeChanged(double)), this, SLOT(onModeSwitchSignal()));
		connect(this, SIGNAL(scansPerBufferChanged(int)), this, SLOT(onModeSwitchSignal()));
		connect(this, SIGNAL(totalScansChanged(int)), this, SLOT(onModeSwitchSignal()));

		switchingReadModes_ = true;
		setScansPerBuffer(1000);
		setTotalScans(1000);
		setDwellTime(0.005);
	}
}

void CLSSIS3820Scaler::onModeSwitchSignal(){
	if(switchingReadModes_){
		if(readModeForTriggerSource_ == AMDetectorDefinitions::SingleRead && scansPerBuffer() == 1 && totalScans() == 1)
			switchingReadModes_ = false;
		else if(readModeForTriggerSource_ == AMDetectorDefinitions::ContinuousRead && scansPerBuffer() == 1000 && totalScans() == 1000)
			switchingReadModes_ = false;

		if(!switchingReadModes_){
			disconnect(this, SIGNAL(dwellTimeChanged(double)), this, SLOT(onModeSwitchSignal()));
			disconnect(this, SIGNAL(scansPerBufferChanged(int)), this, SLOT(onModeSwitchSignal()));
			disconnect(this, SIGNAL(totalScansChanged(int)), this, SLOT(onModeSwitchSignal()));

			triggerScalerAcquisition(isContinuous());
		}
	}
}

bool CLSSIS3820Scaler::triggerScalerAcquisition(bool isContinuous)
{
	disconnect(this, SIGNAL(continuousChanged(bool)), this, SLOT(triggerScalerAcquisition(bool)));
	if(isContinuous)
		return false;

	triggerSourceTriggered_ = true;

	for(int x = 0, size = scalerChannels_.count(); x < size; x++){
		if(scalerChannels_.at(x)->isEnabled()){

			waitingChannels_.append(x);
			triggerChannelMapper_->setMapping(scalerChannels_.at(x), x);
		}
	}

	connect(triggerChannelMapper_, SIGNAL(mapped(int)), this, SLOT(onChannelReadingChanged(int)));
	connect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(triggerAcquisitionFinished()));

	setScanning(true);
	return true;
}

void CLSSIS3820Scaler::onReadingChanged(double value)
{
	Q_UNUSED(value)
	emit readingChanged();

	if(triggerSourceTriggered_ && waitingChannels_.count() == 0){

		triggerSourceTriggered_ = false;
		triggerSource_->setSucceeded();
	}
}

void CLSSIS3820Scaler::onChannelReadingChanged(int channelIndex)
{
	if(waitingChannels_.contains(channelIndex)){

		waitingChannels_.removeAll(channelIndex);
		triggerChannelMapper_->removeMappings(channelAt(channelIndex));
	}

	triggerAcquisitionFinished();
}

void CLSSIS3820Scaler::triggerAcquisitionFinished()
{
	if(triggerSourceTriggered_ && waitingChannels_.count() == 0 && !isScanning()){

		triggerSourceTriggered_ = false;
		disconnect(triggerChannelMapper_, SIGNAL(mapped(int)), this, SLOT(onChannelReadingChanged(int)));
		disconnect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(triggerAcquisitionFinished()));
		triggerSource_->setSucceeded();
	}
}

void CLSSIS3820Scaler::onDwellTimeSourceSetDwellTime(double dwellSeconds){

	if(!isConnected() || isScanning()){

		AMErrorMon::alert(this, CLSSIS3820SCALER_NOT_CONNECTED_OR_IS_SCANNING, QString("Scaler is either not connected (%1) or is scanning (%2) with a requested dwell time %3.\n").arg(!isConnected()).arg(isScanning()).arg(dwellSeconds));
		return;
	}

	if(!(fabs(dwellSeconds - dwellTime()) < dwellTimeTolerance()))
		setDwellTime(dwellSeconds);
	else
		dwellTimeSource_->setSucceeded();
}

AMDetectorDefinitions::ReadMode CLSSIS3820Scaler::readModeFromSettings(){
	if(scansPerBuffer() == 1 && totalScans() == 1)
		return AMDetectorDefinitions::SingleRead;
	else if(scansPerBuffer() == 1000 && totalScans() == 1000)
		return AMDetectorDefinitions::ContinuousRead;
	else
		return AMDetectorDefinitions::InvalidReadMode;
}

// CLSSIS3820ScalarChannel
/////////////////////////////////////////////

CLSSIS3820ScalerChannel::CLSSIS3820ScalerChannel(const QString &baseName, int index, QObject *parent) :
	QObject(parent)
{
	QString fullBaseName = QString("%1%2").arg(baseName).arg(index, 2, 10, QChar('0'));

	wasConnected_ = false;

	// No SR570 or detector to start with.
	currentAmplifier_ = 0;
	voltageRange_ = AMRange();
	detector_ = 0;

	customChannelName_ = QString();

	index_ = index;

	channelEnable_ = new AMPVControl(QString("Channel%1Enable").arg(index), fullBaseName%":enable", fullBaseName+":enable", QString(), this, 0.1);
	channelReading_ = new AMReadOnlyPVControl(QString("Channel%1Reading").arg(index), fullBaseName%":fbk", this);
	channelVoltage_ = new AMReadOnlyPVControl(QString("Channel%1Voltage").arg(index), fullBaseName%":userRate", this);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(channelEnable_);
	allControls_->addControl(channelReading_);
	allControls_->addControl(channelVoltage_);

	connect(channelEnable_, SIGNAL(valueChanged(double)), this, SLOT(onChannelEnabledChanged()));
	connect(channelReading_, SIGNAL(valueChanged(double)), this, SLOT(onChannelReadingChanged(double)));
	connect(channelVoltage_, SIGNAL(valueChanged(double)), this, SIGNAL(voltageChanged(double)));
	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
}

CLSSIS3820ScalerChannel::~CLSSIS3820ScalerChannel(){}

bool CLSSIS3820ScalerChannel::isConnected() const
{
	if (currentAmplifier_)
		return allControls_->isConnected() && currentAmplifier_->isConnected();

	else
		return allControls_->isConnected();
}

bool CLSSIS3820ScalerChannel::isEnabled() const{

	return isConnected() && channelEnable_->withinTolerance(1);
}

int CLSSIS3820ScalerChannel::reading() const{

//	if(isConnected())
		return int(channelReading_->value());

	return -1;
}

double CLSSIS3820ScalerChannel::voltage() const
{
//	if (isConnected())
		return channelVoltage_->value();

	return -1.0;
}

void CLSSIS3820ScalerChannel::onConnectedChanged()
{
	if (wasConnected_ != isConnected())
		emit connected(wasConnected_ = isConnected());
}

AMAction3* CLSSIS3820ScalerChannel::createEnableAction3(bool setEnabled){
	if(!isConnected())
		return 0; //NULL

	AMAction3 *action = AMActionSupport::buildControlMoveAction(channelEnable_, setEnabled ? 1 : 0);
	if(!action)
		return 0; //NULL

	return action;
}

void CLSSIS3820ScalerChannel::setEnabled(bool isEnabled){

	if(isEnabled && channelEnable_->withinTolerance(0))
		channelEnable_->move(1);

	else if(!isEnabled && channelEnable_->withinTolerance(1))
		channelEnable_->move(0);
}

void CLSSIS3820ScalerChannel::setCustomChannelName(const QString &customChannelName)
{
	emit customNameChanged(customChannelName_ = customChannelName);
}

void CLSSIS3820ScalerChannel::onChannelEnabledChanged()
{
	emit enabledChanged(channelEnable_->withinTolerance(1));
}

void CLSSIS3820ScalerChannel::onChannelReadingChanged(double reading)
{
	emit readingChanged((int)reading);
}

void CLSSIS3820ScalerChannel::setCurrentAmplifier(AMCurrentAmplifier *amplifier)
{
	if (currentAmplifier_) {
		disconnect( currentAmplifier_, SIGNAL(isConnected(bool)), this, SLOT(onConnectedChanged()));
		disconnect( currentAmplifier_, SIGNAL(valueChanged()), this, SIGNAL(sensitivityChanged()) );
	}

	currentAmplifier_ = amplifier;
	connect( currentAmplifier_, SIGNAL(isConnected(bool)), this, SLOT(onConnectedChanged()) );
	connect( currentAmplifier_, SIGNAL(valueChanged()), this, SIGNAL(sensitivityChanged()) );
	emit currentAmplifierAttached();
}

void CLSSIS3820ScalerChannel::setMinimumVoltage(double min)
{
	if (voltageRange_.minimum() != min){

		voltageRange_.setMaximum(min);
		emit voltageRangeChanged(voltageRange_);
	}
}

void CLSSIS3820ScalerChannel::setMaximumVoltage(double max)
{
	if (voltageRange_.maximum() != max){

		voltageRange_.setMaximum(max);
		emit voltageRangeChanged(voltageRange_);
	}
}

void CLSSIS3820ScalerChannel::setVoltagRange(const AMRange &range)
{
	if (voltageRange_ != range){

		voltageRange_ = range;
		emit voltageRangeChanged(voltageRange_);
	}
}

void CLSSIS3820ScalerChannel::setVoltagRange(double min, double max)
{
	setVoltagRange(AMRange(min, max));
}

void CLSSIS3820ScalerChannel::setDetector(AMDetector *detector)
{
	if (detector_ != detector) {
		detector_ = detector;
		emit detectorChanged(detector_);
	}
}
