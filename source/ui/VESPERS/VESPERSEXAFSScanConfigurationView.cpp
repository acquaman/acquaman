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
	: AMScanConfigurationView(parent)
{
	config_ = config;
	AMTopFrame *frame = new AMTopFrame("VESPERS EXAFS Configuration");

	// Regions setup
	regionsView_ = new AMRegionsView(config_->regions());
	regionsView_->setMinimumWidth(300);
	regionsLineView_ = new AMEXAFSLineView(config_->exafsRegions());

	// The fluorescence detector setup
	fluorescenceButtonGroup_ = new QButtonGroup;
	QRadioButton *tempButton;
	QVBoxLayout *fluorescenceDetectorLayout = new QVBoxLayout;

	tempButton = new QRadioButton("None");
	fluorescenceButtonGroup_->addButton(tempButton, 0);
	fluorescenceDetectorLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Single Element Vortex");
	fluorescenceButtonGroup_->addButton(tempButton, 1);
	fluorescenceDetectorLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Four Element Vortex");
	fluorescenceButtonGroup_->addButton(tempButton, 2);
	fluorescenceDetectorLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Single && Four");
	fluorescenceButtonGroup_->addButton(tempButton, 3);	// 3 is SingleElement | FourElement
	fluorescenceDetectorLayout->addWidget(tempButton);

	connect(fluorescenceButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onFluorescenceChoiceChanged(int)));
	connect(config_, SIGNAL(fluorescenceDetectorChanged(int)), this, SLOT(updateFluorescenceChoiceButtons(int)));

	fluorescenceButtonGroup_->button((int)config_->fluorescenceDetector())->setChecked(true);

	QGroupBox *fluorescenceDetectorGroupBox = new QGroupBox("Fluorescence Detector");
	fluorescenceDetectorGroupBox->setLayout(fluorescenceDetectorLayout);

	// Ion chamber selection
	QVBoxLayout *ItGroupLayout = new QVBoxLayout;

	ItGroup_ = new QButtonGroup;
	tempButton = new QRadioButton("Isplit");
	tempButton->setEnabled(false);
	ItGroup_->addButton(tempButton, 0);
	ItGroupLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Iprekb");
	ItGroup_->addButton(tempButton, 1);
	ItGroupLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Imini");
	ItGroup_->addButton(tempButton, 2);
	ItGroupLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Ipost");
	ItGroup_->addButton(tempButton, 3);
	ItGroupLayout->addWidget(tempButton);
	connect(ItGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onItClicked(int)));
	connect(config_, SIGNAL(transmissionChoiceChanged(int)), this, SLOT(updateItButtons(int)));

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
	connect(config_, SIGNAL(incomingChoiceChanged(int)), this, SLOT(updateI0Buttons(int)));

	I0Group_->button((int)config_->incomingChoice())->click();
	ItGroup_->button((int)config_->transmissionChoice())->click();

	QGroupBox *I0GroupBox = new QGroupBox("I0");
	I0GroupBox->setLayout(I0GroupLayout);

	QGroupBox *ItGroupBox = new QGroupBox("It");
	ItGroupBox->setLayout(ItGroupLayout);

	QHBoxLayout *ionChambersLayout = new QHBoxLayout;
	ionChambersLayout->addWidget(I0GroupBox);
	ionChambersLayout->addWidget(ItGroupBox);

	// Scan name selection
	scanName_ = new QLineEdit;
	scanName_->setText(config_->name());
	scanName_->setAlignment(Qt::AlignCenter);
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
	QCheckBox *goToPosition = new QCheckBox("Choose Position");
	goToPosition->setChecked(config_->goToPosition());

	QPushButton *setCurrentPosition = new QPushButton(QIcon(":/save.png"), "");
	setCurrentPosition->setEnabled(goToPosition->isChecked());
	connect(setCurrentPosition, SIGNAL(clicked()), this, SLOT(setScanPosition()));

	savedXPosition_ = new QLabel(QString::number(config_->goToPosition() ? config_->x() : 0, 'g', 3) + " mm");
	savedXPosition_->setEnabled(goToPosition->isChecked());
	savedYPosition_ = new QLabel(QString::number(config_->goToPosition() ? config_->y() : 0, 'g', 3) + " mm");
	savedYPosition_->setEnabled(goToPosition->isChecked());

	positionsSaved_ = new QLabel;
	QPalette palette(this->palette());
	palette.setColor(QPalette::Disabled, QPalette::WindowText, Qt::darkGray);

	// Although not entirely valid, the chances that both x and y being identically 0 for a saved position is incredibly unlikely.
	if (!config_->goToPosition() || (config_->x() == 0.0 && config_->y() == 0.0)){

		positionsSaved_->setText("Unsaved");
		palette.setColor(QPalette::Active, QPalette::WindowText, Qt::red);
	}
	else{

		positionsSaved_->setText("Saved");
		palette.setColor(QPalette::Active, QPalette::WindowText, Qt::darkGreen);
	}

	positionsSaved_->setPalette(palette);

	QFont font(this->font());
	font.setBold(true);
	positionsSaved_->setFont(font);
	positionsSaved_->setEnabled(goToPosition->isChecked());

	QHBoxLayout *saveLayout = new QHBoxLayout;
	saveLayout->addWidget(setCurrentPosition);
	saveLayout->addWidget(positionsSaved_);

	xPosition_ = new QDoubleSpinBox;
	xPosition_->setEnabled(goToPosition->isChecked());
	xPosition_->setDecimals(3);
	xPosition_->setRange(-100, 100);
	xPosition_->setValue(config_->goToPosition() ? config_->x() : 0);
	xPosition_->setSuffix(" mm");
	connect(xPosition_, SIGNAL(valueChanged(double)), this, SLOT(onXorYPositionChanged()));
	connect(config_, SIGNAL(xPositionChanged(double)), xPosition_, SLOT(setValue(double)));

	QHBoxLayout *xLayout = new QHBoxLayout;
	xLayout->addWidget(xPosition_);
	xLayout->addWidget(savedXPosition_);

	yPosition_ = new QDoubleSpinBox;
	yPosition_->setEnabled(goToPosition->isChecked());
	yPosition_->setDecimals(3);
	yPosition_->setRange(-100, 100);
	yPosition_->setValue(config_->goToPosition() ? config_->y() : 0);
	yPosition_->setSuffix(" mm");
	connect(yPosition_, SIGNAL(valueChanged(double)), this, SLOT(onXorYPositionChanged()));
	connect(config_, SIGNAL(yPositionChanged(double)), yPosition_, SLOT(setValue(double)));

	setSampleStage(VESPERSBeamline::vespers()->experimentConfiguration()->sampleStageChoice());

	QHBoxLayout *yLayout = new QHBoxLayout;
	yLayout->addWidget(yPosition_);
	yLayout->addWidget(savedYPosition_);

	connect(config_, SIGNAL(gotoPositionChanged(bool)), goToPosition, SLOT(setChecked(bool)));
	connect(goToPosition, SIGNAL(toggled(bool)), config_, SLOT(setGoToPosition(bool)));
	connect(goToPosition, SIGNAL(toggled(bool)), setCurrentPosition, SLOT(setEnabled(bool)));
	connect(goToPosition, SIGNAL(toggled(bool)), xPosition_, SLOT(setEnabled(bool)));
	connect(goToPosition, SIGNAL(toggled(bool)), yPosition_, SLOT(setEnabled(bool)));
	connect(goToPosition, SIGNAL(toggled(bool)), savedXPosition_, SLOT(setEnabled(bool)));
	connect(goToPosition, SIGNAL(toggled(bool)), savedYPosition_, SLOT(setEnabled(bool)));
	connect(goToPosition, SIGNAL(toggled(bool)), positionsSaved_, SLOT(setEnabled(bool)));

	QFormLayout *positionLayout = new QFormLayout;
	positionLayout->addRow(goToPosition);
	positionLayout->addRow(saveLayout);
	positionLayout->addRow("x:", xLayout);
	positionLayout->addRow("y:", yLayout);

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

	if (config_->fluorescenceDetector() == VESPERS::NoXRF)
		roiTextBox_->hide();

	else
		roiTextBox_->show();

	// Label showing where the data will be saved.
	QString exportString =  AMUserSettings::userDataFolder;
	exportString.remove("/userData");
	QLabel *exportPath = new QLabel(QString("Data exported to: %1exportData").arg(exportString));

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

	timeOffsetLabel_ = new QLabel("Scan time offset:");
	timeOffsetLabel_->hide();

	timeOffset_ = new QDoubleSpinBox;
	timeOffset_->hide();
	timeOffset_->setRange(0, 100);
	timeOffset_->setDecimals(2);
	timeOffset_->setSingleStep(0.1);
	timeOffset_->setSuffix(" s");
	timeOffset_->setAlignment(Qt::AlignCenter);
	timeOffset_->setValue(config_->timeOffset());
	connect(timeOffset_, SIGNAL(valueChanged(double)), config_, SLOT(setTimeOffset(double)));

	QHBoxLayout *timeOffsetLayout = new QHBoxLayout;
	timeOffsetLayout->addWidget(timeOffsetLabel_);
	timeOffsetLayout->addWidget(timeOffset_);

	QVBoxLayout *defaultLayout = new QVBoxLayout;
	defaultLayout->addSpacing(35);
	defaultLayout->addWidget(defaultXANESButton);
	defaultLayout->addWidget(defaultEXAFSButton);
	defaultLayout->addStretch();

	// Auto-export option.
	QVBoxLayout *autoExportLayout = new QVBoxLayout;
	QCheckBox *autoExportSpectra = new QCheckBox("Export spectra");
	autoExportSpectra->setChecked(config_->exportSpectraSources());
	autoExportLayout->addWidget(autoExportSpectra);
	connect(autoExportSpectra, SIGNAL(toggled(bool)), config_, SLOT(setExportSpectraSources(bool)));

	QGroupBox *autoExportGroupBox = new QGroupBox("Export Options");
	autoExportGroupBox->setLayout(autoExportLayout);

	// Setting up the layout.
	QGridLayout *contentsLayout = new QGridLayout;
	contentsLayout->addWidget(regionsView_, 1, 1, 2, 2);
	contentsLayout->addWidget(fluorescenceDetectorGroupBox, 1, 3);
	contentsLayout->addLayout(scanNameLayout, 4, 1);
	contentsLayout->addLayout(energyLayout, 0, 1, 1, 3);
	contentsLayout->addLayout(positionLayout, 4, 3, 4, 1);
	contentsLayout->addLayout(ionChambersLayout, 2, 3, 2, 1);
	contentsLayout->addWidget(roiTextBox_, 1, 4, 2, 2);
	contentsLayout->addWidget(useFixedTime, 3, 1);
	contentsLayout->addWidget(estimatedTime_, 6, 1, 1, 2);
	contentsLayout->addWidget(estimatedSetTime_, 7, 1, 1, 2);
	contentsLayout->addLayout(numberOfScansLayout, 5, 1);
	contentsLayout->addLayout(timeOffsetLayout, 8, 1, 1, 2);
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

