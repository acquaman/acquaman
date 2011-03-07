/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "SGMXASScanConfigurationView.h"

SGMXASScanConfigurationView::SGMXASScanConfigurationView(SGMXASScanConfiguration *sxsc, AMOldDetectorInfoSet *cfgDetectorInfoSet, QWidget *parent)  :
		//QWidget(parent){
		AMScanConfigurationView(parent){
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

		//trackingView_ = new AMOldControlSetView(sxsc->trackingSet(), this);
		trackingView_ = new AMControlSetView(sxsc->trackingSet(), true, this);
		connect(trackingView_, SIGNAL(configValuesChanged(AMControlInfoList)), sxsc, SLOT(setTrackingGroup(AMControlInfoList)));
		/*
		connect( ((QComboBox*)(trackingView_->boxByName("undulatorTracking"))), SIGNAL(currentIndexChanged(int)), sxsc, SLOT(setUndulatorTracking(int)) );
		connect( ((QComboBox*)(trackingView_->boxByName("monoTracking"))), SIGNAL(currentIndexChanged(int)), sxsc, SLOT(setMonoTracking(int)) );
		connect( ((QComboBox*)(trackingView_->boxByName("exitSlitTracking"))), SIGNAL(currentIndexChanged(int)), sxsc, SLOT(setExitSlitTracking(int)) );
		*/

		detectorView_ = new AMOldDetectorSetView(sxsc->detectorSet(), cfgDetectorInfoSet_, true, this);
		if( SGMBeamline::sgm()->detectorConnectedByName("tey") )
			connect( ((QCheckBox*)(detectorView_->boxByName("tey"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingTEY(int)) );
		if( SGMBeamline::sgm()->detectorConnectedByName("tfy") )
			connect( ((QCheckBox*)(detectorView_->boxByName("tfy"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingTFY(int)) );
		if( SGMBeamline::sgm()->detectorConnectedByName("sdd") )
			connect( ((QCheckBox*)(detectorView_->boxByName("pgt"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingPGT(int)) );

		warningsLabel_ = new QLabel("");
		QFont warningsFont;
		warningsFont.setPointSize(48);
		warningsLabel_->setFont(warningsFont);
		warningsLabel_->setStyleSheet( "QLabel{ color: red }" );
		/*
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
		*/

		delete doLayoutButton;
		delete layout();

		/*
		AMDetectorViewSupport::registerClass<AMSingleControlDetectorView, AMSingleControlDetector>();
		AMDetectorViewSupport::registerClass<MCPDetectorView, MCPDetector>();
		AMDetectorViewSupport::registerClass<PGTDetectorView, PGTDetector>();
		*/

		/*
		QWidget *dv = qobject_cast<AMDetectorView*>( (AMDetectorViewSupport::registeredClasses()->value(AMDetectorView::staticMetaObject.className())).viewMetaObject->newInstance() );
		QWidget *dv1 = qobject_cast<AMDetectorView*>( (AMDetectorViewSupport::registeredClasses()->value(AMDetector1View::staticMetaObject.className())).viewMetaObject->newInstance() );
		QWidget *dv2 = qobject_cast<AMDetectorView*>( (AMDetectorViewSupport::registeredClasses()->value(AMDetector2View::staticMetaObject.className())).viewMetaObject->newInstance() );
		*/

		/*
		QWidget *dv = qobject_cast<AMDetectorView*>( (AMDetectorViewSupport::supportedClasses(SGMBeamline::sgm()->teyDetector())).first().viewMetaObject->newInstance() );
		QWidget *dv1 = qobject_cast<AMDetectorView*>( (AMDetectorViewSupport::supportedClasses(SGMBeamline::sgm()->tfyDetector())).first().viewMetaObject->newInstance() );
		QWidget *dv2 = qobject_cast<AMDetectorView*>( (AMDetectorViewSupport::supportedClasses(SGMBeamline::sgm()->pgtDetector())).first().viewMetaObject->newInstance() );
		*/

		QWidget *dv = AMDetectorViewSupport::createDetectorView(SGMBeamline::sgm()->teyDetector());
		//QWidget *dv = AMDetectorViewSupport::createDetectorView(SGMBeamline::sgm()->i0Detector());
		QWidget *dv1 = AMDetectorViewSupport::createDetectorView(SGMBeamline::sgm()->tfyDetector());
		QWidget *dv2 = AMDetectorViewSupport::createDetectorView(SGMBeamline::sgm()->pgtDetector());
		//QWidget *dv2 = AMDetectorViewSupport::createDetectorView(SGMBeamline::sgm()->i0Detector());

		QSpacerItem *spc1 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Maximum);
		QSpacerItem *spc2 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Maximum);
//		QSpacerItem *spc3 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Maximum);
		gl_.setSpacing(0);
		gl_.addWidget(regionsLineView_,		0, 0, 1, 5, Qt::AlignCenter);
		gl_.addWidget(regionsView_,		1, 0, 2, 3, Qt::AlignLeft);
		gl_.addWidget(fluxResolutionView_,	3, 0, 2, 3, Qt::AlignLeft);
		gl_.addWidget(trackingView_,		1, 3, 2, 2, Qt::AlignLeft);
		gl_.addWidget(detectorView_,		3, 3, 2, 2, Qt::AlignLeft);
		/**/
		gl_.addWidget(dv,			5, 0, 1, 1, Qt::AlignLeft);
		gl_.addWidget(dv1,			5, 1, 1, 1, Qt::AlignLeft);
		gl_.addWidget(dv2,			5, 2, 1, 1, Qt::AlignLeft);
		/**/
		gl_.addWidget(warningsLabel_,		2, 0, 1, 5, Qt::AlignCenter);
		gl_.addItem(spc1,			8, 0, 2, 3, Qt::AlignLeft);
		gl_.addItem(spc2,			8, 3, 2, 2, Qt::AlignLeft);
//		gl_.addItem(spc3,			9, 3, 1, 2, Qt::AlignLeft);
		this->setLayout(&gl_);
		this->setMaximumSize(800, 800);

		connect(SGMBeamline::sgm(), SIGNAL(criticalControlsConnectionsChanged()), this, SLOT(onSGMBeamlineCriticalControlsConnectedChanged()));
		onSGMBeamlineCriticalControlsConnectedChanged();
	}
}

SGMXASScanConfigurationView::~SGMXASScanConfigurationView(){
}

const AMScanConfiguration* SGMXASScanConfigurationView::configuration() const{
	return cfg_;
}

void SGMXASScanConfigurationView::onAddRegionClicked(){
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

void SGMXASScanConfigurationView::onSGMBeamlineCriticalControlsConnectedChanged(){
	if(SGMBeamline::sgm()->isConnected()){
		regionsView_->setEnabled(true);
		regionsLineView_->setEnabled(true);
		fluxResolutionView_->setEnabled(true);
		trackingView_->setEnabled(true);
		detectorView_->setEnabled(true);
		warningsLabel_->setText("");
	}
	else{
		regionsView_->setEnabled(false);
		regionsLineView_->setEnabled(false);
		fluxResolutionView_->setEnabled(false);
		trackingView_->setEnabled(false);
		detectorView_->setEnabled(false);
		warningsLabel_->setText("SGM Beamline Unavailable");
	}
}

/*
void SGMXASScanConfigurationView::onLockdowScanning(bool isLocked, QString reason){
	if(isLocked){
		startScanButton_->setEnabled(false);
		startScanButton_->setText("Start Scan\n"+reason);
	}
	else{
		startScanButton_->setEnabled(true);
		startScanButton_->setText("Start Scan");
	}
}
*/
