#include "MCPDetector.h"

MCPDetector::MCPDetector(const QString &name, AMControlSet *controls, QObject *parent) :
		MCPDetectorInfo(name, name, parent), AMDetector(name)
{
	ownsControlSet_ = false;
	initializeFromControlSet(controls);
}


MCPDetector::MCPDetector(const QString &name, AMControl *reading, AMControl *hvSetpoint, AMControl *hvFbk, QObject *parent) :
		MCPDetectorInfo(name, name, parent), AMDetector(name)
{
	ownsControlSet_ = true;
	AMControlSet *lControls = new AMControlSet(this);
	lControls->addControl(reading);
	lControls->addControl(hvSetpoint);
	lControls->addControl(hvFbk);
	initializeFromControlSet(lControls);
	/*
	reading_ = reading;
	hvSetpoint_ = hvSetpoint;
	hvFbk_ = hvFbk;
	connect(reading_, SIGNAL(connected(bool)), AMDetector::signalSource(), SIGNAL(connected(bool)));
	connect(reading_, SIGNAL(valueChanged(double)), AMDetector::signalSource(), SIGNAL(valuesChanged()));
	*/
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

AMDetectorInfo MCPDetector::toInfo(){
	return MCPDetectorInfo(*this);
}

AMControl* MCPDetector::readingCtrl() const {
	if(controls_->isConnected())
		return controls_->at(0);
}

AMControl* MCPDetector::hvSetpointCtrl() const {
	if(controls_->isConnected())
		return controls_->at(1);
}

AMControl* MCPDetector::hvFbkCtrl() const {
	if(controls_->isConnected())
		return controls_->at(2);
}

bool MCPDetector::setFromInfo(const AMDetectorInfo &info){
	const MCPDetectorInfo *di = qobject_cast<const MCPDetectorInfo*>(&info);
	if(!di)
		return false;
	hvSetpointCtrl()->move(di->hvSetpoint());
	return true;
}

bool MCPDetector::settingsMatchFbk(MCPDetectorInfo *settings){
	bool rVal = false;
	if( fabs(settings->hvSetpoint() - hvFbkCtrl()->value()) > hvFbkCtrl()->tolerance())
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
	hvSetpointCtrl()->move( mcpSettings->hvSetpoint() );
	return true;
}

void MCPDetector::setDescription(const QString &description){
	AMDetectorInfo::setDescription(description);
}

void MCPDetector::onControlsConnected(bool connected){
	if(connected != isConnected())
		setConnected(connected);
}

void MCPDetector::onControlValuesChanged(){
	if(isConnected()){
		setHVSetpoint(hvSetpointCtrl()->value());
	}
}

void MCPDetector::initializeFromControlSet(AMControlSet *controls){
	controls_ = 0; //NULL

	if(controls->count() == 3){
		controls_ = controls;
		connect(signalSource(), SIGNAL(connected(bool)), this, SLOT(onControlValuesChanged()));
		connect(controls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
		connect(controls_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(onControlValuesChanged()));
		onControlsConnected(controls_->isConnected());
	}
}
