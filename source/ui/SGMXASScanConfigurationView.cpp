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
		detectorView_ = new AMOldDetectorSetView(sxsc->detectorSet(), cfgDetectorInfoSet_, true, this);
		if( SGMBeamline::sgm()->detectorConnectedByName("tey") )
			connect( ((QCheckBox*)(detectorView_->boxByName("tey"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingTEY(int)) );
		if( SGMBeamline::sgm()->detectorConnectedByName("tfy") )
			connect( ((QCheckBox*)(detectorView_->boxByName("tfy"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingTFY(int)) );
		if( SGMBeamline::sgm()->detectorConnectedByName("sdd") )
			connect( ((QCheckBox*)(detectorView_->boxByName("pgt"))), SIGNAL(stateChanged(int)), sxsc, SLOT(setUsingPGT(int)) );
		*/
		xasDetectorsView_ = new AMDetectorSetView(SGMBeamline::sgm()->XASDetectorsNew(), true);

		warningsLabel_ = new QLabel("");
		QFont warningsFont;
		warningsFont.setPointSize(48);
		warningsLabel_->setFont(warningsFont);
		warningsLabel_->setStyleSheet( "QLabel{ color: red }" );

		delete doLayoutButton;
		delete layout();

		/*
		QWidget *dv = AMDetectorViewSupport::createBriefDetectorView(SGMBeamline::sgm()->teyDetectorNew());
		QWidget *dv1 = AMDetectorViewSupport::createBriefDetectorView(SGMBeamline::sgm()->tfyDetectorNew());
		QWidget *dv2 = AMDetectorViewSupport::createBriefDetectorView(SGMBeamline::sgm()->pgtDetectorNew());
		*/

		QSpacerItem *spc1 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Maximum);
		QSpacerItem *spc2 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Maximum);
//		QSpacerItem *spc3 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Maximum);
		gl_.setSpacing(0);
		gl_.addWidget(regionsLineView_,		0, 0, 1, 5, Qt::AlignCenter);
		gl_.addWidget(regionsView_,		1, 0, 2, 3, Qt::AlignLeft);
		gl_.addWidget(fluxResolutionView_,	3, 0, 2, 3, Qt::AlignLeft);
		gl_.addWidget(trackingView_,		1, 3, 2, 2, Qt::AlignLeft);
		gl_.addWidget(xasDetectorsView_,	3, 3, 2, 2, Qt::AlignLeft);
		//gl_.addWidget(detectorView_,		3, 3, 2, 2, Qt::AlignLeft);
		/*
		gl_.addWidget(dv,			5, 0, 1, 1, Qt::AlignLeft);
		gl_.addWidget(dv1,			5, 1, 1, 1, Qt::AlignLeft);
		gl_.addWidget(dv2,			5, 2, 1, 1, Qt::AlignLeft);
		*/
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
		//detectorView_->setEnabled(true);
		xasDetectorsView_->setEnabled(true);
		warningsLabel_->setText("");
	}
	else{
		regionsView_->setEnabled(false);
		regionsLineView_->setEnabled(false);
		fluxResolutionView_->setEnabled(false);
		trackingView_->setEnabled(false);
		//detectorView_->setEnabled(false);
		xasDetectorsView_->setEnabled(false);
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
