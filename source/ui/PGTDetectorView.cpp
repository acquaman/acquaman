#include "PGTDetectorView.h"

PGTBriefDetectorView::PGTBriefDetectorView(PGTDetector *detector, bool configureOnly, QWidget *parent) :
	AMBriefDetectorView(configureOnly, parent)
{
	hl_ = 0;
	readingCE_ = 0;
	powerState_ = 0;
	detector_ = 0;
	powerOnState_ = QIcon(":/power_green.png");
	powerOffState_ = QIcon(":/power_red.png");
	setDetector(detector, configureOnly_);
}

AMDetector* PGTBriefDetectorView::detector(){
	return detector_;
}

void PGTBriefDetectorView::onPoweredOnChanged(bool poweredOn){
	if(poweredOn)
		powerState_->setIcon(powerOnState_);
	else
		powerState_->setIcon(powerOffState_);
}

bool PGTBriefDetectorView::setDetector(AMDetector *detector, bool configureOnly){
	if(detector_)
		disconnect(detector_, SIGNAL(poweredOnChanged(bool)), this, SLOT(onPoweredOnChanged(bool)));
	if(!detector)
		return false;
	detector_ = static_cast<PGTDetector*>(detector);
	configureOnly_ = configureOnly;
	if(!hl_){
		hl_ = new QHBoxLayout();
		setLayout(hl_);
	}
	if(readingCE_){
		hl_->removeWidget(readingCE_);
		delete readingCE_;
		readingCE_ = 0;
	}
	if(powerState_){
		hl_->removeWidget(powerState_);
		delete powerState_;
		powerState_ = 0;
	}
	readingCE_ = new AMControlEditor(detector_->dataWaveformCtrl(), 0, true);
	readingCE_->setControlFormat('f', 0);
	powerState_ = new QToolButton();
	powerState_->setIcon(powerOffState_);
//	powerState_->setEnabled(false);
	hl_->addWidget(readingCE_);
	hl_->addWidget(powerState_);
	connect(detector_, SIGNAL(poweredOnChanged(bool)), this, SLOT(onPoweredOnChanged(bool)));
	if(detector_->isConnected())
		onPoweredOnChanged(detector_->isPoweredOn());
	return true;
}


PGTDetailedDetectorView::PGTDetailedDetectorView(PGTDetector *detector, bool configureOnly, QWidget *parent) :
	AMDetailedDetectorView(configureOnly, parent)
{
	gl_ = 0;
	readingCE_ = 0;
	hvCE_ = 0;
	integrationModeCE_ = 0;
	integrationTimeCE_ = 0;
	detector_ = 0;
	configurationSettings_ = 0;
	setDetector(detector, configureOnly_);
}

AMDetector* PGTDetailedDetectorView::detector(){
	return detector_;
}

AMDetectorInfo* PGTDetailedDetectorView::configurationSettings() const{
	return configurationSettings_;
}

void PGTDetailedDetectorView::onControlSetpointRequested(){
	if(detector_ && detector_->isConnected()){
		configurationSettings_->setHVSetpoint(hvCE_->setpoint());
		configurationSettings_->setIntegrationMode(detector_->integrationModeCtrl()->enumNameAt(integrationModeCE_->setpoint()));
		configurationSettings_->setIntegrationTime(integrationTimeCE_->setpoint());
		emit settingsConfigureRequested();
	}
}

bool PGTDetailedDetectorView::setDetector(AMDetector *detector, bool configureOnly){
	if(!detector)
		return false;
	detector_ = static_cast<PGTDetector*>(detector);
	if(configurationSettings_)
		configurationSettings_->deleteLater();
	// This NEWs a PGTDetectorInfo, I'm responsible for it now
	configurationSettings_ = qobject_cast<PGTDetectorInfo*>(detector_->toNewInfo());
	configureOnly_ = configureOnly;
	if(!gl_){
		gl_ = new QGridLayout();
		setLayout(gl_);
	}
	if(readingCE_){
		gl_->removeWidget(readingCE_);
		delete readingCE_;
		readingCE_ = 0;
	}
	if(hvCE_){
		gl_->removeWidget(hvCE_);
		delete hvCE_;
		hvCE_ = 0;
	}
	if(integrationModeCE_){
		gl_->removeWidget(integrationModeCE_);
		delete integrationModeCE_;
		integrationModeCE_ = 0;
	}
	if(integrationTimeCE_){
		gl_->removeWidget(integrationTimeCE_);
		delete integrationTimeCE_;
		integrationTimeCE_ = 0;
	}
	readingCE_ = new AMControlEditor(detector_->dataWaveformCtrl());
	readingCE_->setControlFormat('f', 0);
	hvCE_ = new AMControlEditor(detector_->hvCtrl(), 0, false, configureOnly_);
	integrationModeCE_ = new AMControlEditor(detector_->integrationModeCtrl(), 0, false, configureOnly_);
	integrationTimeCE_ = new AMControlEditor(detector_->integrationTimeCtrl(), 0, false, configureOnly_);
	gl_->addWidget(new QLabel("SDD"),	0, 0, 1, 1, 0);
	gl_->addWidget(readingCE_,		0, 1, 1, 2, 0);
	gl_->addWidget(new QLabel("HV"),	1, 0, 1, 1, 0);
	gl_->addWidget(hvCE_,			1, 1, 1, 2, 0);
	gl_->addWidget(new QLabel("Mode"),	2, 0, 1, 1, 0);
	gl_->addWidget(integrationModeCE_,	2, 1, 1, 2, 0);
	gl_->addWidget(new QLabel("Time"),	3, 0, 1, 1, 0);
	gl_->addWidget(integrationTimeCE_,	3, 1, 1, 2, 0);
	connect(hvCE_, SIGNAL(setpointRequested(double)), this, SLOT(onControlSetpointRequested()));
	connect(integrationModeCE_, SIGNAL(setpointRequested(double)), this, SLOT(onControlSetpointRequested()));
	connect(integrationTimeCE_, SIGNAL(setpointRequested(double)), this, SLOT(onControlSetpointRequested()));
	return true;
}
