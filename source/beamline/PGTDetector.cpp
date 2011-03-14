#include "PGTDetector.h"

PGTDetector::PGTDetector(const QString &name, AMControlSet *readingsControls, AMControlSet *settingsControls, QObject *parent) :
		PGTDetectorInfo(name, name, parent), AMDetector(name)
{
	ownsControlSets_ = false;
	initializeFromControlSet(readingsControls, settingsControls);
}

PGTDetector::PGTDetector(const QString &name, AMControl *dataWaveform, AMControl *hv, AMControl *integrationTime, AMControl *integrationMode, QObject *parent) :
		PGTDetectorInfo(name, name, parent), AMDetector(name)
{
	ownsControlSets_ = true;
	AMControlSet *readingsControls = new AMControlSet(this);
	AMControlSet *settingsControls = new AMControlSet(this);
	readingsControls->addControl(dataWaveform);
	settingsControls->addControl(hv);
	settingsControls->addControl(integrationTime);
	settingsControls->addControl(integrationMode);
	initializeFromControlSet(readingsControls, settingsControls);
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

AMDetectorInfo* PGTDetector::toInfo() const{
	return new PGTDetectorInfo(*this);
}

PGTDetectorInfo PGTDetector::toPGTInfo() const{
	return PGTDetectorInfo(*this);
}

bool PGTDetector::setFromInfo(const AMDetectorInfo *info){
	qDebug() << "Setting PGT to " << *info;
	const PGTDetectorInfo *di = qobject_cast<const PGTDetectorInfo*>(info);
	if(!di)
		return false;
	qDebug() << "Passed the cast test " << di->integrationTime() << integrationModeCtrl()->enumNames().indexOf(di->integrationMode()) << di->hvSetpoint();
	integrationTimeCtrl()->move(di->integrationTime());
	integrationModeCtrl()->move(integrationModeCtrl()->enumNames().indexOf(di->integrationMode()));
	hvCtrl()->move(di->hvSetpoint());
	return true;
}

bool PGTDetector::setFromInfo(const PGTDetectorInfo& info){
	integrationTimeCtrl()->move(info.integrationTime());
	integrationModeCtrl()->move(integrationModeCtrl()->enumNames().indexOf(info.integrationMode()));
	hvCtrl()->move(info.hvSetpoint());
	return true;
}

bool PGTDetector::isPoweredOn(){
	return poweredOn_;
}

AMControl* PGTDetector::dataWaveformCtrl() const {
	if(isConnected())
		return readingsControls_->at(0);
	else
		return 0;
}

AMControl* PGTDetector::hvCtrl() const {
	if(isConnected())
		return settingsControls_->at(0);
	else
		return 0;
}

AMControl* PGTDetector::integrationTimeCtrl() const {
	if(isConnected())
		return settingsControls_->at(1);
	else
		return 0;
}

AMControl* PGTDetector::integrationModeCtrl() const {
	if(isConnected())
		return settingsControls_->at(2);
	else
		return 0;
}



bool PGTDetector::settingsMatchFbk(PGTDetectorInfo *settings){
	bool rVal = false;
	if( fabs(settings->integrationTime() - integrationTimeCtrl()->value()) > integrationTimeCtrl()->tolerance() )
		return rVal;
	else if( fabs( settings->integrationModeList().indexOf(settings->integrationMode()) - integrationModeCtrl()->value()) > integrationModeCtrl()->tolerance() )
		return rVal;
	else if( fabs(settings->hvSetpoint() - hvCtrl()->value()) > hvCtrl()->tolerance())
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
	hvCtrl()->move( pgtSettings->hvSetpoint() );
	integrationTimeCtrl()->move( pgtSettings->integrationTime() );
	integrationModeCtrl()->move( integrationModeCtrl()->enumNames().indexOf(pgtSettings->integrationMode()) );
	return true;
}

void PGTDetector::onControlsConnected(bool connected){
	bool allConnected = readingsControls_->isConnected() && settingsControls_->isConnected();
	if(allConnected != isConnected())
		setConnected(allConnected);
}

void PGTDetector::onSettingsControlValuesChanged(){
	if(isConnected()){
		setHVSetpoint(hvCtrl()->value());
		setIntegrationTime(integrationTimeCtrl()->value());
		setIntegrationMode(integrationModeCtrl()->enumNameAt(integrationModeCtrl()->value()));
		bool lastPoweredOn = poweredOn_;
		poweredOn_ = (hvCtrl()->value() >= 170);
		if(lastPoweredOn != poweredOn_)
			emit poweredOnChanged(poweredOn_);
		//qDebug() << "In PGTDetector " << *toInfo() << " or " << *this;
		emitSettingsChanged();
	}
}

void PGTDetector::onReadingsControlValuesChanged(){
	if(isConnected())
		emitReadingsChanged();
}

bool PGTDetector::initializeFromControlSet(AMControlSet *readingsControls, AMControlSet *settingsControls){
	readingsControls_ = 0; //NULL
	settingsControls_ = 0; //NULL
	poweredOn_ = false;

	if(readingsControls->count() == 1 && settingsControls->count() == 3){
		readingsControls_ = readingsControls;
		settingsControls_ = settingsControls;
		connect(signalSource(), SIGNAL(connected(bool)), this, SLOT(onSettingsControlValuesChanged()));
		connect(readingsControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
		connect(settingsControls_, SIGNAL(connected(bool)), this, SLOT(onControlsConnected(bool)));
		connect(readingsControls_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(onReadingsControlValuesChanged()));
		connect(settingsControls_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(onSettingsControlValuesChanged()));
		onControlsConnected(readingsControls_->isConnected() && settingsControls_->isConnected());
		if(isConnected()){
			onReadingsControlValuesChanged();
			onSettingsControlValuesChanged();
		}
	}
}
