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


#include "VESPERSEXAFSScanConfigurationView.h"
#include "ui/AMTopFrame.h"
#include "ui/util/AMPeriodicTableDialog.h"
#include "util/AMPeriodicTable.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "util/VESPERS/GeneralUtilities.h"
#include "ui/dataman/AMEXAFSScanAxisView.h"
#include "util/AMEnergyToKSpaceCalculator.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>

VESPERSEXAFSScanConfigurationView::~VESPERSEXAFSScanConfigurationView(){}

VESPERSEXAFSScanConfigurationView::VESPERSEXAFSScanConfigurationView(VESPERSEXAFSScanConfiguration *config, QWidget *parent)
	: VESPERSScanConfigurationView(parent)
{
	configuration_ = config;
	AMTopFrame *frame = new AMTopFrame("VESPERS EXAFS Configuration");

	// Regions setup
	regionsView_ = new AMEXAFSScanAxisView("XAS Region Configuration", configuration_);

	// The fluorescence detector setup
	QGroupBox *fluorescenceDetectorGroupBox = addFluorescenceDetectorSelectionView();
	connect(fluorescenceButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onFluorescenceChoiceChanged(int)));
	connect(configuration_->dbObject(), SIGNAL(fluorescenceDetectorChanged(int)), this, SLOT(updateFluorescenceDetector(int)));
	fluorescenceButtonGroup_->button((int)configuration_->fluorescenceDetector())->setChecked(true);

	// Ion chamber selection
	QGroupBox *ItGroupBox = addItSelectionView();
	connect(ItGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onItClicked(int)));
	connect(configuration_->dbObject(), SIGNAL(transmissionChoiceChanged(int)), this, SLOT(updateItButtons(int)));

	QGroupBox *I0GroupBox = addI0SelectionView();
	connect(I0Group_, SIGNAL(buttonClicked(int)), this, SLOT(onI0Clicked(int)));
	connect(configuration_->dbObject(), SIGNAL(incomingChoiceChanged(int)), this, SLOT(updateI0Buttons(int)));

	I0Group_->button((int)configuration_->incomingChoice())->click();
	ItGroup_->button((int)configuration_->transmissionChoice())->click();

	QHBoxLayout *ionChambersLayout = new QHBoxLayout;
	ionChambersLayout->addWidget(I0GroupBox);
	ionChambersLayout->addWidget(ItGroupBox);

	// Scan name selection
	scanName_ = addScanNameView(configuration_->name());
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
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

	if (configuration_->edge().isEmpty()){

		elementChoice_->setText("Cu");
		fillLinesComboBox(AMPeriodicTable::table()->elementBySymbol("Cu"));
		lineChoice_->setCurrentIndex(0);
	}
	// Resets the view for the view to what it should be.  Using the saved for the energy in case it is different from the original line energy.
	else
		onEdgeChanged();

	connect(configuration_, SIGNAL(edgeChanged(QString)), this, SLOT(onEdgeChanged()));

	QCheckBox *useFixedTime = new QCheckBox("Use fixed time (EXAFS)");
	useFixedTime->setEnabled(configuration_->useFixedTime());
