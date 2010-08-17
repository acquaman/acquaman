#include "AMScanConfigurationView.h"

AMXASScanConfigurationHolder::AMXASScanConfigurationHolder(QWidget *parent) :
	QWidget(parent)
{
	cfg_ = NULL;
	cfgDetectorInfoSet_ = NULL;
	sxscViewer = NULL;
	sxscWizard = NULL;
	vl_ = NULL;
	director = new AMScanConfigurationQueueDirector();
	director->setWindowModality(Qt::ApplicationModal);
	connect(director, SIGNAL(goToQueue()), this, SLOT(goToQueue()));
	connect(director, SIGNAL(goToNewScan()), this, SLOT(goToNewScan()));
}

AMXASScanConfigurationHolder::~AMXASScanConfigurationHolder(){
	delete director;
}

void AMXASScanConfigurationHolder::onFreeToScan(bool ready){
	if(ready){
		SGMXASDacqScanController *xasCtrl = new SGMXASDacqScanController((SGMXASScanConfiguration*)cfg_, SGMBeamline::sgm());
		xasCtrl->initialize();
		xasCtrl->start();
	}
}

void AMXASScanConfigurationHolder::onAddedToQueue(AMScanConfiguration *cfg){
	if(cfg == cfg_)
		director->showDirector();
}

void AMXASScanConfigurationHolder::setWorkflowPaneVariant(const QVariant &workflowPaneVariant){
	workflowPaneVariant_ = workflowPaneVariant;
}

void AMXASScanConfigurationHolder::createScanConfiguration(){
	cfg_ = new SGMXASScanConfiguration(this);
	cfg_->setFileName("daveData.%03d.dat");
	cfg_->setFilePath(AMUserSettings::userDataFolder);
	cfg_->addRegion(0, 950, 5, 960);

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

void AMXASScanConfigurationHolder::destroyScanConfigurationViewer(){
	if(sxscViewer){
		disconnect(sxscViewer, SIGNAL(startScanRequested()), this, SLOT(onStartScanRequested()));
		disconnect(sxscViewer, SIGNAL(addToQueueRequested()), this, SLOT(onAddToQueueRequested()));
		disconnect(sxscViewer, SIGNAL(queueDirectorRequested()), director, SLOT(show()));
		vl_->removeWidget(sxscViewer);
		delete sxscViewer;
		sxscViewer = NULL;
	}
}

void AMXASScanConfigurationHolder::onSidebarLinkChanged(){
	if(!sxscViewer && isVisible() && SGMBeamline::sgm()->isConnected()){
		createScanConfiguration();
		sxscViewer = new SGMXASScanConfigurationViewer(cfg_, cfgDetectorInfoSet_);
		connect(sxscViewer, SIGNAL(startScanRequested()), this, SLOT(onStartScanRequested()));
		connect(sxscViewer, SIGNAL(addToQueueRequested()), this, SLOT(onAddToQueueRequested()));
		connect(sxscViewer, SIGNAL(queueDirectorRequested()), director, SLOT(show()));
		if(!vl_)
			vl_ = new QVBoxLayout();
		vl_->addWidget(sxscViewer);
		if(layout() != vl_){
			delete layout();
			this->setLayout(vl_);
		}
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

void AMXASScanConfigurationHolder::goToQueue(){
	destroyScanConfigurationViewer();
	emit goToQueueRequested(workflowPaneVariant_);
}

void AMXASScanConfigurationHolder::goToNewScan(){
	destroyScanConfigurationViewer();
	onSidebarLinkChanged();
}

AMScanConfigurationQueueDirector::AMScanConfigurationQueueDirector(QWidget *parent) :
		QWidget(parent)
{
	alwaysGoToQueue_ = false;
	alwaysGoToNewScan_ = false;

	vl_ = new QVBoxLayout();
	message_ = new QLabel("<h3>What would you like to do now?</h3>");
	vl_->addWidget(message_);
	QSpacerItem *tmpSpc = new QSpacerItem(10, 20, QSizePolicy::Preferred, QSizePolicy::Maximum);
	vl_->addSpacerItem(tmpSpc);
	choices_ = new QFormLayout();
	QHBoxLayout *tmpHB;
	tmpHB = new QHBoxLayout();
	goToQueueButton_ = new QPushButton("Workflow Queue");
	goToQueueCheck_ = new QCheckBox("Always do this");
	tmpHB->addSpacerItem(new QSpacerItem(25, 10, QSizePolicy::Maximum, QSizePolicy::Preferred));
	tmpHB->addWidget(goToQueueButton_);
	tmpHB->addWidget(goToQueueCheck_);
	choices_->addRow("Go to the Workflow Queue", tmpHB);
	tmpHB = new QHBoxLayout();
	goToNewScanButton_ = new QPushButton("New Scan");
	goToNewScanCheck_ = new QCheckBox("Always do this");
	tmpHB->addSpacerItem(new QSpacerItem(25, 10, QSizePolicy::Maximum, QSizePolicy::Preferred));
	tmpHB->addWidget(goToNewScanButton_);
	tmpHB->addWidget(goToNewScanCheck_);
	choices_->addRow("Create a New Scan Configuration", tmpHB);
	vl_->addLayout(choices_);
	connect(goToQueueButton_, SIGNAL(clicked()), this, SIGNAL(goToQueue()));
	connect(goToQueueButton_, SIGNAL(clicked()), this, SLOT(hide()));
	connect(goToNewScanButton_, SIGNAL(clicked()), this, SIGNAL(goToNewScan()));
	connect(goToNewScanButton_, SIGNAL(clicked()), this, SLOT(hide()));
	connect(goToQueueCheck_, SIGNAL(toggled(bool)), this, SLOT(onAlwaysQueueCheck(bool)));
	connect(goToNewScanCheck_, SIGNAL(toggled(bool)), this, SLOT(onAlwaysNewScanCheck(bool)));
	setLayout(vl_);
}

void AMScanConfigurationQueueDirector::showDirector(){
	if(alwaysGoToQueue_)
		emit goToQueue();
	else if(alwaysGoToNewScan_)
		emit goToNewScan();
	else
		show();
}

void AMScanConfigurationQueueDirector::onAlwaysQueueCheck(bool checked){
	alwaysGoToQueue_ = checked;
	if(checked && alwaysGoToNewScan_)
		goToNewScanCheck_->setChecked(false);
	if(alwaysGoToQueue_)
		goToNewScanButton_->setEnabled(false);
	else
		goToNewScanButton_->setEnabled(true);
}

void AMScanConfigurationQueueDirector::onAlwaysNewScanCheck(bool checked){
	alwaysGoToNewScan_ = checked;
	if(checked && alwaysGoToQueue_)
		goToQueueCheck_->setChecked(false);
	if(alwaysGoToNewScan_)
		goToQueueButton_->setEnabled(false);
	else
		goToQueueButton_->setEnabled(true);
}
