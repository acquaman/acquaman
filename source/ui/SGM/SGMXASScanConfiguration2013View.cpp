#include "SGMXASScanConfiguration2013View.h"

 SGMXASScanConfiguration2013View::~SGMXASScanConfiguration2013View(){}
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

	trackingSet_ = 0; //NULL
	trackingSetView_ = 0; //NULL

	fluxResolutionView_ = new SGMFluxResolutionPickerView(configuration_->xasRegions(), this);
	fluxResolutionView_->setFromInfoList(configuration_->fluxResolutionGroup());
	fluxResolutionView_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
	connect(fluxResolutionView_, SIGNAL(configValuesChanged(AMControlInfoList)), configuration_->dbObject(), SLOT(setFluxResolutionGroup(AMControlInfoList)));
	connect(configuration_->dbObject(), SIGNAL(fluxResolutionGroupChanged()), this, SLOT(onFluxResolutionGroupChanged()));

	scanNameLabel_ = new QLabel("Scan Name");
	scanNameEdit_ = new QLineEdit(this);
	connect(scanNameEdit_, SIGNAL(textEdited(QString)), this, SLOT(onScanNameEditChanged(QString)));

	mainVL_ = new QVBoxLayout();
	mainVL_->addWidget(topFrame_);
	mainVL_->addWidget(regionsLineView_);
	bottomGL_ = new QGridLayout();
	mainVL_->addLayout(bottomGL_, 10);
	bottomGL_->addWidget(regionsView_,		0, 0);
	bottomGL_->addWidget(fluxResolutionView_,	1, 0);
	bottomGL_->setColumnStretch(0, 10);
	bottomGL_->setColumnMinimumWidth(1, 40);
	bottomGL_->setContentsMargins(10, 0, 0, 10);
	mainVL_->addStretch(8);

	QHBoxLayout *nameHL = new QHBoxLayout();
	nameHL->addWidget(scanNameLabel_);
	nameHL->addWidget(scanNameEdit_);
	nameHL->setContentsMargins(10,0,0,0);
	mainVL_->addLayout(nameHL);

	mainVL_->setContentsMargins(0,0,0,0);
	mainVL_->setSpacing(1);
	setLayout(mainVL_);
}

const AMScanConfiguration* SGMXASScanConfiguration2013View::configuration() const{
	configuration_->setDetectorConfigurations(xasDetectorSelector_->selectedDetectorInfos());
	return configuration_;
}

void SGMXASScanConfiguration2013View::setDetectorSelector(AMDetectorSelector *xasDetectorSelector){
	xasDetectorSelector_ = xasDetectorSelector;
	if(!xasDetectorSelectorView_){
		xasDetectorSelectorView_ = new AMDetectorSelectorView(xasDetectorSelector_);
		bottomGL_->addWidget(xasDetectorSelectorView_,	1, 2);
	}
}

void SGMXASScanConfiguration2013View::setTrackingSet(AMControlSet *trackingSet){
	trackingSet_ = trackingSet;
	if(!trackingSetView_){
		trackingSetView_ = new AMControlSetView(trackingSet_, true);
		bottomGL_->addWidget(trackingSetView_, 0, 2);

		connect(trackingSetView_, SIGNAL(configValuesChanged(AMControlInfoList)), configuration_->dbObject(), SLOT(setTrackingGroup(AMControlInfoList)));
		connect(configuration_->dbObject(), SIGNAL(trackingGroupChanged()), this, SLOT(onTrackingGroupChanged()));
	}
}

void SGMXASScanConfiguration2013View::onTrackingGroupChanged(){
	trackingSetView_->setFromInfoList(configuration_->trackingGroup());
}

void SGMXASScanConfiguration2013View::onFluxResolutionGroupChanged(){
	fluxResolutionView_->setFromInfoList(configuration_->fluxResolutionGroup());
}

void SGMXASScanConfiguration2013View::onScanNameEditChanged(const QString &scanName){
	configuration_->setUserScanName(scanName);


	for(int x = 0, size = trackingSet_->count(); x < size; x++)
		qDebug() << "Tracking set at " << x << trackingSet_->at(x)->name() << trackingSet_->controlNamed(trackingSet_->at(x)->name())->value();

	for(int x = 0, size = configuration_->trackingGroup().count(); x < size; x++)
		qDebug() << "Tracking group at " << x << configuration_->trackingGroup().at(x).name() << configuration_->trackingGroup().controlNamed(configuration_->trackingGroup().at(x).name()).value();
}