//	connect(config_->exafsRegions(), SIGNAL(regionsHaveKSpaceChanged(bool)), useFixedTime, SLOT(setEnabled(bool)));
	connect(useFixedTime, SIGNAL(toggled(bool)), configuration_, SLOT(setUseFixedTime(bool)));

	QSpinBox *numberOfScans = new QSpinBox;
	numberOfScans->setMinimum(1);
	numberOfScans->setValue(configuration_->numberOfScans());
	numberOfScans->setAlignment(Qt::AlignCenter);
	connect(numberOfScans, SIGNAL(valueChanged(int)), configuration_, SLOT(setNumberOfScans(int)));
	connect(configuration_, SIGNAL(numberOfScansChanged(int)), this, SLOT(onEstimatedTimeChanged()));

	QFormLayout *numberOfScansLayout = new QFormLayout;
	numberOfScansLayout->addRow("Number of Scans:", numberOfScans);

	QFormLayout *energySetpointLayout = new QFormLayout;
	energySetpointLayout->addRow("Energy:", energy_);

	QHBoxLayout *energyLayout = new QHBoxLayout;
	energyLayout->addLayout(energySetpointLayout);
	energyLayout->addWidget(elementChoice_);
	energyLayout->addWidget(lineChoice_);

	// Setting the scan position.
	QGroupBox *goToPositionGroupBox = addGoToPositionView(configuration_->goToPosition(), configuration_->x(), configuration_->y());

	connect(configuration_, SIGNAL(gotoPositionChanged(bool)), goToPositionCheckBox_, SLOT(setChecked(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), configuration_, SLOT(setGoToPosition(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), setCurrentPositionButton_, SLOT(setEnabled(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), savedXPosition_, SLOT(setEnabled(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), savedYPosition_, SLOT(setEnabled(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), positionsSaved_, SLOT(setEnabled(bool)));
	connect(setCurrentPositionButton_, SIGNAL(clicked()), this, SLOT(setScanPosition()));
	connect(configuration_->dbObject(), SIGNAL(motorChanged(int)), this, SLOT(onMotorsUpdated(int)));

	onMotorsUpdated(configuration_->motor());

	// The estimated scan time.
	estimatedTime_ = new QLabel;
	estimatedSetTime_ = new QLabel;
	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	onEstimatedTimeChanged();

	// The roi text edit.
	roiText_ = new QTextEdit;
	roiText_->setReadOnly(true);

	QPushButton *configureXRFDetectorButton = new QPushButton(QIcon(":/hammer-wrench.png"), "Configure XRF Detector");
	connect(configureXRFDetectorButton, SIGNAL(clicked()), this, SLOT(onConfigureXRFDetectorClicked()));

	QFormLayout *roiTextLayout = new QFormLayout;
	roiTextLayout->addRow(roiText_);
	roiTextLayout->addRow(configureXRFDetectorButton);

	roiTextBox_ = new QGroupBox("Regions Of Interest");
	roiTextBox_->setLayout(roiTextLayout);
	roiTextBox_->setVisible(configuration_->fluorescenceDetector() == VESPERS::NoXRF ? false : true);

	// Label showing where the data will be saved.
	QLabel *exportPath = addExportPathLabel();

	// Label with a help message for EXAFS.
	QLabel *helpMessage = new QLabel("Note when using EXAFS: when using variable integration time, the time column is the maximum time.");

	// Default XANES and EXAFS buttons.
	QPushButton *defaultXANESButton = new QPushButton("Default XANES");
	connect(defaultXANESButton, SIGNAL(clicked()), this, SLOT(setupDefaultXANESScanRegions()));
	QPushButton *defaultEXAFSButton = new QPushButton("Default EXAFS");
	connect(defaultEXAFSButton, SIGNAL(clicked()), this, SLOT(setupDefaultEXAFSScanRegions()));

	// Setting up the steps to show the time offset for scan time estimation.
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
	setContextMenuPolicy(Qt::CustomContextMenu);

	QGroupBox *timeOffsetBox = addTimeOffsetLabel(configuration_->timeOffset());
	connect(timeOffset_, SIGNAL(valueChanged(double)), this, SLOT(setTimeOffset(double)));

	QVBoxLayout *defaultLayout = new QVBoxLayout;
	defaultLayout->addSpacing(35);
	defaultLayout->addWidget(defaultXANESButton);
	defaultLayout->addWidget(defaultEXAFSButton);
	defaultLayout->addStretch();

	// Auto-export option.
	QGroupBox *autoExportGroupBox = addExporterOptionsView(QStringList(), configuration_->exportSpectraSources(), configuration_->exportSpectraInRows());
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), configuration_, SLOT(setExportSpectraSources(bool)));
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), exportSpectraInRows_, SLOT(setEnabled(bool)));
	connect(exportSpectraInRows_, SIGNAL(toggled(bool)), this, SLOT(updateExportSpectraInRows(bool)));

	// Setting up the layout.
	QGridLayout *contentsLayout = new QGridLayout;
	contentsLayout->addWidget(regionsView_, 1, 1, 2, 2);
	contentsLayout->addWidget(fluorescenceDetectorGroupBox, 1, 3);
	contentsLayout->addLayout(scanNameLayout, 4, 1);
	contentsLayout->addLayout(energyLayout, 0, 1, 1, 3);
	contentsLayout->addWidget(goToPositionGroupBox, 4, 3, 4, 1);
	contentsLayout->addLayout(ionChambersLayout, 2, 3, 2, 1);
	contentsLayout->addWidget(roiTextBox_, 1, 4, 2, 2);
	contentsLayout->addWidget(useFixedTime, 3, 1);
	contentsLayout->addWidget(estimatedTime_, 6, 1, 1, 2);
	contentsLayout->addWidget(estimatedSetTime_, 7, 1, 1, 2);
	contentsLayout->addLayout(numberOfScansLayout, 5, 1);
	contentsLayout->addWidget(timeOffsetBox, 8, 1, 1, 2);
	contentsLayout->addWidget(autoExportGroupBox, 4, 5, 2, 3);

	QHBoxLayout *squeezeContents = new QHBoxLayout;
	squeezeContents->addStretch();
	squeezeContents->addLayout(defaultLayout);
	squeezeContents->addLayout(contentsLayout);
	squeezeContents->addStretch();

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addWidget(frame);
	configViewLayout->addStretch();
	configViewLayout->addSpacing(30);
	configViewLayout->addLayout(squeezeContents);
	configViewLayout->addSpacing(30);
	configViewLayout->addWidget(exportPath, 0, Qt::AlignCenter);
	configViewLayout->addSpacing(30);
	configViewLayout->addWidget(helpMessage, 0, Qt::AlignCenter);
	configViewLayout->addStretch();

	setLayout(configViewLayout);
}