void VESPERSEXAFSScanConfigurationView::updateFluorescenceChoiceButtons(int detector)
{
	fluorescenceButtonGroup_->button(detector)->setChecked(true);
}

void VESPERSEXAFSScanConfigurationView::onConfigureXRFDetectorClicked()
{
	switch((int)config_->fluorescenceDetector()){

	case VESPERS::NoXRF:
		break;

	case VESPERS::SingleElement:
		emit configureDetector("Single Element");
		break;

	case VESPERS::FourElement:
		emit configureDetector("Four Element");
		break;

	case VESPERS::SingleElement | VESPERS::FourElement:

		QMenu menu(this);
		menu.addAction("Single Element");
		menu.addAction("Four Element");
		QAction *action = menu.exec(QCursor::pos());

		if (action && (action->text() == "Single Element" || action->text() == "Four Element"))
			emit configureDetector(action->text());

		break;
	}
}

void VESPERSEXAFSScanConfigurationView::updateItButtons(int It)
{
	ItGroup_->button(It)->setChecked(true);
}

void VESPERSEXAFSScanConfigurationView::updateI0Buttons(int I0)
{
	I0Group_->button(I0)->setChecked(true);
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

	roiText_->clear();

	if ((int)config_->fluorescenceDetector() ==  (VESPERS::SingleElement | VESPERS::FourElement)){

		QList<QPair<int, int> > sameList = findRoiPairs();

		AMROIInfoList singleElList = *VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList();
		AMROIInfoList fourElList = *VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList();

		if (!sameList.isEmpty()){

			QList<int> singleRoiList;
			QList<int> fourRoiList;

			roiText_->insertPlainText("Same ROI's\n");
			roiText_->insertPlainText("Name\tLow (eV)\tHigh (eV)\n");

			for (int i = 0, count = sameList.size(); i < count; i++){

				QPair<int, int> temp = sameList.at(i);
				singleRoiList << temp.first;
				fourRoiList << temp.second;
				AMROIInfo info = singleElList.at(temp.first);
				roiText_->insertPlainText(GeneralUtilities::addGreek(info.name())+"\t" + QString::number(info.low()) + "\t" + QString::number(info.high()) +"\n");
			}

			if (singleRoiList.size() < singleElList.count() || fourRoiList.size() < fourElList.count()){

				roiText_->insertPlainText("\nDifferent ROI's\n");

				if (singleRoiList.size() < singleElList.count()){

					roiText_->insertPlainText("Single Element Vortex\n");
					roiText_->insertPlainText("Name\tLow (eV)\tHigh (eV)\n");

					for (int i = 0, count = singleElList.count(); i < count; i++){

						if (!singleRoiList.contains(i)){

							AMROIInfo info = singleElList.at(i);
							roiText_->insertPlainText(GeneralUtilities::addGreek(info.name())+"\t" + QString::number(info.low()) + "\t" + QString::number(info.high()) +"\n");
						}
					}

					roiText_->insertPlainText("\n");
				}

				if (fourRoiList.size() < fourElList.count()){

					roiText_->insertPlainText("Four Element Vortex\n");
					roiText_->insertPlainText("Name\tLow (eV)\tHigh (eV)\n");

					for (int i = 0, count = fourElList.count(); i < count; i++){

						if (!fourRoiList.contains(i)){

							AMROIInfo info = fourElList.at(i);
							roiText_->insertPlainText(GeneralUtilities::addGreek(info.name())+"\t" + QString::number(info.low()) + "\t" + QString::number(info.high()) +"\n");
						}
					}
				}
			}
		}

		else {

			roiText_->insertPlainText("Different ROI's\n");
			roiText_->insertPlainText("Single Element Vortex\n");

			for (int i = 0, count = singleElList.count(); i < count; i++){

				AMROIInfo info = singleElList.at(i);
				roiText_->insertPlainText(GeneralUtilities::addGreek(info.name())+"\t" + QString::number(info.low()) + "\t" + QString::number(info.high()) +"\n");
			}

			roiText_->insertPlainText("\nFour Element Vortex\n");

			for (int i = 0, count = fourElList.count(); i < count; i++){

				AMROIInfo info = fourElList.at(i);
				roiText_->insertPlainText(GeneralUtilities::addGreek(info.name())+"\t" + QString::number(info.low()) + "\t" + QString::number(info.high()) +"\n");
			}
		}
	}

	else {

		roiText_->insertPlainText("Name\tLow (eV)\tHigh (eV)\n");

		for (int i = 0; i < config_->roiList().count(); i++)
			roiText_->insertPlainText(GeneralUtilities::addGreek(config_->roiList().at(i).name())+"\t" + QString::number(config_->roiList().at(i).low()) + "\t" + QString::number(config_->roiList().at(i).high()) +"\n");
	}
}

