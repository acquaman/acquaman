#include "SGMXASScanConfigurationViewer.h"

SGMXASScanConfigurationViewer::SGMXASScanConfigurationViewer(QWidget *parent)  : QWidget(parent){
	setupUi(this);
	cfg_ = NULL;
	if(SGMBeamline::sgm()->isConnected()){
		SGMXASScanConfiguration *sxsc = new SGMXASScanConfiguration(this);

		cfg_ = sxsc;
		sxsc->setFileName("daveData.%03d.dat");
		sxsc->setFilePath(AMUserSettings::userDataFolder);
		/*
		sxsc->addRegion(0, 700, 5, 950);
		sxsc->addRegion(1, 955, 2, 1000);
		sxsc->addRegion(2, 1000, 4, 1100);

		sxsc->addRegion(1, 850, 1, 970);
		*/
		sxsc->addRegion(0, 500, 5, 600);

		regionsLineView_ = new AMRegionsLineView(sxsc->regions(), this);

		regionsView_ = new AMXASRegionsView(sxsc->regions(), this);
		regionsView_->setBeamlineEnergy(SGMBeamline::sgm()->energy());
		connect(regionsView_, SIGNAL(addRegionClicked()), this, SLOT(onAddRegionClicked()));
		connect(sxsc, SIGNAL(regionsChanged()), this, SLOT(onRegionsChanged()));

		fluxResolutionView_ = new AMCompactControlOptimizationSetView((AMControlOptimizationSet*)(sxsc->fluxResolutionSet()), this);
		connect( ((QComboBox*)(fluxResolutionView_->detailView()->boxByName("grating"))), SIGNAL(currentIndexChanged(int)), sxsc, SLOT(setGrating(int)) );
		connect( ((QComboBox*)(fluxResolutionView_->detailView()->boxByName("harmonic"))), SIGNAL(currentIndexChanged(int)), sxsc, SLOT(setHarmonic(int)) );
		connect( ((QDoubleSpinBox*)(fluxResolutionView_->detailView()->boxByName("exitSlitGap"))), SIGNAL(valueChanged(double)), sxsc, SLOT(setExitSlitGap(double)) );
		fluxResolutionView_->onRegionsUpdate(sxsc->regions());

		trackingView_ = new AMControlSetView(sxsc->trackingSet(), this);
		connect( ((QSpinBox*)(trackingView_->boxByName("undulatorTracking"))), SIGNAL(valueChanged(int)), sxsc, SLOT(setUndulatorTracking(int)) );
		connect( ((QSpinBox*)(trackingView_->boxByName("monoTracking"))), SIGNAL(valueChanged(int)), sxsc, SLOT(setMonoTracking(int)) );
		connect( ((QSpinBox*)(trackingView_->boxByName("exitSlitTracking"))), SIGNAL(valueChanged(int)), sxsc, SLOT(setExitSlitTracking(int)) );
		//detectorView_ = new AMDetectorInfoSetView(sxsc->detectorSet(), this);
		detectorView_ = new AMDetectorSetView(sxsc->detectorSet(), this);
		connect( ((QCheckBox*)(detectorView_->boxByName("tey"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingTEY(int)) );
		connect( ((QCheckBox*)(detectorView_->boxByName("tfy"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingTFY(int)) );
		connect( ((QCheckBox*)(detectorView_->boxByName("pgt"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingPGT(int)) );
		startScanButton_ = new QPushButton();
		startScanButton_->setText("Start Scan");
		startScanButton_->setMaximumWidth(200);
		connect(startScanButton_, SIGNAL(clicked()), this, SLOT(onStartScanClicked()));
		delete doLayoutButton;
		delete layout();
		QSpacerItem *spc1 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Maximum);
		QSpacerItem *spc2 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Maximum);
		QSpacerItem *spc3 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Maximum);
		gl_.setSpacing(0);
		gl_.addWidget(regionsLineView_,		0, 0, 1, 5, Qt::AlignCenter);
		gl_.addWidget(regionsView_,			1, 0, 2, 3, Qt::AlignLeft);
		gl_.addWidget(fluxResolutionView_, 3, 0, 2, 3, Qt::AlignLeft);
		gl_.addWidget(trackingView_,		1, 3, 2, 2, Qt::AlignLeft);
		gl_.addWidget(detectorView_,		3, 3, 2, 2, Qt::AlignLeft);
		gl_.addItem(spc1,					5, 0, 2, 3, Qt::AlignLeft);
		gl_.addItem(spc2,					5, 3, 2, 2, Qt::AlignLeft);
		gl_.addWidget(startScanButton_,		9, 0, 1, 2, Qt::AlignLeft);
		gl_.addItem(spc3,					9, 3, 1, 2, Qt::AlignLeft);
		this->setLayout(&gl_);
		this->setMaximumSize(800, 800);
	}
}

SGMXASScanConfigurationViewer::~SGMXASScanConfigurationViewer(){
}

void SGMXASScanConfigurationViewer::onAddRegionClicked(){
/*
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
*/
}

