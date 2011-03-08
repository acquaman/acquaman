#include "PGTDetector.h"

PGTDetector::PGTDetector(const QString &name, AMControl *dataWaveform, AMControl *hvSetpoint, AMControl *hvFbk, AMControl *integrationTime, AMControl *integrationMode, QObject *parent) :
		PGTDetectorInfo(name, name, parent), AMDetector(name)
{
	dataWaveform_ = dataWaveform;
	hvSetpoint_ = hvSetpoint;
	hvFbk_ = hvFbk;
	integrationTime_ = integrationTime;
	integrationMode_ = integrationMode;
	connect(dataWaveform_, SIGNAL(connected(bool)), AMDetector::signalSource(), SIGNAL(connected(bool)));
	connect(dataWaveform_, SIGNAL(valueChanged(double)), AMDetector::signalSource(), SIGNAL(valuesChanged()));
}

PGTDetector::~PGTDetector()
{
	dataWaveform_ = NULL;
	hvSetpoint_ = NULL;
	hvFbk_ = NULL;
	integrationTime_ = NULL;
	integrationMode_ = NULL;
}

const QMetaObject* PGTDetector::getMetaObject() {
	return metaObject();
}

AMDetectorInfo PGTDetector::toInfo(){
	return PGTDetectorInfo(*this);
}

bool PGTDetector::setFromInfo(const AMDetectorInfo &info){
	const PGTDetectorInfo *di = qobject_cast<const PGTDetectorInfo*>(&info);
	if(!di)
		return false;
	integrationTime_->move(di->integrationTime());
	integrationMode_->move(integrationMode_->enumNames().indexOf(di->integrationMode()));
	hvSetpoint_->move(di->hvSetpoint());
	return true;
}

AMControl* PGTDetector::dataWaveformCtrl() const {
	return dataWaveform_;
}

AMControl* PGTDetector::hvSetpointCtrl() const {
	return hvSetpoint_;
}

AMControl* PGTDetector::hvFbkCtrl() const {
	return hvFbk_;
}

AMControl* PGTDetector::integrationTimeCtrl() const {
	return integrationTime_;
}

AMControl* PGTDetector::integrationModeCtrl() const {
	return integrationMode_;
}



bool PGTDetector::settingsMatchFbk(PGTDetectorInfo *settings){
	bool rVal = false;
	if( fabs(settings->integrationTime() - integrationTimeCtrl()->value()) > integrationTimeCtrl()->tolerance() )
		return rVal;
	else if( fabs( settings->integrationModeList().indexOf(settings->integrationMode()) - integrationModeCtrl()->value()) > integrationModeCtrl()->tolerance() )
		return rVal;
	else if( fabs(settings->hvSetpoint() - hvFbkCtrl()->value()) > hvFbkCtrl()->tolerance())
		return rVal;
	else{
		rVal = true;
		return rVal;
	}
}

bool PGTDetector::setControls(PGTDetectorInfo *pgtSettings){
	hvSetpoint_->move( pgtSettings->hvSetpoint() );
	integrationTime_->move( pgtSettings->integrationTime() );
	integrationMode_->move( integrationMode_->enumNames().indexOf(pgtSettings->integrationMode()) );
	return true;
}

