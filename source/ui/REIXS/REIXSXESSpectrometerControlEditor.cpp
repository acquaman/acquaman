/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "REIXSXESSpectrometerControlEditor.h"
#include "ui_REIXSXESSpectrometerControlEditor.h"

#include "util/AMErrorMonitor.h"
#include <QStringBuilder>

#include "util/AMFontSizes.h"

REIXSXESSpectrometerControlEditor::REIXSXESSpectrometerControlEditor(REIXSSpectrometer* spectrometer, QWidget *parent) :
	QGroupBox(parent),
	ui_(new Ui::REIXSXESSpectrometerControlEditor)
{
	ui_->setupUi(this);
	QFont font = ui_->gratingFeedbackLabel->font();
	font.setPointSize(AM_FONT_SMALL);
	ui_->gratingFeedbackLabel->setFont(font);
	ui_->energyFeedbackLabel->setFont(font);

	spectrometer_ = spectrometer;

	populateGratingComboBox();

	connect(spectrometer_, SIGNAL(valueChanged(double)), this, SLOT(updateCurrentEnergyStatus(double)));
	connect(spectrometer_, SIGNAL(calibrationChanged()), this, SLOT(populateGratingComboBox()));
	connect(spectrometer_, SIGNAL(gratingChanged(int)), this, SLOT(updateCurrentGratingStatus()));
	connect(spectrometer_, SIGNAL(movingChanged(bool)), this, SLOT(updateCurrentGratingStatus()));
	connect(spectrometer_->gratingMask(), SIGNAL(valueChanged(double)), this, SLOT(updateMaskPosition()));

	connect(ui_->energyBox, SIGNAL(valueChanged(double)), this, SLOT(updateCurrentEnergyStatus()));
	connect(ui_->gratingSelectorBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onGratingComboBoxActivated(int)));


	connect(ui_->moveNowButton, SIGNAL(clicked()), this, SLOT(onMoveButtonClicked()));

	connect(spectrometer_, SIGNAL(moveSucceeded()), this, SLOT(onSpectrometerMoveSucceeded()));
	connect(spectrometer_, SIGNAL(moveFailed(int)), this, SLOT(onSpectrometerMoveFailed(int)));

	connect(ui_->stopButton, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));

}

REIXSXESSpectrometerControlEditor::~REIXSXESSpectrometerControlEditor()
{
	delete ui_;
}


void REIXSXESSpectrometerControlEditor::onMoveButtonClicked()
{
	spectrometer_->specifyGrating(ui_->gratingSelectorBox->currentIndex());
	spectrometer_->specifyDetectorTiltOffset(ui_->tiltOffsetBox->value());
	spectrometer_->specifyFocusOffset(ui_->defocusOffsetBox->value());

	int failureExplanation = spectrometer_->move(ui_->energyBox->value());
	if(failureExplanation != AMControl::NoFailure)
		onSpectrometerMoveFailed(failureExplanation);
}

void REIXSXESSpectrometerControlEditor::onGratingComboBoxActivated(int grating)
{
	Q_UNUSED(grating)
	updateCurrentGratingStatus();
}

void REIXSXESSpectrometerControlEditor::populateGratingComboBox()
{
	ui_->gratingSelectorBox->blockSignals(true);

	ui_->gratingSelectorBox->clear();

	for(int i=0, cc=spectrometer_->spectrometerCalibration()->gratingCount(); i<cc; i++)
		ui_->gratingSelectorBox->addItem(spectrometer_->spectrometerCalibration()->gratingAt(i).name());

	ui_->gratingSelectorBox->blockSignals(false);
	updateCurrentGratingStatus();
}

void REIXSXESSpectrometerControlEditor::updateCurrentEnergyStatus(double eV)
{
	if(eV < 0) {
		ui_->energyFeedbackLabel->setText("Currently: unknown");
	}

	else if(fabs(eV - ui_->energyBox->value()) < 0.001) {
		ui_->energyFeedbackLabel->setText(QString());	// we're exactly what the user asked for; all is well, don't need to show anything.
	}
	else {
		ui_->energyFeedbackLabel->setText(QString("Currently: %1").arg(eV));
	}
}

void REIXSXESSpectrometerControlEditor::updateMaskPosition()
{
	ui_->maskFeedbackLabel->setText(QString("Currently: %1").arg(spectrometer_->gratingMask()->value()));
}

void REIXSXESSpectrometerControlEditor::updateCurrentEnergyStatus() {
	updateCurrentEnergyStatus(spectrometer_->value());
}

void REIXSXESSpectrometerControlEditor::updateCurrentGratingStatus()
{
	if(spectrometer_->grating() == -1) {
		ui_->gratingFeedbackLabel->setText("Currently: unknown");
	}
	else if(spectrometer_->gratingInPosition() == false) {
		ui_->gratingFeedbackLabel->setText(
					QString("Currently: %1 (out of position)")
					.arg(spectrometer_->spectrometerCalibration()->gratingAt(spectrometer_->grating()).name()));
	}
	else if(spectrometer_->grating() != ui_->gratingSelectorBox->currentIndex()) {
		ui_->gratingFeedbackLabel->setText(("Currently: " % spectrometer_->spectrometerCalibration()->gratingAt(spectrometer_->grating()).name()));
	}
	else {
		// our drop-down selection matches the current grating, and it's in position. All is well -- don't need to tell the user anything.
		ui_->gratingFeedbackLabel->setText(QString());
	}
}

void REIXSXESSpectrometerControlEditor::onSpectrometerMoveSucceeded()
{
//	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Information, 0, QString("Spectrometer move to %1 finished.").arg(spectrometer_->value())));
}

void REIXSXESSpectrometerControlEditor::onSpectrometerMoveFailed(int reason)
{
	Q_UNUSED(reason)
//	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, reason, "Spectrometer Move Failed"));
}

void REIXSXESSpectrometerControlEditor::onStopButtonClicked()
{
	spectrometer_->stop();
}



void REIXSXESSpectrometerControlEditor::on_maskComboBox_currentIndexChanged(const QString &arg1)
{
	if (arg1 == "Pin Hole")
		spectrometer_->gratingMask()->move(8.5);
	else if (arg1 == "LEG")
		spectrometer_->gratingMask()->move(12.9);
	else if (arg1 == "Impurity")
		spectrometer_->gratingMask()->move(12.9);
	else if (arg1 == "MEG")
		spectrometer_->gratingMask()->move(12.8);
	else if (arg1 == "HRMEG")
		spectrometer_->gratingMask()->move(12.6);
	else if (arg1 == "OUT")
		spectrometer_->gratingMask()->move(1.0);
}

