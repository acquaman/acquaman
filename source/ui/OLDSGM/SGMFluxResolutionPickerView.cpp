/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "SGMFluxResolutionPickerView.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QBoxLayout>
#include "ui/beamline/AMExtendedControlEditor.h"
#include "beamline/SGM/SGMBeamline.h"
#include "dataman/AMScanAxis.h"

SGMFluxResolutionPickerStaticView::SGMFluxResolutionPickerStaticView(AMScanAxis *scanAxis, QWidget *parent)
	: QGroupBox("Flux/Resolution", parent)
{
	scanAxis_ = scanAxis;
	minEnergy_ = -1;
	maxEnergy_ = -1;

	QVBoxLayout *tmpVL;
	exitSlitGapGroupBox_ = new QGroupBox("Exit Slit Gap");
	exitSlitGapLineEdit_ = new QLineEdit("N/A");
	tmpVL = new QVBoxLayout();
	tmpVL->setContentsMargins(2,2,2,2);
	tmpVL->addWidget(exitSlitGapLineEdit_);
	exitSlitGapGroupBox_->setLayout(tmpVL);

	gratingGroupBox_ = new QGroupBox("Grating");
	gratingLineEdit_ = new QLineEdit("N/A");
	tmpVL = new QVBoxLayout();
	tmpVL->setContentsMargins(2,2,2,2);
	tmpVL->addWidget(gratingLineEdit_);
	gratingGroupBox_->setLayout(tmpVL);

	harmonicGroupBox_ = new QGroupBox("Harmonic");
	harmonicLineEdit_ = new QLineEdit("N/A");
	tmpVL = new QVBoxLayout();
	tmpVL->setContentsMargins(2,2,2,2);
	tmpVL->addWidget(harmonicLineEdit_);
	harmonicGroupBox_->setLayout(tmpVL);

	buttonsVL_ = new QVBoxLayout();

	ceVL_ = new QVBoxLayout();
	ceVL_->addWidget(exitSlitGapGroupBox_);
	ceVL_->addWidget(gratingGroupBox_);
	ceVL_->addWidget(harmonicGroupBox_);

	settingsHL_ = new QHBoxLayout();
	settingsHL_->addLayout(buttonsVL_);
	settingsHL_->addSpacing(5);
	settingsHL_->addLayout(ceVL_);

	mainVL_ = new QVBoxLayout();
	mainVL_->addLayout(settingsHL_);
	setLayout(mainVL_);
}

SGMFluxResolutionPickerStaticView::~SGMFluxResolutionPickerStaticView(){}

void SGMFluxResolutionPickerStaticView::setFromInfoList(const AMControlInfoList &infoList){
	for(int x = 0; x < infoList.count(); x++){
		if( infoList.at(x).name() == "exitSlitGap")
			exitSlitGapLineEdit_->setText(QString("%1").arg(infoList.at(x).value()));
		if( infoList.at(x).name() == "grating")
			gratingLineEdit_->setText(QString("%1").arg(SGMBeamlineInfo::sgmInfo()->sgmGratingDescription((SGMBeamlineInfo::sgmGrating)infoList.at(x).value())));
		if( infoList.at(x).name() == "harmonic")
			harmonicLineEdit_->setText(QString("%1").arg(SGMBeamlineInfo::sgmInfo()->sgmHarmonicDescription((SGMBeamlineInfo::sgmHarmonic)infoList.at(x).value())));
	}
}


SGMFluxResolutionPickerView::SGMFluxResolutionPickerView(AMScanAxis *scanAxis, QWidget *parent)
	: SGMFluxResolutionPickerStaticView(scanAxis, parent)
{
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

	buttonsVL_->addWidget(bestFluxButton_);
	buttonsVL_->addWidget(bestResolutionButton_);
	buttonsVL_->addStretch();

	ceVL_->removeWidget(exitSlitGapGroupBox_);
	ceVL_->removeWidget(gratingGroupBox_);
	ceVL_->removeWidget(harmonicGroupBox_);
	exitSlitGapGroupBox_->deleteLater();
	gratingGroupBox_->deleteLater();
	harmonicGroupBox_->deleteLater();

	ceVL_->addWidget(exitSlitGapCE_);
	ceVL_->addWidget(gratingCE_);
	ceVL_->addWidget(harmonicCE_);
	ceVL_->addStretch();

	mainVL_->addWidget(warningsLabel_);

	connect(scanAxis_, SIGNAL(regionsChanged()), this, SLOT(onRegionsChanged()));
	connect(exitSlitGapCE_, SIGNAL(setpointRequested(double)), this, SLOT(onSetpointsChanged()));
	connect(gratingCE_, SIGNAL(setpointRequested(double)), this, SLOT(onSetpointsChanged()));
	connect(harmonicCE_, SIGNAL(setpointRequested(double)), this, SLOT(onSetpointsChanged()));
	connect(bestFluxButton_, SIGNAL(clicked()), this, SLOT(onBestFluxButtonClicked()));
	connect(bestResolutionButton_, SIGNAL(clicked()), this, SLOT(onBestResolutionButtonClicked()));
	onRegionsChanged();
}

SGMFluxResolutionPickerView::~SGMFluxResolutionPickerView(){}

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
	exitSlitGapCE_->setReadOnlyPreference(disabled);
	gratingCE_->setReadOnlyPreference(disabled);
	harmonicCE_->setReadOnlyPreference(disabled);
}

void SGMFluxResolutionPickerView::onRegionsChanged(){
	if( (minEnergy_ != double(scanAxis_->regionAt(0)->regionStart())) || (maxEnergy_ != double(scanAxis_->regionAt(scanAxis_->regionCount()-1)->regionEnd())) ){
		minEnergy_ = double(scanAxis_->regionAt(0)->regionStart());
		maxEnergy_ = double(scanAxis_->regionAt(scanAxis_->regionCount()-1)->regionEnd());
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

