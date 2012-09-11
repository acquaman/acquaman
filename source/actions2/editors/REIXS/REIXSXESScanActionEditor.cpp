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


#include "REIXSXESScanActionEditor.h"

#include <QDoubleSpinBox>
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QComboBox>

#include "acquaman/REIXS/REIXSXESScanConfiguration.h"
#include "ui/REIXS/REIXSXESScanConfigurationView.h"

REIXSXESScanActionEditor::REIXSXESScanActionEditor(REIXSXESScanActionInfo *info, QWidget *parent)
	: QFrame(parent) {

	info_ = info;
	setFrameStyle(QFrame::StyledPanel);

	energy_ = new QDoubleSpinBox();
	energy_->setDecimals(3);
	energy_->setRange(50, 1400);	// todo: restrict to current grating?

//	focusOffset_ = new QDoubleSpinBox();
//	focusOffset_->setDecimals(2);
//	focusOffset_->setRange(-1000, 1000);

	tiltOffset_ = new QDoubleSpinBox();
	tiltOffset_->setDecimals(2);
	tiltOffset_->setRange(-6, 6);
	tiltOffset_->setSingleStep(0.1);

	grating_ = new QComboBox();

	QPushButton* moreDetailsButton = new QPushButton("More...");

	QHBoxLayout* hl = new QHBoxLayout(this);
	hl->addWidget(new QLabel("Detector eV"));
	hl->addWidget(energy_);
	hl->addWidget(grating_);
	hl->addWidget(new QLabel("Tilt offset (deg)"));
	hl->addWidget(tiltOffset_);
//	hl->addWidget(new QLabel("Defocus (mm)"));
//	hl->addWidget(focusOffset_);
	hl->addStretch();
	hl->addWidget(moreDetailsButton);


	// Initialize from config:
	// populate the grating drop down.
	REIXSXESCalibration2 calibration;
	if(info_->xesConfig()->spectrometerCalibrationId() > 0)
		calibration.loadFromDb(AMDatabase::database("user"), info_->xesConfig()->spectrometerCalibrationId());
	for(int i=0; i<calibration.gratingCount(); ++i)
		grating_->addItem(calibration.gratingAt(i).name());

	grating_->setCurrentIndex(info_->xesConfig()->gratingNumber());

	energy_->setValue(info_->xesConfig()->centerEV());
	tiltOffset_->setValue(info_->xesConfig()->detectorTiltOffset());
//	focusOffset_->setValue(info_->xesConfig()->defocusDistanceMm());

	connect(energy_, SIGNAL(editingFinished()), this, SLOT(onEnergyEditingFinished()));
//	connect(focusOffset_, SIGNAL(editingFinished()), this, SLOT(onFocusOffsetEditingFinished()));
	connect(tiltOffset_, SIGNAL(editingFinished()), this, SLOT(onTiltOffsetEditingFinished()));
	connect(grating_, SIGNAL(activated(int)), this, SLOT(onGratingChoiceActivated(int)));
	connect(moreDetailsButton, SIGNAL(clicked()), this, SLOT(onMoreDetailsButtonClicked()));

	connect(info_->xesConfig(), SIGNAL(configurationChanged()), this, SLOT(onScanConfigurationChanged()));

}

void REIXSXESScanActionEditor::onEnergyEditingFinished()
{
	// TODO: Down the road, would it make sense for scan configurations to be AMActionInfo objects themselves... so we don't need this wrapper layer?
	info_->xesConfig()->setCenterEV(energy_->value());
}

//void REIXSXESScanActionEditor::onFocusOffsetEditingFinished()
//{
//	info_->xesConfig()->setDefocusDistanceMm(focusOffset_->value());
//}

void REIXSXESScanActionEditor::onTiltOffsetEditingFinished()
{
	info_->xesConfig()->setDetectorTiltOffset(tiltOffset_->value());
}

void REIXSXESScanActionEditor::onMoreDetailsButtonClicked()
{
	REIXSXESScanConfigurationView* view = new REIXSXESScanConfigurationView(info_->xesConfig());
	QDialog* dialog = new QDialog(this, Qt::Tool);
	QVBoxLayout* vl = new QVBoxLayout(dialog);
	vl->addWidget(view);

	dialog->show();	// dialog is a child of this widget, so will be destroyed when this widget is destroyed.
}

void REIXSXESScanActionEditor::onScanConfigurationChanged()
{
	double energy = info_->xesConfig()->centerEV();
	double tilt = info_->xesConfig()->detectorTiltOffset();
//	double defocus = info_->xesConfig()->defocusDistanceMm();
	int gratingNumber = info_->xesConfig()->gratingNumber();

	if(energy_->value() != energy) {
		energy_->blockSignals(true);
		energy_->setValue(energy);
		energy_->blockSignals(false);
	}
	if(tiltOffset_->value() != tilt) {
		tiltOffset_->blockSignals(true);
		tiltOffset_->setValue(tilt);
		tiltOffset_->blockSignals(false);
	}
//	if(focusOffset_->value() != defocus) {
//		focusOffset_->blockSignals(true);
//		focusOffset_->setValue(defocus);
//		focusOffset_->blockSignals(false);
//	}
	if(grating_->currentIndex() != gratingNumber) {
		grating_->blockSignals(true);
		grating_->setCurrentIndex(gratingNumber);
		grating_->blockSignals(false);
	}
}

void REIXSXESScanActionEditor::onGratingChoiceActivated(int index)
{
	info_->xesConfig()->setGratingNumber(index);
}
