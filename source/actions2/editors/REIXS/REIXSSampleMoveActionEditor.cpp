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


#include "REIXSSampleMoveActionEditor.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QBoxLayout>
#include <QLabel>

#include "beamline/REIXS/REIXSBeamline.h"
#include "dataman/AMSamplePlatePre2013.h"

REIXSSampleMoveActionEditor::REIXSSampleMoveActionEditor(REIXSSampleMoveActionInfo *info, QWidget *parent) : QFrame(parent) {

	info_ = info;

	// Create GUI:
	fromSample_ = new QRadioButton("Sample on Plate");
	fromPositions_ = new QRadioButton("Defined Position");

	QVBoxLayout* vl = new QVBoxLayout();
	vl->addWidget(fromSample_);
	vl->addWidget(fromPositions_);
	vl->setContentsMargins(0,0,0,0);

	QHBoxLayout* hl = new QHBoxLayout();

	sample_ = new QComboBox();

	x_ = new QDoubleSpinBox();
	x_->setRange(-100, 100);
	x_->setDecimals(2);

	y_ = new QDoubleSpinBox();
	y_->setRange(-100, 100);
	y_->setDecimals(2);

	z_ = new QDoubleSpinBox();
	z_->setRange(-1000, 1000);
	z_->setDecimals(2);

	theta_ = new QDoubleSpinBox();
	theta_->setRange(-360, 360);
	theta_->setDecimals(2);

	hl->addLayout(vl);
	hl->addStretch(0);
	hl->addWidget(sample_);
	hl->addStretch(0);
	hl->addWidget(new QLabel("X:"));
	hl->addWidget(x_);
	hl->addWidget(new QLabel("Y:"));
	hl->addWidget(y_);
	hl->addWidget(new QLabel("Z:"));
	hl->addWidget(z_);
	hl->addWidget(new QLabel("Theta:"));
	hl->addWidget(theta_);


	setLayout(hl);

	// Initialize GUI:

	// Fill the sample choices based on the beamline's _current_ sample plate. This is a dubious choice, and couples this widget to the beamline class.
	AMSamplePlatePre2013* samplePlate = REIXSBeamline::bl()->samplePlate();
	for(int i=0, cc=samplePlate->count(); i<cc; ++i) {
		int sampleId = samplePlate->at(i).sampleId();
		AMSamplePre2013 s; s.loadFromDb(AMDatabase::database("user"), sampleId);
		sample_->addItem(s.name(), sampleId);
	}
	if(samplePlate->id() == info_->samplePlateId() && info_->samplePlateId() > 0) {	// If the beamline's current sample plate id matches the samplePlateId() of our action, set the currently-selected sample to match.
		sample_->setCurrentIndex(info_->sampleIndex());
	}

	// Mode 1: move to sample plate / sample:
	if(info_->samplePlateId() > 0) {
		fromSample_->setChecked(true);

		sample_->setEnabled(true);
		x_->setEnabled(false);
		y_->setEnabled(false);
		z_->setEnabled(false);
		theta_->setEnabled(false);
	}
	// Mode 2: move to fixed position:
	else {
		fromPositions_->setChecked(true);
		sample_->setEnabled(false);
		x_->setEnabled(true);
		y_->setEnabled(true);
		z_->setEnabled(true);
		theta_->setEnabled(true);

		x_->setValue(info_->x());
		y_->setValue(info_->y());
		z_->setValue(info_->z());
		theta_->setValue(info_->theta());
	}

	// make connections:
	connect(sample_, SIGNAL(activated(int)), this, SLOT(onSampleActivated(int)));
	connect(fromSample_, SIGNAL(toggled(bool)), this, SLOT(onFromSampleToggled(bool)));

	// connect editing finished for all spin boxes...
	connect(x_, SIGNAL(editingFinished()), this, SLOT(onSpinBoxEditingFinished()));
	connect(y_, SIGNAL(editingFinished()), this, SLOT(onSpinBoxEditingFinished()));
	connect(z_, SIGNAL(editingFinished()), this, SLOT(onSpinBoxEditingFinished()));
	connect(theta_, SIGNAL(editingFinished()), this, SLOT(onSpinBoxEditingFinished()));
}

void REIXSSampleMoveActionEditor::onFromSampleToggled(bool useSample)
{
	if(useSample) {
		sample_->setEnabled(true);
		x_->setEnabled(false);
		y_->setEnabled(false);
		z_->setEnabled(false);
		theta_->setEnabled(false);

		if(info_->samplePlateId() != REIXSBeamline::bl()->samplePlate()->id())
			info_->setSamplePlateId(REIXSBeamline::bl()->samplePlate()->id());
		if(sample_->currentIndex() >= 0)
			info_->setSampleIndex(sample_->currentIndex());
	}
	else {
		sample_->setEnabled(false);
		x_->setEnabled(true);
		y_->setEnabled(true);
		z_->setEnabled(true);
		theta_->setEnabled(true);

		info_->setSamplePlateId(0);	// use manual positions instead

		info_->setX(x_->value());
		info_->setY(y_->value());
		info_->setZ(z_->value());
		info_->setTheta(theta_->value());
	}
}

void REIXSSampleMoveActionEditor::onSampleActivated(int index)
{
	// tell the action info to use this sample index.
	info_->setSampleIndex(index);
}

void REIXSSampleMoveActionEditor::onSpinBoxEditingFinished()
{
	info_->setX(x_->value());
	info_->setY(y_->value());
	info_->setZ(z_->value());
	info_->setTheta(theta_->value());
}
