#include "SGMFluxResolutionPickerView.h"

#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>
#include "ui/beamline/AMExtendedControlEditor.h"
#include "acquaman/AMRegionsList.h"
#include "beamline/SGM/SGMBeamline.h"

SGMFluxResolutionPickerView::SGMFluxResolutionPickerView(AMXASRegionsList *regions, QWidget *parent) :
		QGroupBox("Flux/Resolution", parent)
{
	regions_ = regions;
	minEnergy_ = -1;
	maxEnergy_ = -1;

	bestFluxButton_ = new QPushButton("Best Flux");
	bestResolutionButton_ = new QPushButton("Best Resolution");

	exitSlitGapCE_ = new AMExtendedControlEditor(SGMBeamline::sgm()->exitSlitGap(), 0, false, true);
	gratingCE_ = new AMExtendedControlEditor(SGMBeamline::sgm()->grating(), 0, false, true);
	harmonicCE_ = new AMExtendedControlEditor(SGMBeamline::sgm()->harmonic(), 0, false, true);

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
		if(!SGMBeamlineInfo::sgmInfo()->energyRangeValidForSettings((SGMBeamlineInfo::sgmGrating)gratingCE_->setpoint(), (SGMBeamlineInfo::sgmHarmonic)harmonicCE_->setpoint(), minEnergy_, maxEnergy_))
			warningsLabel_->setText("Grating and/or harmonic are \ninvalid for current energy range");
		else
			warningsLabel_->setText("");
	}
}

void SGMFluxResolutionPickerView::onSetpointsChanged(){
	if(!SGMBeamlineInfo::sgmInfo()->energyRangeValidForSettings((SGMBeamlineInfo::sgmGrating)gratingCE_->setpoint(), (SGMBeamlineInfo::sgmHarmonic)harmonicCE_->setpoint(), minEnergy_, maxEnergy_))
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
	QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic> bestFlux = SGMBeamlineInfo::sgmInfo()->forBestFlux(minEnergy_, maxEnergy_);
	if( SGMBeamlineInfo::sgmInfo()->energyRangeValidForSettings(bestFlux.first, bestFlux.second, minEnergy_, maxEnergy_)){
		exitSlitGapCE_->setSetpoint(150);
		gratingCE_->setSetpoint(bestFlux.first);
		harmonicCE_->setSetpoint(bestFlux.second);
	}
}

void SGMFluxResolutionPickerView::onBestResolutionButtonClicked(){
	QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic> bestResolution = SGMBeamlineInfo::sgmInfo()->forBestResolution(minEnergy_, maxEnergy_);
	if( SGMBeamlineInfo::sgmInfo()->energyRangeValidForSettings(bestResolution.first, bestResolution.second, minEnergy_, maxEnergy_)){
		exitSlitGapCE_->setSetpoint(10);
		gratingCE_->setSetpoint(bestResolution.first);
		harmonicCE_->setSetpoint(bestResolution.second);
	}
}

