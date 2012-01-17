#include "CLSSIS3820Scaler.h"

#include "actions/AMBeamlineControlMoveAction.h"

CLSSIS3820Scaler::CLSSIS3820Scaler(const QString &baseName, QObject *parent) :
	QObject(parent)
{
	connectedOnce_ = false;
	CLSSIS3820ScalerChannel *tmpChannel;
	for(int x = 0; x < 32; x++){
		tmpChannel = new CLSSIS3820ScalerChannel(baseName, x, this);
		scalerChannels_.append(tmpChannel);
		connect(tmpChannel, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged(bool)));
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

	connect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(onScanningToggleChanged(double)));
	connect(continuousToggle_, SIGNAL(valueChanged(double)), this, SLOT(onContinuousToggleChanged(double)));
	connect(dwellTime_, SIGNAL(valueChanged(double)), this, SIGNAL(dwellTimeChanged(double)));
	connect(scanPerBuffer_, SIGNAL(valueChanged(double)), this, SLOT(onScanPerBufferChanged(double)));
	connect(totalScans_, SIGNAL(valueChanged(double)), this, SLOT(onTotalScansChanged(double)));
	connect(qobject_cast<AMReadOnlyPVControl*>(reading_)->readPV(), SIGNAL(valueChanged()), this, SLOT(onReadingChanged()));
	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged(bool)));
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
		return dwellTime_->value();
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

AMControl* CLSSIS3820Scaler::startToggleControl(){
	if(isConnected())
		return startToggle_;
	return 0; //NULL
}

AMControl* CLSSIS3820Scaler::continuousToggleControl(){
	if(isConnected())
		return continuousToggle_;
	return 0; //NULL
}

AMControl* CLSSIS3820Scaler::dwellTimeControl(){
	if(isConnected())
		return dwellTime_;
	return 0; //NULL
}

AMControl* CLSSIS3820Scaler::scansPerBufferControl(){
	if(isConnected())
		return scanPerBuffer_;
	return 0; //NULL
}

AMControl* CLSSIS3820Scaler::totalScansControl(){
	if(isConnected())
		return totalScans_;
	return 0; //NULL
}

AMBeamlineActionItem* CLSSIS3820Scaler::createStartAction(bool setScanning) {
	if(!isConnected())
		return 0; //NULL
	double setpoint;
	if(setScanning)
		setpoint = 1;
	else
		setpoint = 0;
	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(startToggle_);
	if(!action)
		return 0; //NULL
	action->setSetpoint(setpoint);
	return action;
}

AMBeamlineActionItem* CLSSIS3820Scaler::createContinuousEnableAction(bool enableContinuoius) {
	if(!isConnected())
		return 0; //NULL
	double setpoint;
	if(enableContinuoius)
		setpoint = 1;
	else
		setpoint = 0;
	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(continuousToggle_);
	if(!action)
		return 0; //NULL
	action->setSetpoint(setpoint);
	return action;
}

AMBeamlineActionItem* CLSSIS3820Scaler::createDwellTimeAction(double dwellTime) {
	if(!isConnected())
		return 0; //NULL
	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(dwellTime_);
	if(!action)
		return 0; //NULL
	action->setSetpoint(dwellTime);
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

AMBeamlineActionItem* CLSSIS3820Scaler::createTotalScansAction(int totalScans) {
	if(!isConnected())
		return 0; //NULL
	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(totalScans_);
	if(!action)
		return 0; //NULL
	action->setSetpoint(totalScans);
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
	if(!dwellTime_->withinTolerance(dwellTime))
		dwellTime_->move(dwellTime);
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

void CLSSIS3820Scaler::onScanningToggleChanged(double isScanning){
	Q_UNUSED(isScanning)
	if(!isConnected())
		return;
	if(startToggle_->withinTolerance(1))
		emit scanningChanged(true);
	else
		emit scanningChanged(false);
}

void CLSSIS3820Scaler::onContinuousToggleChanged(double isContinuous){
	Q_UNUSED(isContinuous)
	if(!isConnected())
		return;
	if(continuousToggle_->withinTolerance(1))
		emit continuousChanged(true);
	else
		emit continuousChanged(false);
}

void CLSSIS3820Scaler::onScanPerBufferChanged(double scansPerBuffer){
	if(!isConnected())
		return;
	emit scansPerBufferChanged((int)scansPerBuffer);
}

void CLSSIS3820Scaler::onTotalScansChanged(double totalScans){
	if(!isConnected())
		return;
	emit totalScansChanged((int)totalScans);
}

void CLSSIS3820Scaler::onReadingChanged(){
	if(!isConnected())
		return;
	emit readingChanged(qobject_cast<AMReadOnlyPVControl*>(reading_)->readPV()->lastIntegerValues());
}

void CLSSIS3820Scaler::onConnectedChanged(bool connected){
	Q_UNUSED(connected)
	if(isConnected() && !connectedOnce_)
		connectedOnce_ = true;
	if(connectedOnce_)
		emit connectedChanged(isConnected());
}

CLSSIS3820ScalerChannel::CLSSIS3820ScalerChannel(const QString &baseName, int index, QObject *parent) :
	QObject(parent)
{
	QString fullBaseName = QString("%1%2").arg(baseName).arg(index, 2, 10, QChar('0'));

	channelEnable_ = new AMPVControl(QString("Channel%1Enable").arg(index), fullBaseName+":enable", fullBaseName+":enable", QString(), this, 0.1);
	channelReading_ = new AMReadOnlyPVControl(QString("Channel%1Reading").arg(index), fullBaseName+":fbk", this);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(channelEnable_);
	allControls_->addControl(channelReading_);

	connect(channelEnable_, SIGNAL(valueChanged(double)), this, SLOT(onChannelEnabledChanged(double)));
	connect(channelReading_, SIGNAL(valueChanged(double)), this, SLOT(onChannelReadingChanged(double)));
	connect(allControls_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
}

bool CLSSIS3820ScalerChannel::isConnected() const{
	return allControls_->isConnected();
}

bool CLSSIS3820ScalerChannel::isEnabled() const{
	if(isConnected() && channelEnable_->withinTolerance(1))
		return true;
	return false;
}

int CLSSIS3820ScalerChannel::reading() const{
	if(isConnected())
		return channelReading_->value();
	return -1;
}

AMBeamlineActionItem* CLSSIS3820ScalerChannel::createEnableAction(bool setEnabled){
	if(!isConnected())
		return 0; //NULL
	double setpoint;
	if(setEnabled)
		setpoint = 1;
	else
		setpoint = 0;
	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(channelEnable_);
	if(!action)
		return 0; //NULL
	action->setSetpoint(setpoint);
	return action;
}

void CLSSIS3820ScalerChannel::setEnabled(bool isEnabled){
	if(isEnabled && channelEnable_->withinTolerance(0))
		channelEnable_->move(1);
	else if(!isEnabled && channelEnable_->withinTolerance(1))
		channelEnable_->move(0);
}

void CLSSIS3820ScalerChannel::onChannelEnabledChanged(double enabled){
	Q_UNUSED(enabled)
	if(channelEnable_->withinTolerance(1))
		emit enabledChanged(true);
	else
		emit enabledChanged(false);
}

void CLSSIS3820ScalerChannel::onChannelReadingChanged(double reading){
	emit readingChanged((int)reading);
	QString readingString = QString("%1").arg(reading);
	emit readingChanged(readingString);
}
