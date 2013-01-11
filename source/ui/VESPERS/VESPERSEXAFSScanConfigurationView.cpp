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

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>

VESPERSEXAFSScanConfigurationView::VESPERSEXAFSScanConfigurationView(VESPERSEXAFSScanConfiguration *config, QWidget *parent)
	: VESPERSScanConfigurationView(parent)
{
	config_ = config;
	AMTopFrame *frame = new AMTopFrame("VESPERS EXAFS Configuration");

	// Regions setup
	regionsView_ = new AMRegionsView(config_->regions());
	regionsView_->setMinimumWidth(300);
	regionsLineView_ = new AMEXAFSLineView(config_->exafsRegions());

	// The fluorescence detector setup
	QGroupBox *fluorescenceDetectorGroupBox = addFluorescenceDetectorSelectionView();
	connect(fluorescenceButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onFluorescenceChoiceChanged(int)));
	connect(config_->dbObject(), SIGNAL(fluorescenceDetectorChanged(int)), this, SLOT(updateFluorescenceDetector(int)));
	fluorescenceButtonGroup_->button((int)config_->fluorescenceDetector())->setChecked(true);

	// Ion chamber selection
	QGroupBox *ItGroupBox = addItSelectionView();
	connect(ItGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onItClicked(int)));
	connect(config_->dbObject(), SIGNAL(transmissionChoiceChanged(int)), this, SLOT(updateItButtons(int)));

	QGroupBox *I0GroupBox = addI0SelectionView();
	connect(I0Group_, SIGNAL(buttonClicked(int)), this, SLOT(onI0Clicked(int)));
	connect(config_->dbObject(), SIGNAL(incomingChoiceChanged(int)), this, SLOT(updateI0Buttons(int)));

	I0Group_->button((int)config_->incomingChoice())->click();
	ItGroup_->button((int)config_->transmissionChoice())->click();

	QHBoxLayout *ionChambersLayout = new QHBoxLayout;
	ionChambersLayout->addWidget(I0GroupBox);
	ionChambersLayout->addWidget(ItGroupBox);

	// Scan name selection
	scanName_ = addScanNameView(config_->name());
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(config_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
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
	else
		onEdgeChanged();

	connect(config_, SIGNAL(edgeChanged(QString)), this, SLOT(onEdgeChanged()));

	QCheckBox *useFixedTime = new QCheckBox("Use fixed time (EXAFS)");
	useFixedTime->setEnabled(config_->useFixedTime());
	connect(config_->exafsRegions(), SIGNAL(regionsHaveKSpaceChanged(bool)), useFixedTime, SLOT(setEnabled(bool)));
	connect(useFixedTime, SIGNAL(toggled(bool)), config_, SLOT(setUseFixedTime(bool)));

	QSpinBox *numberOfScans = new QSpinBox;
	numberOfScans->setMinimum(1);
	numberOfScans->setValue(config_->numberOfScans());
	numberOfScans->setAlignment(Qt::AlignCenter);
	connect(numberOfScans, SIGNAL(valueChanged(int)), config_, SLOT(setNumberOfScans(int)));
	connect(config_, SIGNAL(numberOfScansChanged(int)), this, SLOT(onEstimatedTimeChanged()));

	QFormLayout *numberOfScansLayout = new QFormLayout;
	numberOfScansLayout->addRow("Number of Scans:", numberOfScans);

	QFormLayout *energySetpointLayout = new QFormLayout;
	energySetpointLayout->addRow("Energy:", energy_);

	QHBoxLayout *energyLayout = new QHBoxLayout;
	energyLayout->addLayout(energySetpointLayout);
	energyLayout->addWidget(elementChoice_);
	energyLayout->addWidget(lineChoice_);

	// Setting the scan position.
	QGroupBox *goToPositionGroupBox = addGoToPositionView(config_->goToPosition(), config_->x(), config_->y());

	connect(config_, SIGNAL(gotoPositionChanged(bool)), goToPositionCheckBox_, SLOT(setChecked(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), config_, SLOT(setGoToPosition(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), setCurrentPositionButton_, SLOT(setEnabled(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), xPosition_, SLOT(setEnabled(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), yPosition_, SLOT(setEnabled(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), savedXPosition_, SLOT(setEnabled(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), savedYPosition_, SLOT(setEnabled(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), positionsSaved_, SLOT(setEnabled(bool)));
	connect(setCurrentPositionButton_, SIGNAL(clicked()), this, SLOT(setScanPosition()));

	setSampleStage(VESPERSBeamline::vespers()->experimentConfiguration()->sampleStageChoice());

	// The estimated scan time.
	estimatedTime_ = new QLabel;
	estimatedSetTime_ = new QLabel;
	connect(config_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
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
	roiTextBox_->setVisible(config_->fluorescenceDetector() == VESPERS::NoXRF ? false : true);

	// Label showing where the data will be saved.
	QLabel *exportPath = addExportPathLabel();

	// Label with a help message for EXAFS.
	QLabel *helpMessage = new QLabel("Note when using EXAFS: when using variable integration time, the time column is the maximum time.");

	// Default XANES and EXAFS buttons.
	QPushButton *defaultXANESButton = new QPushButton("Default XANES");
	connect(defaultXANESButton, SIGNAL(clicked()), this, SLOT(onDefaultXANESScanClicked()));

	QPushButton *defaultEXAFSButton = new QPushButton("Default EXAFS");
	connect(defaultEXAFSButton, SIGNAL(clicked()), this, SLOT(onDefaultEXAFSScanClicked()));

	// Setting up the steps to show the time offset for scan time estimation.
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
	setContextMenuPolicy(Qt::CustomContextMenu);

	QGroupBox *timeOffsetBox = addTimeOffsetLabel(config_->timeOffset());
	connect(timeOffset_, SIGNAL(valueChanged(double)), this, SLOT(setTimeOffset(double)));

	QVBoxLayout *defaultLayout = new QVBoxLayout;
	defaultLayout->addSpacing(35);
	defaultLayout->addWidget(defaultXANESButton);
	defaultLayout->addWidget(defaultEXAFSButton);
	defaultLayout->addStretch();

	// Auto-export option.
	QGroupBox *autoExportGroupBox = addExporterOptionsView(QStringList(), config_->exportSpectraSources());
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), config_, SLOT(setExportSpectraSources(bool)));

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
	configViewLayout->addWidget(regionsLineView_, 0, Qt::AlignCenter);
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
	config_->setFluorescenceDetector(id);
	roiTextBox_->setVisible(id > 0 ? true : false);
	updateRoiText();
}

void VESPERSEXAFSScanConfigurationView::updateRoiText()
{
	switch((int)config_->fluorescenceDetector()){

	case VESPERS::NoXRF:
		config_->setRoiInfoList(AMROIInfoList());
		break;

	case VESPERS::SingleElement:
		config_->setRoiInfoList(*VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList());
		break;

	case VESPERS::FourElement:
		config_->setRoiInfoList(*VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList());
		break;

	case VESPERS::SingleElement | VESPERS::FourElement:{

		AMROIInfoList list;
		AMROIInfoList singleElList = *VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList();
		AMROIInfoList fourElList = *VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList();

		for (int i = 0, count = singleElList.count(); i < count; i++)
			list.append(singleElList.at(i));

		for (int i = 0, count = fourElList.count(); i < count; i++)
			list.append(fourElList.at(i));

		config_->setRoiInfoList(list);
		break;
	}
	}

	updateAndSetRoiTextBox(int(config_->fluorescenceDetector()));
}

void VESPERSEXAFSScanConfigurationView::onElementChoiceClicked()
{
	const AMElement *el = AMPeriodicTableDialog::getElement(this);

	if (el){

		elementChoice_->setText(el->symbol());
		fillLinesComboBox(el);
		onLinesComboBoxIndexChanged(0);
	}
}

void VESPERSEXAFSScanConfigurationView::fillLinesComboBox(const AMElement *el)
{
	if (!el)
		return;

	QPair<QString, QString> edge;
	lineChoice_->clear();

	for (int i = 0; i < el->edges().size(); i++){

		edge = el->edges().at(i);

		if (edge.second.toDouble() <= 30000 && edge.second.toDouble() >= 6700)
			lineChoice_->addItem(edge.first+": "+edge.second+" eV", edge.second.toDouble());
	}
}

void VESPERSEXAFSScanConfigurationView::onLinesComboBoxIndexChanged(int index)
{
	if (lineChoice_->count() == 0 || index == -1)
		return;

	energy_->setValue(lineChoice_->itemData(index).toDouble());
	setEnergy();
	config_->setEdge(elementChoice_->text()+" "+lineChoice_->itemText(index).split(":").first());
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

	config_->setTransmissionChoice(id);
}

void VESPERSEXAFSScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + VESPERS::convertTimeToString(config_->totalTime()));
	estimatedSetTime_->setText("Estimated time for set:\t" + VESPERS::convertTimeToString(config_->totalTime()*config_->numberOfScans()));
}

void VESPERSEXAFSScanConfigurationView::onEdgeChanged()
{
	QString currentChoice = elementChoice_->text() % " " % lineChoice_->itemText(lineChoice_->currentIndex()).split(":").first();
	if (config_->edge() == currentChoice)
		return;

	elementChoice_->setText(config_->edge().split(" ").first());
	lineChoice_->blockSignals(true);
	fillLinesComboBox(AMPeriodicTable::table()->elementBySymbol(elementChoice_->text()));
	lineChoice_->blockSignals(false);
	lineChoice_->setCurrentIndex(lineChoice_->findText(config_->edge().split(" ").last(), Qt::MatchStartsWith | Qt::MatchCaseSensitive));

	if (energy_->value() != config_->energy())
		energy_->setValue(config_->energy());
}

void VESPERSEXAFSScanConfigurationView::onDefaultXANESScanClicked()
{
	while (config_->regionCount() != 1)
	{
		config_->deleteRegion(0);
	}

	config_->setRegionStart(0, -30);
	config_->setRegionDelta(0, 0.5);
	config_->setRegionEnd(0, 40);
	config_->setRegionTime(0, 1);
	config_->exafsRegions()->setType(0, AMEXAFSRegion::Energy);
}

void VESPERSEXAFSScanConfigurationView::onDefaultEXAFSScanClicked()
{
	while (config_->regionCount() != 1)
	{
		config_->deleteRegion(0);
	}

	config_->exafsRegions()->setType(0, AMEXAFSRegion::Energy);
	config_->setRegionStart(0, -200);
	config_->setRegionDelta(0, 10);
	config_->setRegionEnd(0, -30);
	config_->setRegionTime(0, 1);

	config_->regions()->addRegion(1, -30, 0.5, 40, 1);
	config_->exafsRegions()->setType(1, AMEXAFSRegion::Energy);

	config_->regions()->addRegion(2, 40, 0.05, 857.4627, 10); // 857.4627 = 15k
	config_->exafsRegions()->setType(2, AMEXAFSRegion::kSpace);
	config_->exafsRegions()->setEndByType(2, 15, AMEXAFSRegion::kSpace);
}

void VESPERSEXAFSScanConfigurationView::setSampleStage(bool sampleStage)
{
	if (sampleStage){

		disconnect(VESPERSBeamline::vespers()->realSampleStage(), SIGNAL(horizontalSetpointChanged(double)), xPosition_, SLOT(setValue(double)));
		disconnect(VESPERSBeamline::vespers()->realSampleStage(), SIGNAL(verticalSetpointChanged(double)), yPosition_, SLOT(setValue(double)));
		connect(VESPERSBeamline::vespers()->pseudoSampleStage(), SIGNAL(horizontalSetpointChanged(double)), xPosition_, SLOT(setValue(double)));
		connect(VESPERSBeamline::vespers()->pseudoSampleStage(), SIGNAL(verticalSetpointChanged(double)), yPosition_, SLOT(setValue(double)));
		xPosition_->setValue(VESPERSBeamline::vespers()->pseudoSampleStage()->horizontalPosition());
		yPosition_->setValue(VESPERSBeamline::vespers()->pseudoSampleStage()->verticalPosition());
	}
	else{

		disconnect(VESPERSBeamline::vespers()->pseudoSampleStage(), SIGNAL(horizontalSetpointChanged(double)), xPosition_, SLOT(setValue(double)));
		disconnect(VESPERSBeamline::vespers()->pseudoSampleStage(), SIGNAL(verticalSetpointChanged(double)), yPosition_, SLOT(setValue(double)));
		connect(VESPERSBeamline::vespers()->realSampleStage(), SIGNAL(horizontalSetpointChanged(double)), xPosition_, SLOT(setValue(double)));
		connect(VESPERSBeamline::vespers()->realSampleStage(), SIGNAL(verticalSetpointChanged(double)), yPosition_, SLOT(setValue(double)));
		xPosition_->setValue(VESPERSBeamline::vespers()->realSampleStage()->horizontalPosition());
		yPosition_->setValue(VESPERSBeamline::vespers()->realSampleStage()->verticalPosition());
	}
}