void VESPERSEXAFSScanConfigurationView::onFluorescenceChoiceChanged(int id)
{
	configuration_->setFluorescenceDetector(id);
	roiTextBox_->setVisible(id > 0 ? true : false);
	updateRoiText();
}

void VESPERSEXAFSScanConfigurationView::onScanNameEdited()
{
	configuration_->setName(scanName_->text());
	configuration_->setUserScanName(scanName_->text());

	double n = 0;

	VESPERS::Motors motor = configuration_->motor();

	if (motor == (VESPERS::H | VESPERS::V))
		n = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->normalControl()->value();

	else if (motor == (VESPERS::X | VESPERS::Z))
		n = VESPERSBeamline::vespers()->sampleStageY()->value();

	configuration_->setNormalPosition(n);
}

void VESPERSEXAFSScanConfigurationView::updateRoiText()
{
	VESPERS::FluorescenceDetectors xrfFlag = configuration_->fluorescenceDetector();

	if (xrfFlag == VESPERS::NoXRF)
		configuration_->setRoiInfoList(AMROIInfoList());

	else if (xrfFlag == VESPERS::SingleElement)
		configuration_->setRoiInfoList(*VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList());

	else if (xrfFlag == VESPERS::FourElement)
		configuration_->setRoiInfoList(*VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList());

	else if (xrfFlag == (VESPERS::SingleElement | VESPERS::FourElement)){

		AMROIInfoList list;
		AMROIInfoList singleElList = *VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList();
		AMROIInfoList fourElList = *VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList();

		for (int i = 0, count = singleElList.count(); i < count; i++)
			list.append(singleElList.at(i));

		for (int i = 0, count = fourElList.count(); i < count; i++)
			list.append(fourElList.at(i));

		configuration_->setRoiInfoList(list);
	}

	updateAndSetRoiTextBox(int(xrfFlag));
}

