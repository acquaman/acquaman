/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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

#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>

SGMXASScanConfigurationView::SGMXASScanConfigurationView(SGMXASScanConfiguration *sxsc, QWidget *parent)  :
		AMScanConfigurationView(parent)
{
	cfg_ = NULL;
	if(SGMBeamline::sgm()->isConnected()){
		cfg_ = sxsc;

		topFrame_ = new AMTopFrame("Configure an XAS Scan to Run Later");
		topFrame_->setIcon(QIcon(":/utilities-system-monitor.png"));

		//This should be done somewhere else
		//sxsc->xasRegions()->setEnergyControl(SGMBeamline::sgm()->energy());

		regionsLineView_ = new AMRegionsLineView(sxsc->regions(), this);
		regionsView_ = new AMRegionsView(sxsc->regions(), this);

		fluxResolutionView_ = new SGMFluxResolutionPickerView(sxsc->xasRegions(), this);
		fluxResolutionView_->setFromInfoList(sxsc->fluxResolutionGroup());
		fluxResolutionView_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
		connect(fluxResolutionView_, SIGNAL(configValuesChanged(AMControlInfoList)), sxsc, SLOT(setFluxResolutionGroup(AMControlInfoList)));
		connect(sxsc, SIGNAL(fluxResolutionGroupChanged(AMControlInfoList)), fluxResolutionView_, SLOT(setFromInfoList(AMControlInfoList)));

		trackingView_ = new AMControlSetView(sxsc->trackingSet(), true, this);
		trackingView_->setFromInfoList(sxsc->trackingGroup());
		connect(trackingView_, SIGNAL(configValuesChanged(AMControlInfoList)), sxsc, SLOT(setTrackingGroup(AMControlInfoList)));
		connect(sxsc, SIGNAL(trackingGroupChanged(AMControlInfoList)), trackingView_, SLOT(setFromInfoList(AMControlInfoList)));

		xasDetectorsView_ = new AMDetectorSetView(sxsc->detectorChoices(), sxsc->detectorChoiceConfigurations());
		connect(xasDetectorsView_, SIGNAL(configValuesChanged()), this, SLOT(onDetectorConfigurationsChanged()));

		warningsLabel_ = new QLabel("");
		QFont warningsFont;
		warningsFont.setPointSize(48);
		warningsLabel_->setFont(warningsFont);
		warningsLabel_->setStyleSheet( "QLabel{ color: red }" );

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
		bottomGL_->addWidget(trackingView_,		0, 2);
		bottomGL_->addWidget(xasDetectorsView_,	1, 2);
		bottomGL_->setColumnStretch(0, 10);
		bottomGL_->setColumnMinimumWidth(1, 40);
		bottomGL_->setContentsMargins(10, 0, 0, 0);
		mainVL_->addStretch(8);
		QHBoxLayout *nameHL = new QHBoxLayout();
		nameHL->addWidget(scanNameLabel_);
		nameHL->addWidget(scanNameEdit_);
		nameHL->setContentsMargins(10,0,0,0);
		mainVL_->addLayout(nameHL);
		mainVL_->setContentsMargins(0,0,0,0);
		mainVL_->setSpacing(1);
		setLayout(mainVL_);

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

void SGMXASScanConfigurationView::setDisabled(bool disabled){
	regionsView_->setDisabled(disabled);
	fluxResolutionView_->setDisabled(disabled);
	trackingView_->setDisabled(disabled);
	xasDetectorsView_->setDisabled(disabled);
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

void SGMXASScanConfigurationView::onScanNameEditChanged(const QString &scanName){
	cfg_->setUserScanName(scanName);
}

SGMFluxResolutionPickerView::SGMFluxResolutionPickerView(AMXASRegionsList *regions, QWidget *parent) :
		QGroupBox("Flux/Resolution", parent)
{
	regions_ = regions;
	minEnergy_ = -1;
	maxEnergy_ = -1;

	bestFluxButton_ = new QPushButton("Best Flux");
	bestResolutionButton_ = new QPushButton("Best Resolution");

	exitSlitGapCE_ = new AMControlEditor(SGMBeamline::sgm()->exitSlitGap(), 0, false, true);
	gratingCE_ = new AMControlEditor(SGMBeamline::sgm()->grating(), 0, false, true);
	harmonicCE_ = new AMControlEditor(SGMBeamline::sgm()->harmonic(), 0, false, true);

	warningsLabel_ = new QLabel("");
	QFont warningsFont;
	warningsFont.setPointSize(18);
	warningsLabel_->setFont(warningsFont);
	warningsLabel_->setStyleSheet( "QLabel{ color: red }" );

	buttonsVL_ = new QVBoxLayout();
	buttonsVL_->addWidget(bestFluxButton_);
	buttonsVL_->addWidget(bestResolutionButton_);
	buttonsVL_->addStretch(8);
	buttonsVL_->insertSpacing(0, 4);

	ceVL_ = new QVBoxLayout();
	ceVL_->addWidget(exitSlitGapCE_);
	ceVL_->addWidget(gratingCE_);
	ceVL_->addWidget(harmonicCE_);
	ceVL_->addStretch(10);

	settingsHL_ = new QHBoxLayout();
	settingsHL_->addLayout(buttonsVL_);
	settingsHL_->addSpacing(5);
	settingsHL_->addLayout(ceVL_, 10);

	mainVL_ = new QVBoxLayout();
	mainVL_->addLayout(settingsHL_);
	mainVL_->addWidget(warningsLabel_);
	setLayout(mainVL_);

	connect(regions_, SIGNAL(regionsChanged()), this, SLOT(onRegionsChanged()));
	connect(exitSlitGapCE_, SIGNAL(setpointRequested(double)), this, SLOT(onSetpointsChanged()));
	connect(gratingCE_, SIGNAL(setpointRequested(double)), this, SLOT(onSetpointsChanged()));
	connect(harmonicCE_, SIGNAL(setpointRequested(double)), this, SLOT(onSetpointsChanged()));
	connect(bestFluxButton_, SIGNAL(clicked()), this, SLOT(onBestFluxButtonClicked()));
	connect(bestResolutionButton_, SIGNAL(clicked()), this, SLOT(onBestResolutionButtonClicked()));
	onRegionsChanged();
}

void SGMFluxResolutionPickerView::setFromInfoList(const AMControlInfoList &infoList){
	for(int x = 0; x < infoList.count(); x++){
		if( (infoList.at(x).name() == SGMBeamline::sgm()->exitSlitGap()->name()) && (infoList.at(x).value() != exitSlitGapCE_->setpoint()) )
			exitSlitGapCE_->setSetpoint(infoList.at(x).value());
		if( (infoList.at(x).name() == SGMBeamline::sgm()->grating()->name()) && (infoList.at(x).value() != gratingCE_->setpoint()) )
			gratingCE_->setSetpoint(infoList.at(x).value());
		if( (infoList.at(x).name() == SGMBeamline::sgm()->harmonic()->name()) && (infoList.at(x).value() != harmonicCE_->setpoint()) )
			harmonicCE_->setSetpoint(infoList.at(x).value());
	}
}

void SGMFluxResolutionPickerView::setDisabled(bool disabled){
	bestFluxButton_->setDisabled(disabled);
	bestResolutionButton_->setDisabled(disabled);
	exitSlitGapCE_->setReadOnly(disabled);
	gratingCE_->setReadOnly(disabled);
	harmonicCE_->setReadOnly(disabled);
}

void SGMFluxResolutionPickerView::onRegionsChanged(){
	if( (minEnergy_ != regions_->start(0)) || (maxEnergy_ != regions_->end(regions_->count()-1)) ){
		minEnergy_ = regions_->start(0);
		maxEnergy_ = regions_->end(regions_->count()-1);
		if(!SGMBeamline::sgm()->energyRangeValidForSettings((SGMBeamline::sgmGrating)gratingCE_->setpoint(), (SGMBeamline::sgmHarmonic)harmonicCE_->setpoint(), minEnergy_, maxEnergy_))
			warningsLabel_->setText("Grating and/or harmonic are \ninvalid for current energy range");
		else
			warningsLabel_->setText("");
	}
}

void SGMFluxResolutionPickerView::onSetpointsChanged(){
	if(!SGMBeamline::sgm()->energyRangeValidForSettings((SGMBeamline::sgmGrating)gratingCE_->setpoint(), (SGMBeamline::sgmHarmonic)harmonicCE_->setpoint(), minEnergy_, maxEnergy_))
		warningsLabel_->setText("Grating and/or harmonic are \ninvalid for current energy range");
	else
		warningsLabel_->setText("");
	AMControlInfoList setpoints;
	AMControlInfoList originals = SGMBeamline::sgm()->fluxResolutionSet()->toInfoList();
	QList<double> newSettings;
	newSettings << gratingCE_->setpoint() << harmonicCE_->setpoint() << exitSlitGapCE_->setpoint();
	for(int x = 0; x < originals.count(); x++){
		AMControlInfo tmpCI = originals.at(x);
		tmpCI.setValue(newSettings.at(x));
		setpoints.append(tmpCI);
	}
	emit configValuesChanged(setpoints);
}

void SGMFluxResolutionPickerView::onBestFluxButtonClicked(){
	QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic> bestFlux = SGMBeamline::sgm()->forBestFlux(minEnergy_, maxEnergy_);
	if( SGMBeamline::sgm()->energyRangeValidForSettings(bestFlux.first, bestFlux.second, minEnergy_, maxEnergy_)){
		exitSlitGapCE_->setSetpoint(150);
		gratingCE_->setSetpoint(bestFlux.first);
		harmonicCE_->setSetpoint(bestFlux.second);
	}
}

void SGMFluxResolutionPickerView::onBestResolutionButtonClicked(){
	QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic> bestResolution = SGMBeamline::sgm()->forBestResolution(minEnergy_, maxEnergy_);
	if( SGMBeamline::sgm()->energyRangeValidForSettings(bestResolution.first, bestResolution.second, minEnergy_, maxEnergy_)){
		exitSlitGapCE_->setSetpoint(10);
		gratingCE_->setSetpoint(bestResolution.first);
		harmonicCE_->setSetpoint(bestResolution.second);
	}
}
