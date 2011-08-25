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


#include "VESPERSXASScanConfigurationView.h"
#include "ui/AMTopFrame.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QToolButton>

VESPERSXASScanConfigurationView::VESPERSXASScanConfigurationView(VESPERSXASScanConfiguration *config, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	config_ = config;
	AMTopFrame *frame = new AMTopFrame("VESPERS XAS Configuration");

	// Regions setup
	regionsView_ = new AMXASRegionsView(config_->regions());
	regionsView_->setBeamlineEnergy(VESPERSBeamline::vespers()->energyRelative());

	regionsLineView_ = new AMRegionsLineView(config_->regions());

	// The fluorescence detector setup
	QButtonGroup *fluorescenceButtonGroup = new QButtonGroup;
	QRadioButton *tempButton;
	QVBoxLayout *fluorescenceDetectorLayout = new QVBoxLayout;

	tempButton = new QRadioButton("None");
	fluorescenceButtonGroup->addButton(tempButton, 0);
	fluorescenceDetectorLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Single Element Vortex");
	fluorescenceButtonGroup->addButton(tempButton, 1);
	fluorescenceDetectorLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Four Element Vortex");
	fluorescenceButtonGroup->addButton(tempButton, 2);
	fluorescenceDetectorLayout->addWidget(tempButton);
	tempButton->setChecked(true);
	connect(fluorescenceButtonGroup, SIGNAL(buttonClicked(int)), config_, SLOT(setFluorescenceDetectorChoice(int)));

	QGroupBox *fluorescenceDetectorGroupBox = new QGroupBox("Fluorescence Detector");
	fluorescenceDetectorGroupBox->setLayout(fluorescenceDetectorLayout);

	// Ion chamber selection
	QVBoxLayout *ItGroupLayout = new QVBoxLayout;

	ItGroup_ = new QButtonGroup;
	tempButton = new QRadioButton("Isplit");
	tempButton->setEnabled(false);
	tempButton->hide();
	ItGroup_->addButton(tempButton, 0);
	ItGroupLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Iprekb");
	tempButton->hide();
	ItGroup_->addButton(tempButton, 1);
	ItGroupLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Imini");
	tempButton->hide();
	ItGroup_->addButton(tempButton, 2);
	ItGroupLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Ipost");
	tempButton->hide();
	ItGroup_->addButton(tempButton, 3);
	ItGroupLayout->addWidget(tempButton);
	connect(ItGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onItClicked(int)));

	QVBoxLayout *I0GroupLayout = new QVBoxLayout;

	I0Group_ = new QButtonGroup;
	tempButton = new QRadioButton("Isplit");
	I0Group_->addButton(tempButton, 0);
	I0GroupLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Iprekb");
	I0Group_->addButton(tempButton, 1);
	I0GroupLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Imini");
	tempButton->setChecked(true);
	I0Group_->addButton(tempButton, 2);
	I0GroupLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Ipost");
	I0Group_->addButton(tempButton, 3);
	I0GroupLayout->addWidget(tempButton);
	connect(I0Group_, SIGNAL(buttonClicked(int)), this, SLOT(onI0Clicked(int)));

	QHBoxLayout *ItI0Layout = new QHBoxLayout;

	QButtonGroup *ItI0ButtonGroup = new QButtonGroup;
	QToolButton *ItI0temp = new QToolButton;
	ItI0temp->setText("I0");
	ItI0temp->setCheckable(true);
	ItI0ButtonGroup->addButton(ItI0temp, 0);
	ItI0Layout->addWidget(ItI0temp);
	ItI0temp->setChecked(true);
	ItI0temp = new QToolButton;
	ItI0temp->setText("It");
	ItI0temp->setCheckable(true);
	ItI0ButtonGroup->addButton(ItI0temp, 1);
	ItI0Layout->addWidget(ItI0temp);
	ItI0Layout->setSpacing(0);
	ItI0Layout->addStretch();
	connect(ItI0ButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onItI0Toggled(int)));

	I0Group_->button(2)->click();
	ItGroup_->button(3)->click();

	QHBoxLayout *ionChambersLayout = new QHBoxLayout;
	ionChambersLayout->addLayout(I0GroupLayout);
	ionChambersLayout->addLayout(ItGroupLayout);
	QVBoxLayout *ionChambersGroupBoxLayout = new QVBoxLayout;
	ionChambersGroupBoxLayout->addLayout(ItI0Layout);
	ionChambersGroupBoxLayout->addLayout(ionChambersLayout);

	QGroupBox *ionChambersGroupBox = new QGroupBox("Ion Chambers");
	ionChambersGroupBox->setLayout(ionChambersGroupBoxLayout);

	// Scan name selection
	scanName_ = new QLineEdit;
	scanName_->setText("XAS Scan");
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	onScanNameEdited();

	QFormLayout *scanNameLayout = new QFormLayout;
	scanNameLayout->addRow("Scan Name:", scanName_);

	// Energy (Eo) selection
	energy_ = new QDoubleSpinBox;
	energy_->setSuffix(" eV");
	energy_->setMinimum(0);
	energy_->setMaximum(30000);
	connect(energy_, SIGNAL(editingFinished()), this, SLOT(setEnergy()));
	connect(VESPERSBeamline::vespers()->mono(), SIGNAL(EoChanged(double)), this, SLOT(onEnergyChanged(double)));

	QFormLayout *energyLayout = new QFormLayout;
	energyLayout->addRow("Energy:", energy_);

	QGridLayout *contentsLayout = new QGridLayout;
	contentsLayout->addWidget(regionsLineView_, 0, 0, 1, 4, Qt::AlignCenter);
	contentsLayout->addWidget(regionsView_, 1, 0, 1, 3);
	contentsLayout->addWidget(fluorescenceDetectorGroupBox, 1, 3);
	contentsLayout->addLayout(scanNameLayout, 2, 0);
	contentsLayout->addLayout(energyLayout, 3, 0);
	contentsLayout->addWidget(ionChambersGroupBox, 2, 3);

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addWidget(frame);
	configViewLayout->addStretch();
	configViewLayout->addLayout(contentsLayout);
	configViewLayout->addStretch();

	setLayout(configViewLayout);
}

void VESPERSXASScanConfigurationView::onItI0Toggled(int id)
{
	if (id == 0){

		ItGroup_->button(0)->hide();
		ItGroup_->button(1)->hide();
		ItGroup_->button(2)->hide();
		ItGroup_->button(3)->hide();

		I0Group_->button(0)->show();
		I0Group_->button(1)->show();
		I0Group_->button(2)->show();
		I0Group_->button(3)->show();
	}
	else{

		I0Group_->button(0)->hide();
		I0Group_->button(1)->hide();
		I0Group_->button(2)->hide();
		I0Group_->button(3)->hide();

		ItGroup_->button(0)->show();
		ItGroup_->button(1)->show();
		ItGroup_->button(2)->show();
		ItGroup_->button(3)->show();
	}
}

void VESPERSXASScanConfigurationView::onItClicked(int id)
{
	// If the new It is at or upstream of I0, move I0.  Using id-1 is safe because Isplit can't be chosen for It.
	if (id <= I0Group_->checkedId())
		I0Group_->button(id-1)->click();

	for (int i = 0; i < id; i++)
		I0Group_->button(i)->setEnabled(true);

	for (int i = id; i < 4; i++)
		I0Group_->button(i)->setEnabled(false);

	config_->setTransmissionChoice(id);
}
