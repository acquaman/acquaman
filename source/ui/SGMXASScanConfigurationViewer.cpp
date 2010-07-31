#include "SGMXASScanConfigurationViewer.h"

SGMXASScanConfigurationViewer::SGMXASScanConfigurationViewer(QWidget *parent)  : QWidget(parent){
	setupUi(this);
	cfg_ = NULL;
	if(SGMBeamline::sgm()->isConnected()){
		SGMXASScanConfiguration *sxsc = new SGMXASScanConfiguration(this);

	/*
	QList<chPair> newChannels(sxsc->defaultChannels());
	newChannels.append(chPair("dave", "dave"));
	sxsc->setDefaultChannels(newChannels);

	pScan_()->addChannel("bigTEYNorm", "10000*tey/I0");
	pScan_()->addChannel("bigTFYNorm", "-10000*tfy/I0");

	pScan_()->addChannel("bigTeyNorm", "10000*tey/I0");
	pScan_()->addChannel("pgtTest1", "pgt.pgtCounts[130]");
	pScan_()->addChannel("pgtTest2", "pgt[130].pgtCounts");


	QString indexer = "";
	for(int x = 0; x < 1024; x++){
		indexer.setNum(x);
		pScan_()->addChannel("PGT_COUNTS"+indexer, "pgt.pgtCounts["+indexer+"]");
	}
	*/

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
/*		fluxResolutionView_ = new AMControlOptimizationSetView((AMControlOptimizationSet*)(sxsc->fluxResolutionSet()), this);
		fluxResolutionView_->onRegionsUpdate(sxsc->regions());
		connect(sxsc, SIGNAL(regionsChanged()), this, SLOT(onRegionsChanged()));
//d		connect( ((QSpinBox*)(fluxResolutionView_->boxByName("grating"))), SIGNAL(valueChanged(int)), sxsc, SLOT(setGrating(int)) );
//d		((QSpinBox*)(fluxResolutionView_->boxByName("harmonic")))->setSingleStep(2);
//d		connect( ((QSpinBox*)(fluxResolutionView_->boxByName("harmonic"))), SIGNAL(valueChanged(int)), sxsc, SLOT(setHarmonic(int)) );
//d		connect( ((QDoubleSpinBox*)(fluxResolutionView_->boxByName("exitSlitGap"))), SIGNAL(valueChanged(double)), sxsc, SLOT(setExitSlitGap(double)) );
*/
		fluxResolutionView2_ = new AMCompactControlOptimizationSetView((AMControlOptimizationSet*)(sxsc->fluxResolutionSet()), this);
		fluxResolutionView2_->onRegionsUpdate(sxsc->regions());

//		fluxResolutionView2_ = new AMColorControlOptimizationSetView((AMControlOptimizationSet*)(sxsc->fluxResolutionSet()), this);

		trackingView_ = new AMControlSetView(sxsc->trackingSet(), this);
		connect( ((QSpinBox*)(trackingView_->boxByName("undulatorTracking"))), SIGNAL(valueChanged(int)), sxsc, SLOT(setUndulatorTracking(int)) );
		connect( ((QSpinBox*)(trackingView_->boxByName("monoTracking"))), SIGNAL(valueChanged(int)), sxsc, SLOT(setMonoTracking(int)) );
		connect( ((QSpinBox*)(trackingView_->boxByName("exitSlitTracking"))), SIGNAL(valueChanged(int)), sxsc, SLOT(setExitSlitTracking(int)) );
		detectorView_ = new AMAbstractDetectorSetView(sxsc->detectorSet(), this);
		connect( ((QCheckBox*)(detectorView_->boxByName("tey"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingTEY(int)) );
		connect( ((QCheckBox*)(detectorView_->boxByName("tfy"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingTFY(int)) );
		connect( ((QCheckBox*)(detectorView_->boxByName("pgt"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingPGT(int)) );
		startScanButton_ = new QPushButton();
		startScanButton_->setText("Start Scan");
		startScanButton_->setMaximumWidth(200);
		connect(startScanButton_, SIGNAL(clicked()), this, SLOT(onStartScanClicked()));
		delete doLayoutButton;
		delete layout();
		/*
		vl_.setSpacing(0);
		vl_.addWidget(regionsLineView_);
		vl_.addWidget(regionsView_);
		vl_.addSpacing(40);
		vl_.addWidget(fluxResolutionView_);
		vl_.addWidget(fluxResolutionView2_);
		vl_.addWidget(trackingView_);
		vl_.addWidget(detectorView_);
		vl_.addWidget(startScanButton_);
		qDebug() << "Spacing " << vl_.spacing();
		this->setLayout(&vl_);
		*/
		gl_.setSpacing(0);
		gl_.addWidget(regionsLineView_, 0, 0, 1, 5, Qt::AlignCenter);
		gl_.addWidget(regionsView_, 1, 0, 2, 3, Qt::AlignLeft);
		//gl_.addSpacing(40);
		gl_.addWidget(fluxResolutionView_, 3, 0, 2, 3, Qt::AlignLeft);
		gl_.addWidget(fluxResolutionView2_, 5, 0, 2, 3, Qt::AlignLeft);
		gl_.addWidget(trackingView_, 3, 3, 2, 2, Qt::AlignRight);
		gl_.addWidget(detectorView_, 5, 3, 2, 2, Qt::AlignRight);
		gl_.addWidget(startScanButton_, 7, 0, 1, 2, Qt::AlignLeft);
		qDebug() << "Spacing " << gl_.spacing();
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

