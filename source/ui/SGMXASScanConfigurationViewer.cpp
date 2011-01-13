#include "SGMXASScanConfigurationViewer.h"

SGMXASScanConfigurationViewer::SGMXASScanConfigurationViewer(SGMXASScanConfiguration *sxsc, AMDetectorInfoSet *cfgDetectorInfoSet, QWidget *parent)  : QWidget(parent){
	setupUi(this);
	cfg_ = NULL;
	if(SGMBeamline::sgm()->isConnected()){
		cfg_ = sxsc;
		cfgDetectorInfoSet_ = cfgDetectorInfoSet;

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
		connect( ((QComboBox*)(trackingView_->boxByName("undulatorTracking"))), SIGNAL(currentIndexChanged(int)), sxsc, SLOT(setUndulatorTracking(int)) );
		connect( ((QComboBox*)(trackingView_->boxByName("monoTracking"))), SIGNAL(currentIndexChanged(int)), sxsc, SLOT(setMonoTracking(int)) );
		connect( ((QComboBox*)(trackingView_->boxByName("exitSlitTracking"))), SIGNAL(currentIndexChanged(int)), sxsc, SLOT(setExitSlitTracking(int)) );

		detectorView_ = new AMDetectorSetView(sxsc->detectorSet(), cfgDetectorInfoSet_, true, this);
		if( SGMBeamline::sgm()->detectorConnectedByName("tey") )
			connect( ((QCheckBox*)(detectorView_->boxByName("tey"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingTEY(int)) );
		if( SGMBeamline::sgm()->detectorConnectedByName("tfy") )
			connect( ((QCheckBox*)(detectorView_->boxByName("tfy"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingTFY(int)) );
		if( SGMBeamline::sgm()->detectorConnectedByName("sdd") )
			connect( ((QCheckBox*)(detectorView_->boxByName("pgt"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingPGT(int)) );

		startScanButton_ = new QPushButton();
		startScanButton_->setText("Start Scan");
		addToQueueButton_ = new QPushButton();
		addToQueueButton_->setText("Add Scan to Queue");
		queueDirectorButton_ = new QPushButton();
		queueDirectorButton_->setText("Queue Director");
		int buttonWidth = max(startScanButton_->sizeHint().width(), addToQueueButton_->sizeHint().width());
		buttonWidth = max(buttonWidth, queueDirectorButton_->sizeHint().width());
		int buttonHeight = max(startScanButton_->sizeHint().height(), addToQueueButton_->sizeHint().height());
		buttonHeight = max(buttonHeight, queueDirectorButton_->sizeHint().height());
		qDebug() << "height: " << buttonHeight;
		startScanButton_->setMinimumSize(buttonWidth, (int)(1.75*buttonHeight));
		addToQueueButton_->setMinimumSize(buttonWidth, buttonHeight);
		queueDirectorButton_->setMinimumSize(buttonWidth, buttonHeight);
		connect(startScanButton_, SIGNAL(clicked()), this, SLOT(onStartScanClicked()));
		connect(addToQueueButton_, SIGNAL(clicked()), this, SLOT(onAddToQueueRequested()));
		connect(queueDirectorButton_, SIGNAL(clicked()), this, SIGNAL(queueDirectorRequested()));

		delete doLayoutButton;
		delete layout();
		QSpacerItem *spc1 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Maximum);
		QSpacerItem *spc2 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Maximum);
//		QSpacerItem *spc3 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Maximum);
		gl_.setSpacing(0);
		gl_.addWidget(regionsLineView_,		0, 0, 1, 5, Qt::AlignCenter);
		gl_.addWidget(regionsView_,		1, 0, 2, 3, Qt::AlignLeft);
		gl_.addWidget(fluxResolutionView_,	3, 0, 2, 3, Qt::AlignLeft);
		gl_.addWidget(trackingView_,		1, 3, 2, 2, Qt::AlignLeft);
		gl_.addWidget(detectorView_,		3, 3, 2, 2, Qt::AlignLeft);
		gl_.addWidget(startScanButton_,		5, 3, 1, 2, Qt::AlignRight);
		gl_.addWidget(addToQueueButton_,	6, 3, 1, 2, Qt::AlignRight);
		gl_.addWidget(queueDirectorButton_,	7, 3, 1, 2, Qt::AlignRight);
		gl_.addItem(spc1,			8, 0, 2, 3, Qt::AlignLeft);
		gl_.addItem(spc2,			8, 3, 2, 2, Qt::AlignLeft);
//		gl_.addItem(spc3,			9, 3, 1, 2, Qt::AlignLeft);
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

void SGMXASScanConfigurationViewer::onLockdowScanning(bool isLocked, QString reason){
	if(isLocked){
		startScanButton_->setEnabled(false);
		startScanButton_->setText("Start Scan\n"+reason);
	}
	else{
		startScanButton_->setEnabled(true);
		startScanButton_->setText("Start Scan");
	}
}

