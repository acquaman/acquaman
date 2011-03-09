#include "PGTDetector.h"

PGTDetector::PGTDetector(const QString &name, AMControlSet *controls, QObject *parent) :
		PGTDetectorInfo(name, name, parent), AMDetector(name)
{
	ownsControlSet_ = false;
	initializeFromControlSet(controls);
}

PGTDetector::PGTDetector(const QString &name, AMControl *dataWaveform, AMControl *hvSetpoint, AMControl *hvFbk, AMControl *integrationTime, AMControl *integrationMode, QObject *parent) :
		PGTDetectorInfo(name, name, parent), AMDetector(name)
{
	ownsControlSet_ = true;
	AMControlSet *lControls = new AMControlSet(this);
	lControls->addControl(dataWaveform);
	lControls->addControl(hvSetpoint);
	lControls->addControl(hvFbk);
	lControls->addControl(integrationTime);
	lControls->addControl(integrationMode);
	initializeFromControlSet(lControls);
	/*
	dataWaveform_ = dataWaveform;
	hvSetpoint_ = hvSetpoint;
	hvFbk_ = hvFbk;
	integrationTime_ = integrationTime;
	integrationMode_ = integrationMode;

	if(hvSetpoint_->isConnected())
		setHVSetpoint(hvSetpoint_->value());
	if(integrationTime_->isConnected())
		setIntegrationTime(integrationTime_->value());
	if(integrationMode_->isConnected())
		setIntegrationMode(integrationMode_->enumNameAt(integrationMode_->value()));

	connect(dataWaveform_, SIGNAL(connected(bool)), AMDetector::signalSource(), SIGNAL(connected(bool)));
	connect(dataWaveform_, SIGNAL(valueChanged(double)), AMDetector::signalSource(), SIGNAL(valuesChanged()));
	*/
}

PGTDetector::~PGTDetector()
{
	/*
	dataWaveform_ = NULL;
	hvSetpoint_ = NULL;
	hvFbk_ = NULL;
	integrationTime_ = NULL;
	integrationMode_ = NULL;
	*/
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
	integrationTimeCtrl()->move(di->integrationTime());
	integrationModeCtrl()->move(integrationModeCtrl()->enumNames().indexOf(di->integrationMode()));
	hvSetpointCtrl()->move(di->hvSetpoint());
	return true;
}

AMControl* PGTDetector::dataWaveformCtrl() const {
	if(controls_->isConnected())
		return controls_->at(0);
	else
		return 0;
}

AMControl* PGTDetector::hvSetpointCtrl() const {
	if(controls_->isConnected())
		return controls_->at(1);
	else
		return 0;
}

AMControl* PGTDetector::hvFbkCtrl() const {
	if(controls_->isConnected())
		return controls_->at(2);
	else
		return 0;
}

AMControl* PGTDetector::integrationTimeCtrl() const {
	if(controls_->isConnected())
		return controls_->at(3);
	else
		return 0;
}

AMControl* PGTDetector::integrationModeCtrl() const {
	if(controls_->isConnected())
		return controls_->at(4);
	else
		return 0;
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

QString PGTDetector::description() const{
	return AMDetectorInfo::description();
}

void PGTDetector::setDescription(const QString &description){
	AMDetectorInfo::setDescription(description);
}

bool PGTDetector::setControls(PGTDetectorInfo *pgtSettings){
	hvSetpointCtrl()->move( pgtSettings->hvSetpoint() );
	integrationTimeCtrl()->move( pgtSettings->integrationTime() );
	integrationModeCtrl()->move( integrationModeCtrl()->enumNames().indexOf(pgtSettings->integrationMode()) );
	return true;
}

void PGTDetector::onControlsConnected(bool connected){
	if(connected != isConnected())
		setConnected(connected);
}

void PGTDetector::onControlValuesChanged(){
	if(isConnected()){
		setHVSetpoint(hvSetpointCtrl()->value());
		setIntegrationTime(integrationTimeCtrl()->value());
		setIntegrationMode(integrationModeCtrl()->enumNameAt(integrationModeCtrl()->value()));
		emit detectorSettingsChanged();
	}
}

bool PGTDetector::initializeFromControlSet(AMControlSet *controls){
	controls_ = 0; //NULL

	if(controls->count() == 5){
		controls_ = controls;
		connect(signalSource(), SIGNAL(connected(bool)), this, SLOT(onControlValuesChanged()));
		connect(controls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
		connect(controls_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(onControlValuesChanged()));
		onControlsConnected(controls_->isConnected());
	}
}