QList<QPair<int, int> > VESPERSEXAFSScanConfigurationView::findRoiPairs() const
{
	AMROIInfoList *el1 = VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList();
	AMROIInfoList *el4 = VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList();
	QList<QPair<int, int> > list;

	// Do it the easy way first.  Only possible when the sizes are the same.
	if (el1->count() == el4->count()){

		bool allLinedUp = true;

		for (int i = 0, count = el1->count(); i < count; i++)
			if (el1->at(i).name() != el4->at(i).name())
				allLinedUp = false;

		// If true, this is really straight forward.
		if (allLinedUp){

			for (int i = 0, count = el1->count(); i < count; i++)
				list << qMakePair(i, i);
		}

		// Otherwise, we have to check each individually.  Not all may match and only matches will be added to the list.
		else {

			for (int i = 0, count = el1->count(); i < count; i++)
				for (int j = 0; j < count; j++)
					if (el1->at(i).name() == el4->at(j).name())
						list << qMakePair(i, j);
		}
	}

	// This is the same the above double for-loop but with different boundaries.
	else {

		for (int i = 0, count1 = el1->count(); i < count1; i++)
			for (int j = 0, count4 = el4->count(); j < count4; j++)
				if (el1->at(i).name() == el4->at(j).name())
					list << qMakePair(i, j);
	}

	return list;
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
	estimatedTime_->setText("Estimated time per scan:\t" + convertTimeToString(config_->totalTime()));
	estimatedSetTime_->setText("Estimated time for set:\t" + convertTimeToString(config_->totalTime()*config_->numberOfScans()));
}

QString VESPERSEXAFSScanConfigurationView::convertTimeToString(double time)
{
	QString timeString;

	int days = int(time/3600.0/24.0);

	if (days > 0){

		time -= days*3600*24;
		timeString += QString::number(days) + "d:";
	}

	int hours = int(time/3600.0);

	if (hours > 0){

		time -= hours*3600;
		timeString += QString::number(hours) + "h:";
	}

	int minutes = int(time/60.0);

	if (minutes > 0){

		time -= minutes*60;
		timeString += QString::number(minutes) + "m:";
	}

	int seconds = ((int)time)%60;
	timeString += QString::number(seconds) + "s";

	return timeString;
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

void VESPERSEXAFSScanConfigurationView::onCustomContextMenuRequested(QPoint pos)
{
	QMenu popup(this);

	QAction *temp = popup.addAction("Set time offset");
	temp = popup.exec(mapToGlobal(pos));

	// If a valid action was selected.
	if (temp && (temp->text() == "Set time offset")){

		timeOffsetLabel_->setVisible(!timeOffsetLabel_->isVisible());
		timeOffset_->setVisible(!timeOffset_->isVisible());
	}
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
