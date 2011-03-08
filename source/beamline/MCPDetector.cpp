#include "MCPDetector.h"

MCPDetector::MCPDetector(const QString &name, AMControl *reading, AMControl *hvSetpoint, AMControl *hvFbk, QObject *parent) :
		MCPDetectorInfo(name, name, parent), AMDetector(name)
{
	reading_ = reading;
	hvSetpoint_ = hvSetpoint;
	hvFbk_ = hvFbk;
	connect(reading_, SIGNAL(connected(bool)), AMDetector::signalSource(), SIGNAL(connected(bool)));
	connect(reading_, SIGNAL(valueChanged(double)), AMDetector::signalSource(), SIGNAL(valuesChanged()));
}

MCPDetector::~MCPDetector(){
	reading_ = NULL;
	hvSetpoint_ = NULL;
	hvFbk_ = NULL;
}

const QMetaObject* MCPDetector::getMetaObject() {
	return metaObject();
}

AMDetectorInfo MCPDetector::toInfo(){
	return MCPDetectorInfo(*this);
}

AMControl* MCPDetector::readingCtrl() const {
	return reading_;
}

AMControl* MCPDetector::hvSetpointCtrl() const {
	return hvSetpoint_;
}

AMControl* MCPDetector::hvFbkCtrl() const {
	return hvFbk_;
}

bool MCPDetector::setFromInfo(const AMDetectorInfo &info){
	const MCPDetectorInfo *di = qobject_cast<const MCPDetectorInfo*>(&info);
	if(!di)
		return false;
	hvSetpoint_->move(di->hvSetpoint());
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

bool MCPDetector::setControls(MCPDetectorInfo *mcpSettings){
	hvSetpoint_->move( mcpSettings->hvSetpoint() );
	return true;
}
