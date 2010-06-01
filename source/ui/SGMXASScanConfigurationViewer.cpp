#include "SGMXASScanConfigurationViewer.h"

SGMXASScanConfigurationViewer::SGMXASScanConfigurationViewer(QWidget *parent)  : QWidget(parent){
	setupUi(this);
	cfg_ = NULL;
	connect(doLayoutButton, SIGNAL(clicked()), this, SLOT(onDoLayout()));
}

SGMXASScanConfigurationViewer::~SGMXASScanConfigurationViewer(){
}

void SGMXASScanConfigurationViewer::onAddRegionClicked(){
	if(!cfg_)
		return;
	SGMXASScanConfiguration *sxsc = (SGMXASScanConfiguration*)cfg_;
	for(int x = 0; x < sxsc->count(); x++)
		qDebug() << "Region " << x << ": " << sxsc->start(x) << " " << sxsc->delta(x) << " " << sxsc->end(x);
	if(regionsView_->addRegion(sxsc->count(), 500, 1, 510))
		qDebug() << "Passed add valid";
	else
		qDebug() << "Failed add valid?";
	if(regionsView_->addRegion(sxsc->count(), 100, 1, 110))
		qDebug() << "Failed add invalid?";
	else
		qDebug() << "Passed add invalid";
	for(int x = 0; x < sxsc->count(); x++)
		qDebug() << "Region " << x << ": " << sxsc->start(x) << " " << sxsc->delta(x) << " " << sxsc->end(x);
}

void SGMXASScanConfigurationViewer::setScanConfiguration(AMScanConfiguration *cfg){
	cfg_ = cfg;
	SGMXASScanConfiguration *sxsc = (SGMXASScanConfiguration*)cfg_;
	sxsc->setFileName("daveData.%03d.dat");
	sxsc->setFilePath(AMUserSettings::userDataFolder);
/**/
	sxsc->addRegion(0, 280, 0.5, 290);
/**//*
	sxsc->addRegion(0, 270, 1, 280);
	sxsc->addRegion(1, 280.25, 0.25, 290);
	sxsc->addRegion(2, 290, 1, 320);
*//*
	sxsc->addRegion(0, 600, 1, 650);
	sxsc->addRegion(1, 650.5, 0.5, 690);
	sxsc->addRegion(2, 692, 2, 720);
*/
	regionsView_ = new AMXASRegionsView(sxsc->regions(), this);
	regionsView_->setBeamlineEnergy(SGMBeamline::sgm()->energy());
	connect(regionsView_, SIGNAL(addRegionClicked()), this, SLOT(onAddRegionClicked()));
	fluxResolutionView_ = new AMControlOptimizationSetView((AMControlOptimizationSet*)(sxsc->fluxResolutionSet()), this);
	fluxResolutionView_->onRegionsUpdate(sxsc->regions());
	connect( ((QSpinBox*)(fluxResolutionView_->boxByName("grating"))), SIGNAL(valueChanged(int)), sxsc, SLOT(setGrating(int)) );
	((QSpinBox*)(fluxResolutionView_->boxByName("harmonic")))->setSingleStep(2);
	connect( ((QSpinBox*)(fluxResolutionView_->boxByName("harmonic"))), SIGNAL(valueChanged(int)), sxsc, SLOT(setHarmonic(int)) );
	connect( ((QDoubleSpinBox*)(fluxResolutionView_->boxByName("exitSlitGap"))), SIGNAL(valueChanged(double)), sxsc, SLOT(setExitSlitGap(double)) );

	trackingView_ = new AMControlSetView(sxsc->trackingSet(), this);
	connect( ((QSpinBox*)(trackingView_->boxByName("undulatorTracking"))), SIGNAL(valueChanged(int)), sxsc, SLOT(setUndulatorTracking(int)) );
	connect( ((QSpinBox*)(trackingView_->boxByName("monoTracking"))), SIGNAL(valueChanged(int)), sxsc, SLOT(setMonoTracking(int)) );
	connect( ((QSpinBox*)(trackingView_->boxByName("exitSlitTracking"))), SIGNAL(valueChanged(int)), sxsc, SLOT(setExitSlitTracking(int)) );
	detectorView_ = new AMDetectorSetView(sxsc->detectorSet(), this);
	connect( ((QCheckBox*)(detectorView_->boxByName("tey"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingTEY(int)) );
	connect( ((QCheckBox*)(detectorView_->boxByName("tfy"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingTFY(int)) );
	connect( ((QCheckBox*)(detectorView_->boxByName("pgt"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingPGT(int)) );
	startScanButton_ = new QPushButton();
	startScanButton_->setText("Start Scan");
	startScanButton_->setMaximumWidth(200);
	connect(startScanButton_, SIGNAL(clicked()), this, SLOT(onStartScanClicked()));
	disconnect(doLayoutButton, SIGNAL(clicked()), this, SLOT(onDoLayout()));
	delete doLayoutButton;
	delete layout();
	vl_.addWidget(regionsView_);
	vl_.addWidget(fluxResolutionView_);
	vl_.addWidget(trackingView_);
	vl_.addWidget(detectorView_);
	vl_.addWidget(startScanButton_);
	this->setLayout(&vl_);
}

