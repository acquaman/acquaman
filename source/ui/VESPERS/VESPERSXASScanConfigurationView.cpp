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
#include "ui/util/AMPeriodicTableDialog.h"
#include "util/AMPeriodicTable.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "util/VESPERS/GeneralUtilities.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>

VESPERSXASScanConfigurationView::VESPERSXASScanConfigurationView(VESPERSXASScanConfiguration *config, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	config_ = config;
	AMTopFrame *frame = new AMTopFrame("VESPERS XAS Configuration");

	// Regions setup
	config_->regions()->setSensibleRange(-30, 40);
	config_->regions()->setEnergyControl(VESPERSBeamline::vespers()->energyRelative());

	regionsView_ = new AMRegionsView(config_->regions());
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
	connect(fluorescenceButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onFluorescenceChoiceChanged(int)));

	fluorescenceButtonGroup->button((int)config_->fluorescenceDetectorChoice())->setChecked(true);

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

	I0Group_->button((int)config_->incomingChoice())->click();
	ItGroup_->button((int)config_->transmissionChoice())->click();

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
	scanName_->setText(config_->name());
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

	elementChoice_ = new QToolButton;
	connect(elementChoice_, SIGNAL(clicked()), this, SLOT(onElementChoiceClicked()));

	lineChoice_ = new QComboBox;
	connect(lineChoice_, SIGNAL(currentIndexChanged(int)), this, SLOT(onLinesComboBoxIndexChanged(int)));

	if (config_->edge().isEmpty()){

		elementChoice_->setText("Cu");
		fillLinesComboBox(AMPeriodicTable::table()->elementBySymbol("Cu"));
		onLinesComboBoxIndexChanged(0);
	}
	// Resets the view for the view to what it should be.  Using the saved for the energy in case it is different from the original line energy.
	else {

		double energy = config_->energy();
		QString edgeName(config_->edge());

		elementChoice_->setText(config_->edge().split(" ").first());
		fillLinesComboBox(AMPeriodicTable::table()->elementBySymbol(elementChoice_->text()));
		config_->setEdge(edgeName);
		lineChoice_->setCurrentIndex(lineChoice_->findText(config_->edge().split(" ").last(), Qt::MatchStartsWith | Qt::MatchCaseSensitive));

		if (energy_->value() != energy)
			energy_->setValue(energy);
	}

	QFormLayout *energySetpointLayout = new QFormLayout;
	energySetpointLayout->addRow("Energy:", energy_);

	QHBoxLayout *energyLayout = new QHBoxLayout;
	energyLayout->addLayout(energySetpointLayout);
	energyLayout->addWidget(elementChoice_);
	energyLayout->addWidget(lineChoice_);

	// Setting the scan position.
	QCheckBox *goToPosition = new QCheckBox("Choose Position");
	goToPosition->setChecked(config_->goToPosition());

	QPushButton *setCurrentPosition = new QPushButton("Set Position");
	setCurrentPosition->setEnabled(goToPosition->isChecked());
	connect(setCurrentPosition, SIGNAL(clicked()), this, SLOT(setScanPosition()));

	xPosition_ = new QDoubleSpinBox;
	xPosition_->setEnabled(goToPosition->isChecked());
	xPosition_->setDecimals(3);
	xPosition_->setRange(-100, 100);
	xPosition_->setValue(config_->x());
	xPosition_->setSuffix(" mm");
	connect(VESPERSBeamline::vespers()->pseudoSampleStage(), SIGNAL(horizontalSetpointChanged(double)), xPosition_, SLOT(setValue(double)));

	yPosition_ = new QDoubleSpinBox;
	yPosition_->setEnabled(goToPosition->isChecked());
	yPosition_->setDecimals(3);
	yPosition_->setRange(-100, 100);
	yPosition_->setValue(config_->y());
	yPosition_->setSuffix(" mm");
	connect(VESPERSBeamline::vespers()->pseudoSampleStage(), SIGNAL(verticalSetpointChanged(double)), yPosition_, SLOT(setValue(double)));

	connect(goToPosition, SIGNAL(toggled(bool)), config_, SLOT(setGoToPosition(bool)));
	connect(goToPosition, SIGNAL(toggled(bool)), setCurrentPosition, SLOT(setEnabled(bool)));
	connect(goToPosition, SIGNAL(toggled(bool)), xPosition_, SLOT(setEnabled(bool)));
	connect(goToPosition, SIGNAL(toggled(bool)), yPosition_, SLOT(setEnabled(bool)));

	QFormLayout *positionLayout = new QFormLayout;
	positionLayout->addRow(goToPosition);
	positionLayout->addRow(setCurrentPosition);
	positionLayout->addRow("x:", xPosition_);
	positionLayout->addRow("y:", yPosition_);

	// Setting the time.
	QDoubleSpinBox *time = new QDoubleSpinBox;
	time->setSuffix(" s");
	time->setMinimum(0);
	time->setMaximum(100000);
	connect(time, SIGNAL(valueChanged(double)), config_, SLOT(setAccumulationTime(double)));
	time->setValue(config_->accumulationTime());

	QFormLayout *timeLayout = new QFormLayout;
	timeLayout->addRow("Time:", time);

	// The roi text edit.
	roiText_ = new QTextEdit;
	roiText_->setReadOnly(true);

	if (config_->fluorescenceDetectorChoice() == VESPERSXASScanConfiguration::None)
		roiText_->hide();

	else{

		roiText_->insertPlainText("Name\tLow (eV)\tHigh(eV)\n");

		for (int i = 0; i < config_->roiList().count(); i++)
			roiText_->insertPlainText(GeneralUtilities::addGreek(config_->roiList().at(i).name())+"\t" + QString::number(config_->roiList().at(i).low()) + "\t" + QString::number(config_->roiList().at(i).high()) +"\n");
	}

	// Setting up the layout.
	QGridLayout *contentsLayout = new QGridLayout;
	contentsLayout->addWidget(regionsView_, 3, 0, 1, 2);
	contentsLayout->addWidget(fluorescenceDetectorGroupBox, 2, 0);
	contentsLayout->addLayout(scanNameLayout, 0, 0);
	contentsLayout->addLayout(energyLayout, 1, 0, 1, 2);
	contentsLayout->addLayout(timeLayout, 0, 1);
	contentsLayout->addLayout(positionLayout, 2, 2);
	contentsLayout->addWidget(ionChambersGroupBox, 2, 1);
	contentsLayout->addWidget(roiText_, 3, 2, 2, 2);

	QHBoxLayout *squeezeContents = new QHBoxLayout;
	squeezeContents->addStretch();
	squeezeContents->addLayout(contentsLayout);
	squeezeContents->addStretch();

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addWidget(frame);
	configViewLayout->addStretch();
	configViewLayout->addLayout(squeezeContents);
	configViewLayout->addStretch();
	configViewLayout->addWidget(regionsLineView_, 0, Qt::AlignCenter);

	setLayout(configViewLayout);
}

