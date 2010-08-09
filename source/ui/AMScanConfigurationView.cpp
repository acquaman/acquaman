#include "AMScanConfigurationView.h"

AMXASScanConfigurationHolder::AMXASScanConfigurationHolder(QWidget *parent) :
	QWidget(parent)
{
	cfg_ = NULL;
	cfgDetectorInfoSet_ = NULL;
	sxscViewer = NULL;
	sxscWizard = NULL;
}

AMXASScanConfigurationHolder::~AMXASScanConfigurationHolder(){
}

void AMXASScanConfigurationHolder::onFreeToScan(bool ready){
	if(ready){
		SGMXASDacqScanController *xasCtrl = new SGMXASDacqScanController((SGMXASScanConfiguration*)cfg_, SGMBeamline::sgm());
		xasCtrl->initialize();
		xasCtrl->start();
	}
}

void AMXASScanConfigurationHolder::createScanConfiguration(){
	cfg_ = new SGMXASScanConfiguration(this);
	cfg_->setFileName("daveData.%03d.dat");
	cfg_->setFilePath(AMUserSettings::userDataFolder);
	/*
	cfg_->addRegion(0, 700, 5, 950);
	cfg_->addRegion(1, 955, 2, 1000);
	cfg_->addRegion(2, 1000, 4, 1100);

	cfg_->addRegion(1, 850, 1, 970);
	*/
	cfg_->addRegion(0, 500, 5, 600);

	cfgDetectorInfoSet_ = new AMDetectorInfoSet(this);
	cfg_->setCfgDetectorInfoSet(cfgDetectorInfoSet_);
	AMDetectorInfo* tmpDI, *tdi;
	for(int x = 0; x < cfg_->detectorSet()->count(); x++){
		tdi = cfg_->detectorSet()->detectorAt(x);
		if(tdi->typeDescription() == "PGT SDD Spectrum-Output Detector")
			tmpDI = new PGTDetectorInfo(tdi->name(), tdi->description(), this);
		else if(tdi->typeDescription() == "MCP Detector")
			tmpDI = new MCPDetectorInfo(tdi->name(), tdi->description(), this);
		else
			tmpDI = new AMDetectorInfo(tdi->name(), tdi->description(), this);

		QList<AMMetaMetaData> all = tmpDI->metaDataAllKeys();
		for(int y = 0; y < all.count(); y++)
			tmpDI->setMetaData(all.at(y).key, tdi->metaData(all.at(y).key));
		cfgDetectorInfoSet_->addDetector(tmpDI, cfg_->detectorSet()->isDefaultAt(x));
	}
}

void AMXASScanConfigurationHolder::onSidebarLinkChanged(){
	if(!sxscViewer && isVisible() && SGMBeamline::sgm()->isConnected()){
		createScanConfiguration();
		sxscViewer = new SGMXASScanConfigurationViewer(cfg_, cfgDetectorInfoSet_);
		connect(sxscViewer, SIGNAL(startScanRequested()), this, SLOT(onStartScanRequested()));
		connect(sxscViewer, SIGNAL(addToQueueRequested()), this, SLOT(onAddToQueueRequested()));
		vl_ = new QVBoxLayout();
		vl_->addWidget(sxscViewer);
		this->setLayout(vl_);
	}
//	if(!sxscWizard && isVisible() && SGMBeamline::sgm()->isConnected()){
//		sxscWizard = new SGMXASScanConfigurationWizard(cfg_, cfgDetectorInfoSet_);
//		connect(sxscWizard, SIGNAL(scanStartRequested()), this, SLOT(onStartScanRequested()));
//		sxscWizard->show();
//	}
}

void AMXASScanConfigurationHolder::onStartScanRequested(){
	emit startScanRequested();
}

void AMXASScanConfigurationHolder::onAddToQueueRequested(){
	emit addToQueueRequested(cfg_);
}
