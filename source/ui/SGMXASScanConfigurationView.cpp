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

SGMXASScanConfigurationView::SGMXASScanConfigurationView(SGMXASScanConfiguration *sxsc, QWidget *parent)  :
		AMScanConfigurationView(parent)
{
	setupUi(this);
	cfg_ = NULL;
	if(SGMBeamline::sgm()->isConnected()){
		cfg_ = sxsc;

		regionsLineView_ = new AMRegionsLineView(sxsc->regions(), this);

		regionsView_ = new AMXASRegionsView(sxsc->regions(), this);
		regionsView_->setBeamlineEnergy(SGMBeamline::sgm()->energy());
		connect(sxsc, SIGNAL(regionsChanged()), this, SLOT(onRegionsChanged()));

		fluxResolutionView_ = new AMCompactControlOptimizationSetView((AMControlOptimizationSet*)(sxsc->fluxResolutionSet()), this);
		connect( ((QComboBox*)(fluxResolutionView_->detailView()->boxByName("grating"))), SIGNAL(currentIndexChanged(int)), sxsc, SLOT(setGrating(int)) );
		connect( ((QComboBox*)(fluxResolutionView_->detailView()->boxByName("harmonic"))), SIGNAL(currentIndexChanged(int)), sxsc, SLOT(setHarmonic(int)) );
		connect( ((QDoubleSpinBox*)(fluxResolutionView_->detailView()->boxByName("exitSlitGap"))), SIGNAL(valueChanged(double)), sxsc, SLOT(setExitSlitGap(double)) );
		fluxResolutionView_->onRegionsUpdate(sxsc->regions());

		qDebug() << "Flux 250 - 270: " << SGMBeamline::sgm()->forBestFlux(250, 270);
		qDebug() << "Flux 380 - 410: " << SGMBeamline::sgm()->forBestFlux(380, 410);
		qDebug() << "Flux 395 - 425: " << SGMBeamline::sgm()->forBestFlux(395, 425);
		qDebug() << "Flux 410 - 485: " << SGMBeamline::sgm()->forBestFlux(410, 485);
		qDebug() << "Flux 600 - 635: " << SGMBeamline::sgm()->forBestFlux(600, 635);
		qDebug() << "Flux 620 - 660: " << SGMBeamline::sgm()->forBestFlux(620, 660);
		qDebug() << "Flux 770 - 790: " << SGMBeamline::sgm()->forBestFlux(770, 790);
		qDebug() << "Flux 780 - 810: " << SGMBeamline::sgm()->forBestFlux(780, 810);
		qDebug() << "Flux 780 - 850: " << SGMBeamline::sgm()->forBestFlux(780, 850);
		qDebug() << "Flux 840 - 1000: " << SGMBeamline::sgm()->forBestFlux(840, 1000);
		qDebug() << "Flux 1090 - 1105: " << SGMBeamline::sgm()->forBestFlux(1090, 1105);
		qDebug() << "Flux 1080 - 1150: " << SGMBeamline::sgm()->forBestFlux(1080, 1150);
		qDebug() << "Flux 1110 - 1170: " << SGMBeamline::sgm()->forBestFlux(1110, 1170);
		qDebug() << "Flux 1170 - 1205: " << SGMBeamline::sgm()->forBestFlux(1170, 1205);
		qDebug() << "Flux 1180 - 1320: " << SGMBeamline::sgm()->forBestFlux(1180, 1320);
		qDebug() << "Flux 1400 - 1600: " << SGMBeamline::sgm()->forBestFlux(1400, 1600);

		qDebug() << "Resolution 250 - 270: " << SGMBeamline::sgm()->forBestResolution(250, 270);
		qDebug() << "Resolution 380 - 410: " << SGMBeamline::sgm()->forBestResolution(380, 410);
		qDebug() << "Resolution 395 - 425: " << SGMBeamline::sgm()->forBestResolution(395, 425);
		qDebug() << "Resolution 410 - 485: " << SGMBeamline::sgm()->forBestResolution(410, 485);
		qDebug() << "Resolution 600 - 635: " << SGMBeamline::sgm()->forBestResolution(600, 635);
		qDebug() << "Resolution 620 - 660: " << SGMBeamline::sgm()->forBestResolution(620, 660);
		qDebug() << "Resolution 770 - 790: " << SGMBeamline::sgm()->forBestResolution(770, 790);
		qDebug() << "Resolution 780 - 810: " << SGMBeamline::sgm()->forBestResolution(780, 810);
		qDebug() << "Resolution 780 - 850: " << SGMBeamline::sgm()->forBestResolution(780, 850);
		qDebug() << "Resolution 840 - 1000: " << SGMBeamline::sgm()->forBestResolution(840, 1000);
		qDebug() << "Resolution 1090 - 1105: " << SGMBeamline::sgm()->forBestResolution(1090, 1105);
		qDebug() << "Resolution 1080 - 1150: " << SGMBeamline::sgm()->forBestResolution(1080, 1150);
		qDebug() << "Resolution 1110 - 1170: " << SGMBeamline::sgm()->forBestResolution(1110, 1170);
		qDebug() << "Resolution 1170 - 1205: " << SGMBeamline::sgm()->forBestResolution(1170, 1205);
		qDebug() << "Resolution 1180 - 1320: " << SGMBeamline::sgm()->forBestResolution(1180, 1320);
		qDebug() << "Resolution 1400 - 1600: " << SGMBeamline::sgm()->forBestResolution(1400, 1600);

		trackingView_ = new AMControlSetView(sxsc->trackingSet(), true, this);
		connect(trackingView_, SIGNAL(configValuesChanged(AMControlInfoList)), sxsc, SLOT(setTrackingGroup(AMControlInfoList)));

		xasDetectorsView_ = new AMDetectorSetView(sxsc->detectorChoices(), true);
		connect(xasDetectorsView_, SIGNAL(configValuesChanged()), this, SLOT(onDetectorConfigurationsChanged()));

		warningsLabel_ = new QLabel("");
		QFont warningsFont;
		warningsFont.setPointSize(48);
		warningsLabel_->setFont(warningsFont);
		warningsLabel_->setStyleSheet( "QLabel{ color: red }" );

		delete doLayoutButton;
		delete layout();

		QSpacerItem *spc1 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Maximum);
		QSpacerItem *spc2 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Maximum);
		gl_.setSpacing(0);
		gl_.addWidget(regionsLineView_,		0, 0, 1, 5, Qt::AlignCenter);
		gl_.addWidget(regionsView_,		1, 0, 2, 3, Qt::AlignLeft);
		gl_.addWidget(fluxResolutionView_,	3, 0, 2, 3, Qt::AlignLeft);
		gl_.addWidget(trackingView_,		1, 3, 2, 2, Qt::AlignLeft);
		gl_.addWidget(xasDetectorsView_,	3, 3, 2, 2, Qt::AlignLeft);
		gl_.addWidget(warningsLabel_,		2, 0, 1, 5, Qt::AlignCenter);
		gl_.addItem(spc1,			8, 0, 2, 3, Qt::AlignLeft);
		gl_.addItem(spc2,			8, 3, 2, 2, Qt::AlignLeft);
		this->setLayout(&gl_);
		this->setMaximumSize(800, 800);

		connect(SGMBeamline::sgm(), SIGNAL(criticalControlsConnectionsChanged()), this, SLOT(onSGMBeamlineCriticalControlsConnectedChanged()));
		onSGMBeamlineCriticalControlsConnectedChanged();
	}
}

SGMXASScanConfigurationView::~SGMXASScanConfigurationView(){
}

const AMScanConfiguration* SGMXASScanConfigurationView::configuration() const{
	cfg_->setDetectorConfigurations(xasDetectorsView_->configValues());
	return cfg_;
}

void SGMXASScanConfigurationView::onDetectorConfigurationsChanged(){
	cfg_->setDetectorConfigurations(xasDetectorsView_->configValues());
}

void SGMXASScanConfigurationView::onSGMBeamlineCriticalControlsConnectedChanged(){
	if(SGMBeamline::sgm()->isConnected()){
		regionsView_->setEnabled(true);
		regionsLineView_->setEnabled(true);
		fluxResolutionView_->setEnabled(true);
		trackingView_->setEnabled(true);
		xasDetectorsView_->setEnabled(true);
		warningsLabel_->setText("");
	}
	else{
		regionsView_->setEnabled(false);
		regionsLineView_->setEnabled(false);
		fluxResolutionView_->setEnabled(false);
		trackingView_->setEnabled(false);
		xasDetectorsView_->setEnabled(false);
		warningsLabel_->setText("SGM Beamline Unavailable");
	}
}
