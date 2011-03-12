#include "MCPDetector.h"

MCPDetector::MCPDetector(const QString &name, AMControlSet *readingsControls, AMControlSet *settingsControls, QObject *parent) :
		MCPDetectorInfo(name, name, parent), AMDetector(name)
{
	ownsControlSets_ = false;
	initializeFromControlSet(readingsControls, settingsControls);
}


MCPDetector::MCPDetector(const QString &name, AMControl *reading, AMControl *hv, QObject *parent) :
		MCPDetectorInfo(name, name, parent), AMDetector(name)
{
	ownsControlSets_ = true;
	AMControlSet *readingsControls = new AMControlSet(this);
	readingsControls->addControl(reading);
	AMControlSet *settingsControls = new AMControlSet(this);
	settingsControls->addControl(hv);
	initializeFromControlSet(readingsControls, settingsControls);
}

MCPDetector::~MCPDetector(){
	/*
	reading_ = NULL;
	hvSetpoint_ = NULL;
	hvFbk_ = NULL;
	*/
}

const QMetaObject* MCPDetector::getMetaObject() {
	return metaObject();
}

AMDetectorInfo* MCPDetector::toInfo() const{
	return new MCPDetectorInfo(*this);
}

MCPDetectorInfo MCPDetector::toMCPInfo() const{
	return MCPDetectorInfo(*this);
}

bool MCPDetector::isPoweredOn(){
	return poweredOn_;
}

AMControl* MCPDetector::readingCtrl() const {
	if(isConnected())
		return readingsControls_->at(0);
	return 0;
}

AMControl* MCPDetector::hvCtrl() const {
	if(isConnected())
		return settingsControls_->at(0);
	return 0;
}

bool MCPDetector::setFromInfo(const AMDetectorInfo *info){
	const MCPDetectorInfo *di = qobject_cast<const MCPDetectorInfo*>(info);
	if(!di)
		return false;
	hvCtrl()->move(di->hvSetpoint());
	return true;
}

bool MCPDetector::setFromInfo(const MCPDetectorInfo& info){
	hvCtrl()->move(info.hvSetpoint());
	return true;
}

bool MCPDetector::settingsMatchFbk(MCPDetectorInfo *settings){
	bool rVal = false;
	if( fabs(settings->hvSetpoint() - hvCtrl()->value()) > hvCtrl()->tolerance())
		return rVal;
	else{
		rVal = true;
		return rVal;
	}
}

QString MCPDetector::description() const{
	return AMDetectorInfo::description();
}

bool MCPDetector::setControls(MCPDetectorInfo *mcpSettings){
	hvCtrl()->move( mcpSettings->hvSetpoint() );
	return true;
}

void MCPDetector::setDescription(const QString &description){
	AMDetectorInfo::setDescription(description);
}

void MCPDetector::onControlsConnected(bool connected){
	bool allConnected = readingsControls_->isConnected() && settingsControls_->isConnected();
	if(allConnected != isConnected())
		setConnected(allConnected);
}

void MCPDetector::onReadingsControlValuesChanged(){
	if(isConnected())
		emitReadingsChanged();
}

void MCPDetector::onSettingsControlValuesChanged(){
	if(isConnected()){
		setHVSetpoint(hvCtrl()->value());
		bool lastPoweredOn = poweredOn_;
		poweredOn_ = (hvCtrl()->value() >= 1200);
		if(poweredOn_ != lastPoweredOn)
			emit poweredOnChanged(poweredOn_);
		emitSettingsChanged();
	}
}

void MCPDetector::initializeFromControlSet(AMControlSet *readingsControls, AMControlSet *settingsControls){
	readingsControls_ = 0; //NULL
	settingsControls_ = 0; //NULL
	poweredOn_ = false;

	if(readingsControls->count() == 1 && settingsControls->count() == 1){
		readingsControls_ = readingsControls;
		settingsControls_ = settingsControls;
		connect(signalSource(), SIGNAL(connected(bool)), this, SLOT(onSettingsControlValuesChanged()));
		connect(readingsControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
		connect(settingsControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
		connect(readingsControls_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(onReadingsControlValuesChanged()));
		connect(settingsControls_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(onSettingsControlValuesChanged()));
		onControlsConnected(readingsControls_->isConnected()&&settingsControls_->isConnected());
		if(isConnected()){
			onReadingsControlValuesChanged();
			onSettingsControlValuesChanged();
		}
	}
}