void VESPERSXASScanConfigurationView::onFluorescenceChoiceChanged(int id)
{
	config_->setFluorescenceDetectorChoice(id);
	roiText_->clear();

	switch(id){

	case 0:
		config_->setRoiInfoList(AMROIInfoList());
		roiText_->hide();
		break;

	case 1:
		config_->setRoiInfoList(*VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList());
		roiText_->show();
		break;

	case 2:
		config_->setRoiInfoList(*VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList());
		roiText_->show();
		break;
	}

	roiText_->insertPlainText("Name\tLow (eV)\tHigh(eV)\n");

	for (int i = 0; i < config_->roiList().count(); i++)
		roiText_->insertPlainText(GeneralUtilities::addGreek(config_->roiList().at(i).name())+"\t" + QString::number(config_->roiList().at(i).low()) + "\t" + QString::number(config_->roiList().at(i).high()) +"\n");
}

void VESPERSXASScanConfigurationView::onElementChoiceClicked()
{
	AMElement *el = AMPeriodicTableDialog::getElement(this);

	if (el){

		elementChoice_->setText(el->symbol());
		fillLinesComboBox(el);
		onLinesComboBoxIndexChanged(0);
	}
}

void VESPERSXASScanConfigurationView::fillLinesComboBox(AMElement *el)
{
	QPair<QString, QString> edge;
	lineChoice_->clear();

	for (int i = 0; i < el->edges().size(); i++){

		edge = el->edges().at(i);

		if (edge.second.toDouble() <= 30000 && edge.second.toDouble() >= 6700)
			lineChoice_->addItem(edge.first+": "+edge.second+" eV", edge.second.toDouble());
	}
}

void VESPERSXASScanConfigurationView::onLinesComboBoxIndexChanged(int index)
{
	if (lineChoice_->count() == 0 || index == -1)
		return;

	energy_->setValue(lineChoice_->itemData(index).toDouble());
	setEnergy();
	config_->setEdge(elementChoice_->text()+" "+lineChoice_->itemText(index).split(":").first());
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