void VESPERSEXAFSScanConfigurationView::onElementChoiceClicked()
{
	AMElement *el = AMPeriodicTableDialog::getElement(this);

	if (el){

		elementChoice_->setText(el->symbol());
		fillLinesComboBox(el);
		lineChoice_->setCurrentIndex(0);
	}
}

void VESPERSEXAFSScanConfigurationView::fillLinesComboBox(AMElement *el)
{
	if (!el)
		return;

	AMAbsorptionEdge edge;
	lineChoice_->clear();

	for (int i = 0; i < el->absorptionEdges().size(); i++){

		edge = el->absorptionEdges().at(i);

		if (edge.energy() <= 30000 && edge.energy() >= 6700)
			lineChoice_->addItem(edge.name()+": "+edge.energyString()+" eV", edge.energy());
	}

	lineChoice_->setCurrentIndex(-1);
}

void VESPERSEXAFSScanConfigurationView::onLinesComboBoxIndexChanged(int index)
{
	if (lineChoice_->count() == 0 || index == -1)
		return;

	energy_->setValue(lineChoice_->itemData(index).toDouble());
	setEnergy();
	configuration_->setEdge(elementChoice_->text()+" "+lineChoice_->itemText(index).split(":").first());
}

void VESPERSEXAFSScanConfigurationView::setEnergy()
{
	configuration_->setEnergy(energy_->value());
	regionsView_->setEdgeEnergy(energy_->value());
}

void VESPERSEXAFSScanConfigurationView::onItClicked(int id)
{
	// If the new It is at or upstream of I0, move I0.  Using id-1 is safe because Isplit can't be chosen for It.
	if (id <= I0Group_->checkedId())
		I0Group_->button(id-1)->click();

	for (int i = 0; i < id; i++)
		I0Group_->button(i)->setEnabled(true);

	for (int i = id; i < 4; i++)
		I0Group_->button(i)->setEnabled(false);

	configuration_->setTransmissionChoice(id);
}

void VESPERSEXAFSScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + VESPERS::convertTimeToString(configuration_->totalTime()));
	estimatedSetTime_->setText("Estimated time for set:\t" + VESPERS::convertTimeToString(configuration_->totalTime()*configuration_->numberOfScans()));
}

void VESPERSEXAFSScanConfigurationView::onEdgeChanged()
{
	QString currentChoice = elementChoice_->text() % " " % lineChoice_->itemText(lineChoice_->currentIndex()).split(":").first();
	if (configuration_->edge() == currentChoice)
		return;

	elementChoice_->setText(configuration_->edge().split(" ").first());
	lineChoice_->blockSignals(true);
	fillLinesComboBox(AMPeriodicTable::table()->elementBySymbol(elementChoice_->text()));
	lineChoice_->blockSignals(false);
	lineChoice_->setCurrentIndex(lineChoice_->findText(configuration_->edge().split(" ").last(), Qt::MatchStartsWith | Qt::MatchCaseSensitive));

	if (energy_->value() != configuration_->energy())
		energy_->setValue(configuration_->energy());
}

void VESPERSEXAFSScanConfigurationView::setupDefaultXANESScanRegions()
{
	while (configuration_->scanAxisAt(0)->regionCount())
	{
		regionsView_->removeEXAFSRegion(0);
	}

	AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
	region->setEdgeEnergy(configuration_->energy());
	region->setRegionStart(configuration_->energy() - 30);
	region->setRegionStep(0.5);
	region->setRegionEnd(configuration_->energy() + 40);
	region->setRegionTime(1.0);
	regionsView_->insertEXAFSRegion(0, region);
}

