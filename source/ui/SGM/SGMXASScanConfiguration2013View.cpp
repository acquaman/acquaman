#include "SGMXASScanConfiguration2013View.h"

SGMXASScanConfiguration2013View::SGMXASScanConfiguration2013View(SGMXASScanConfiguration2013 *configuration, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	configuration_ = configuration;

	topFrame_ = new AMTopFrame("Configure an XAS Scan to Run Later");
	topFrame_->setIcon(QIcon(":/utilities-system-monitor.png"));

	regionsLineView_ = new AMRegionsLineView(configuration_->regions(), this);
	regionsView_ = new AMRegionsView(configuration_->regions(), this);

	xasDetectorSelector_ = 0; //NULL
	xasDetectorSelectorView_ = 0; //NULL

	createNewScanActionButton_ = new QPushButton("Create New Scan Controller");

	mainVL_ = new QVBoxLayout();
	mainVL_->addWidget(topFrame_);
	mainVL_->addWidget(regionsLineView_);
	topHL_ = new QHBoxLayout();
	topHL_->addWidget(regionsView_);
	topHL_->addStretch(10);
	topHL_->setContentsMargins(0, 0, 0, 0);
	bottomHL_ = new QHBoxLayout();
	bottomHL_->addStretch(10);
	bottomHL_->setContentsMargins(0, 0, 0, 0);
	mainVL_->addLayout(topHL_);
	mainVL_->addLayout(bottomHL_);
	mainVL_->addWidget(createNewScanActionButton_);
	/*
	bottomGL_ = new QGridLayout();
	mainVL_->addLayout(bottomGL_, 10);
	bottomGL_->addWidget(regionsView_,		0, 0);
	//bottomGL_->addWidget(fluxResolutionView_,	1, 0);
	//bottomGL_->addWidget(trackingView_,		0, 2);
	//bottomGL_->addWidget(xasDetectorSelectorView_,	1, 2);
	bottomGL_->setColumnStretch(0, 10);
	bottomGL_->setColumnMinimumWidth(1, 40);
	bottomGL_->setContentsMargins(10, 0, 0, 0);
	mainVL_->addStretch(8);
	*/
	/*
	QHBoxLayout *nameHL = new QHBoxLayout();
	nameHL->addWidget(scanNameLabel_);
	nameHL->addWidget(scanNameEdit_);
	nameHL->setContentsMargins(10,0,0,0);
	mainVL_->addLayout(nameHL);
	*/
	mainVL_->setContentsMargins(0,0,0,0);
	mainVL_->setSpacing(1);
	setLayout(mainVL_);

	connect(createNewScanActionButton_, SIGNAL(clicked()), this, SLOT(onCreateNewScanActionButtonClicked()));
}

const AMScanConfiguration* SGMXASScanConfiguration2013View::configuration() const{
	//cfg_->setDetectorConfigurations(xasDetectorsView_->configValues());
	return configuration_;
}

void SGMXASScanConfiguration2013View::setDetectorSelector(AMDetectorSelector *xasDetectorSelector){
	xasDetectorSelector_ = xasDetectorSelector;
	if(!xasDetectorSelectorView_){
		xasDetectorSelectorView_ = new AMDetectorSelectorView(xasDetectorSelector_);
		//bottomGL_->addWidget(xasDetectorSelectorView_,	1, 2);
		bottomHL_->addWidget(xasDetectorSelectorView_);
	}
}

void SGMXASScanConfiguration2013View::onCreateNewScanActionButtonClicked(){
	configuration_->setDetectorConfigurations(xasDetectorSelector_->selectedDetectorInfos());

	AMScanController *scanController = configuration_->createController();
	emit scanControllerCreated(scanController);
}