void VESPERSEXAFSScanConfigurationView::setupDefaultEXAFSScanRegions()
{
	while (configuration_->scanAxisAt(0)->regionCount())
	{
		regionsView_->removeEXAFSRegion(0);
	}

	AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
	region->setEdgeEnergy(configuration_->energy());
	region->setRegionStart(configuration_->energy() - 200);
	region->setRegionStep(10);
	region->setRegionEnd(configuration_->energy() - 30);
	region->setRegionTime(1.0);
	regionsView_->insertEXAFSRegion(0, region);

	region = new AMScanAxisEXAFSRegion;
	region->setEdgeEnergy(configuration_->energy());
	region->setRegionStart(configuration_->energy() - 30);
	region->setRegionStep(0.5);
	region->setRegionEnd(configuration_->energy() + 40);
	region->setRegionTime(1.0);
	regionsView_->insertEXAFSRegion(1, region);

	region = new AMScanAxisEXAFSRegion;
	region->setEdgeEnergy(configuration_->energy());
	region->setInKSpace(true);
	region->setRegionStart(AMEnergyToKSpaceCalculator::k(region->edgeEnergy(), configuration_->energy() + 40));
	region->setRegionStep(0.05);
	region->setRegionEnd(10);
	region->setRegionTime(1.0);
	region->setMaximumTime(10.0);
	regionsView_->insertEXAFSRegion(2, region);
}

void VESPERSEXAFSScanConfigurationView::onMotorsUpdated(int id)
{
	if (id == int(VESPERS::H | VESPERS::V) || id == (VESPERS::AttoH | VESPERS::AttoV)){

		savedXPosition_->setText(QString("H: %1 mm").arg(0.0, 0, 'g', 3));
		savedYPosition_->setText(QString("V: %1 mm").arg(0.0, 0, 'g', 3));
	}

	else if (id == int(VESPERS::X | VESPERS::Z) || id == (VESPERS::AttoX | VESPERS::AttoZ)){

		savedXPosition_->setText(QString("X: %1 mm").arg(0.0, 0, 'g', 3));
		savedYPosition_->setText(QString("Z: %1 mm").arg(0.0, 0, 'g', 3));
	}

	positionsSaved_->setText("Unsaved");
	QPalette palette(this->palette());
	palette.setColor(QPalette::Active, QPalette::WindowText, Qt::darkRed);
	positionsSaved_->setPalette(palette);
}

void VESPERSEXAFSScanConfigurationView::setScanPosition()
{
	double x = 0;
	double y = 0;
	VESPERS::Motors motor = configuration_->motor();

	if (motor == (VESPERS::H | VESPERS::V)){

		x = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalControl()->value();
		y = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalControl()->value();
		savedXPosition_->setText(QString("H: %1 mm").arg(x, 0, 'g', 3));
		savedYPosition_->setText(QString("V: %1 mm").arg(y, 0, 'g', 3));
	}

	else if (motor == (VESPERS::X | VESPERS::Z)){

		x = VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->horizontalControl()->value();
		y = VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->verticalControl()->value();
		savedXPosition_->setText(QString("X: %1 mm").arg(x, 0, 'g', 3));
		savedYPosition_->setText(QString("Z: %1 mm").arg(y, 0, 'g', 3));
	}

	else if (motor == (VESPERS::AttoH | VESPERS::AttoV)){

		x = VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->horizontalControl()->value();
		y = VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->verticalControl()->value();
		savedXPosition_->setText(QString("H: %1 mm").arg(x, 0, 'g', 3));
		savedYPosition_->setText(QString("V: %1 mm").arg(y, 0, 'g', 3));
	}

	else if (motor == (VESPERS::AttoX | VESPERS::AttoZ)){

		x = VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->horizontalControl()->value();
		y = VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->verticalControl()->value();
		savedXPosition_->setText(QString("X: %1 mm").arg(x, 0, 'g', 3));
		savedYPosition_->setText(QString("Z: %1 mm").arg(y, 0, 'g', 3));
	}

	configuration_->setPosition(x, y);
	positionsSaved_->setText("Saved");
	QPalette palette(this->palette());
	palette.setColor(QPalette::Active, QPalette::WindowText, Qt::darkGreen);
	positionsSaved_->setPalette(palette);
}
