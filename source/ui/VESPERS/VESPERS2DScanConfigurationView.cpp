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


#include "VESPERS2DScanConfigurationView.h"
#include "ui/AMTopFrame.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "util/VESPERS/GeneralUtilities.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QPushButton>
#include <QSpinBox>
#include <QMenu>

VESPERS2DScanConfigurationView::VESPERS2DScanConfigurationView(VESPERS2DScanConfiguration *config, QWidget *parent)
	: VESPERSScanConfigurationView(parent)
{
	config_ = config;
	AMTopFrame *frame = new AMTopFrame("VESPERS 2D Map Configuration");

	// Setup the group box for setting the start and end points.
	QGroupBox *positionsBox = new QGroupBox("Positions");

	hStart_ = buildPositionDoubleSpinBox("H: ", " mm", config_->xStart(), 3);
	connect(hStart_, SIGNAL(editingFinished()), this, SLOT(onXStartChanged()));
	connect(config_, SIGNAL(xStartChanged(double)), hStart_, SLOT(setValue(double)));

	vStart_ = buildPositionDoubleSpinBox("V: ", " mm", config_->yStart(), 3);
	connect(vStart_, SIGNAL(editingFinished()), this, SLOT(onYStartChanged()));
	connect(config_, SIGNAL(yStartChanged(double)), vStart_, SLOT(setValue(double)));

	QPushButton *startUseCurrentButton = new QPushButton("Use Current");
	connect(startUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetStartPosition()));

	QHBoxLayout *startPointLayout = new QHBoxLayout;
	startPointLayout->addWidget(new QLabel("Start:"));
	startPointLayout->addWidget(hStart_);
	startPointLayout->addWidget(vStart_);
	startPointLayout->addWidget(startUseCurrentButton);

	hEnd_ = buildPositionDoubleSpinBox("H: ", " mm", config_->xEnd(), 3);
	connect(hEnd_, SIGNAL(editingFinished()), this, SLOT(onXEndChanged()));
	connect(config_, SIGNAL(xEndChanged(double)), hEnd_, SLOT(setValue(double)));

	vEnd_ = buildPositionDoubleSpinBox("V: ", " mm", config_->yEnd(), 3);
	connect(vEnd_, SIGNAL(editingFinished()), this, SLOT(onYEndChanged()));
	connect(config_, SIGNAL(yEndChanged(double)), vEnd_, SLOT(setValue(double)));

	QPushButton *endUseCurrentButton = new QPushButton("Use Current");
	connect(endUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetEndPosition()));

	QHBoxLayout *endPointLayout = new QHBoxLayout;
	endPointLayout->addWidget(new QLabel("End:"));
	endPointLayout->addWidget(hEnd_);
	endPointLayout->addWidget(vEnd_);
	endPointLayout->addWidget(endUseCurrentButton);

	hStep_ = buildPositionDoubleSpinBox("H: ", QString(" %1").arg(QString::fromUtf8("µm")), config_->xStep()*1000, 1);	// xStep needs to be in mm.
	connect(hStep_, SIGNAL(editingFinished()), this, SLOT(onXStepChanged()));
	connect(config_, SIGNAL(xStepChanged(double)), this, SLOT(updateXStep(double)));

	vStep_ = buildPositionDoubleSpinBox("V: ", QString(" %1").arg(QString::fromUtf8("µm")), config_->yStep()*1000, 1);	// yStep needs to be in mm.
	connect(vStep_, SIGNAL(editingFinished()), this, SLOT(onYStepChanged()));
	connect(config_, SIGNAL(yStepChanged(double)), this, SLOT(updateYStep(double)));

	QHBoxLayout *stepSizeLayout = new QHBoxLayout;
	stepSizeLayout->addWidget(new QLabel("Step Size:"));
	stepSizeLayout->addWidget(hStep_);
	stepSizeLayout->addWidget(vStep_);
	stepSizeLayout->addStretch();

	mapInfo_ = new QLabel;
	updateMapInfo();

	QVBoxLayout *positionsLayout = new QVBoxLayout;
	positionsLayout->addLayout(startPointLayout);
	positionsLayout->addLayout(endPointLayout);
	positionsLayout->addLayout(stepSizeLayout);
	positionsLayout->addWidget(mapInfo_);

	positionsBox->setLayout(positionsLayout);

	// Dwell time.
	dwellTime_ = addDwellTimeWidget(config_->timeStep());
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));
	connect(config_, SIGNAL(timeStepChanged(double)), dwellTime_, SLOT(setValue(double)));

	QHBoxLayout *timeLayout = new QHBoxLayout;
	timeLayout->addWidget(new QLabel("Dwell Time:"));
	timeLayout->addWidget(dwellTime_);

	// Using the CCD.
	QGroupBox *ccdBox = new QGroupBox("XRD maps");

	ccdCheckBox_ = new QCheckBox("Do XRD simultaneously");
	ccdCheckBox_->setChecked(config_->ccdDetector() == VESPERS::Roper ? true : false);
	connect(config_->dbObject(), SIGNAL(ccdDetectorChanged(int)), this, SLOT(onCCDDetectorChanged(int)));
	connect(ccdCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onCCDButtonClicked(bool)));

	currentCCDFileName_ = new QLabel;
	onCCDFileNameChanged(config_->ccdFileName());
	currentCCDFileName_->setVisible(config_->ccdDetector() == VESPERS::Roper ? true : false);
	connect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(ccdNameChanged(QString)), this, SLOT(onCCDFileNameChanged(QString)));


	QPushButton *configureRoperDetectorButton = new QPushButton(QIcon(":/hammer-wrench.png"), "Configure Roper CCD");
	configureRoperDetectorButton->setEnabled(config_->ccdDetector());
	connect(configureRoperDetectorButton, SIGNAL(clicked()), this, SLOT(onConfigureRoperDetectorClicked()));
	connect(ccdCheckBox_, SIGNAL(toggled(bool)), configureRoperDetectorButton, SLOT(setEnabled(bool)));

	QHBoxLayout *ccdBoxFirstRowLayout = new QHBoxLayout;
	ccdBoxFirstRowLayout->addWidget(ccdCheckBox_);
	ccdBoxFirstRowLayout->addWidget(configureRoperDetectorButton);

	QVBoxLayout *ccdBoxLayout = new QVBoxLayout;
	ccdBoxLayout->addLayout(ccdBoxFirstRowLayout);
	ccdBoxLayout->addWidget(currentCCDFileName_);

	ccdBox->setLayout(ccdBoxLayout);

	// The fluorescence detector setup
	QGroupBox *fluorescenceDetectorGroupBox  = addFluorescenceDetectorSelectionView();
	connect(fluorescenceButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onFluorescenceChoiceChanged(int)));
	connect(config_->dbObject(), SIGNAL(fluorescenceDetectorChanged(int)), this, SLOT(updateFluorescenceDetector(int)));

	fluorescenceButtonGroup_->button((int)config_->fluorescenceDetector())->setChecked(true);

	// Ion chamber selection
	QGroupBox *I0GroupBox = addI0SelectionView();
	connect(I0Group_, SIGNAL(buttonClicked(int)), this, SLOT(onI0Clicked(int)));
	connect(config_->dbObject(), SIGNAL(incomingChoiceChanged(int)), this, SLOT(updateI0Buttons(int)));

	I0Group_->button((int)config_->incomingChoice())->click();

	// Motor selection.
	QRadioButton *tempButton;
	QGroupBox *motorSetChoiceBox = new QGroupBox("Sample Stage");
	QVBoxLayout *motorChoiceLayout = new QVBoxLayout;
	motorButtonGroup_ = new QButtonGroup;

	tempButton = new QRadioButton("H and V");
	motorButtonGroup_->addButton(tempButton, VESPERS::H | VESPERS::V);
	motorChoiceLayout->addWidget(tempButton);
	tempButton = new QRadioButton("X and Z");
	motorButtonGroup_->addButton(tempButton, VESPERS::X | VESPERS::Z);
	motorChoiceLayout->addWidget(tempButton);

	connect(motorButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onMotorChanged(int)));

	motorButtonGroup_->button(int(config_->motor()))->click();
	motorSetChoiceBox->setLayout(motorChoiceLayout);

	// Scan name selection
	scanName_ = addScanNameView(config_->name());
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(config_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
	onScanNameEdited();

	QFormLayout *scanNameLayout = new QFormLayout;
	scanNameLayout->addRow("Scan Name:", scanName_);

	// The estimated scan time.
	estimatedTime_ = new QLabel;
	connect(config_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	onEstimatedTimeChanged();

	// The roi text edit and configuration.
	roiText_ = new QTextEdit;
	roiText_->setReadOnly(true);

	QPushButton *configureXRFDetectorButton = new QPushButton(QIcon(":/hammer-wrench.png"), "Configure XRF Detector");
	connect(configureXRFDetectorButton, SIGNAL(clicked()), this, SLOT(onConfigureXRFDetectorClicked()));

	QFormLayout *roiTextLayout = new QFormLayout;
	roiTextLayout->addRow(roiText_);
	roiTextLayout->addRow(configureXRFDetectorButton);

	QGroupBox *roiTextBox = new QGroupBox("Regions Of Interest");
	roiTextBox->setLayout(roiTextLayout);

	// Label showing where the data will be saved.
	QLabel *exportPath = addExportPathLabel();

	// Setting up the steps to show the time offset for scan time estimation.
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
	setContextMenuPolicy(Qt::CustomContextMenu);

	QGroupBox *timeOffsetBox = addTimeOffsetLabel(config_->timeOffset());
	connect(timeOffset_, SIGNAL(valueChanged(double)), this, SLOT(setTimeOffset(double)));

	// Auto-export option.
	QVBoxLayout *autoExportLayout = new QVBoxLayout;
	QButtonGroup *autoExportButtonGroup_ = new QButtonGroup;

	QRadioButton *autoExportButton = new QRadioButton("Ascii");
	autoExportButtonGroup_->addButton(autoExportButton, 0);
	autoExportLayout->addWidget(autoExportButton);

	autoExportButton = new QRadioButton("SMAK");
	autoExportButtonGroup_->addButton(autoExportButton, 1);
	autoExportLayout->addWidget(autoExportButton);

	connect(autoExportButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(updateAutoExporter(int)));

	autoExportButtonGroup_->button(config_->exportAsAscii() ? 0 : 1)->click();

	QCheckBox *autoExportSpectra = new QCheckBox("Export Spectra");
	autoExportSpectra->setChecked(config_->exportSpectraSources());
	autoExportLayout->addWidget(autoExportSpectra);
	connect(autoExportSpectra, SIGNAL(toggled(bool)), config_, SLOT(setExportSpectraSources(bool)));

	QGroupBox *autoExportGroupBox = new QGroupBox("Export Options");
	autoExportGroupBox->setLayout(autoExportLayout);

	// Setting up the layout.
	QGridLayout *contentsLayout = new QGridLayout;
	contentsLayout->addWidget(positionsBox, 0, 0, 1, 3);
	contentsLayout->addLayout(timeLayout, 1, 0, 1, 1);
	contentsLayout->addWidget(ccdBox, 2, 0, 1, 1);
	contentsLayout->addWidget(fluorescenceDetectorGroupBox, 1, 3, 1, 1);
	contentsLayout->addLayout(scanNameLayout, 3, 0, 1, 1);
	contentsLayout->addWidget(I0GroupBox, 2, 3, 4, 1);
	contentsLayout->addWidget(roiTextBox, 0, 5, 3, 3);
	contentsLayout->addWidget(estimatedTime_, 4, 0, 1, 1);
	contentsLayout->addWidget(timeOffsetBox, 5, 0, 1, 1);
	contentsLayout->addWidget(motorSetChoiceBox, 0, 3);
	contentsLayout->addWidget(autoExportGroupBox, 4, 5, 2, 3);

	QHBoxLayout *squeezeContents = new QHBoxLayout;
	squeezeContents->addStretch();
	squeezeContents->addLayout(contentsLayout);
	squeezeContents->addStretch();

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addWidget(frame);
	configViewLayout->addStretch();
	configViewLayout->addLayout(squeezeContents);
	configViewLayout->addStretch();
	configViewLayout->addWidget(exportPath, 0, Qt::AlignCenter);
	configViewLayout->addSpacing(30);

	setLayout(configViewLayout);
}

void VESPERS2DScanConfigurationView::onFluorescenceChoiceChanged(int id)
{
	config_->setFluorescenceDetector(id);
	updateRoiText();
}

void VESPERS2DScanConfigurationView::onMotorChanged(int id)
{
	config_->setMotor(id);
}

void VESPERS2DScanConfigurationView::onConfigureXRFDetectorClicked()
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

void VESPERS2DScanConfigurationView::onConfigureRoperDetectorClicked()
{
	emit configureDetector("Roper CCD");
}

void VESPERS2DScanConfigurationView::onCCDDetectorChanged(int useCCD)
{
	if (useCCD > 0){

		ccdCheckBox_->setChecked(true);
		connect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(ccdNameChanged(QString)), config_->dbObject(), SLOT(setCCDFileName(QString)));
		config_->setCCDFileName(VESPERSBeamline::vespers()->roperCCD()->ccdFileName());
		onCCDFileNameChanged(VESPERSBeamline::vespers()->roperCCD()->ccdFileName());
		currentCCDFileName_->show();
	}
	else {

		ccdCheckBox_->setChecked(false);
		disconnect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(ccdNameChanged(QString)), config_->dbObject(), SLOT(setCCDFileName(QString)));
		config_->setCCDFileName("");
		onCCDFileNameChanged("");
		currentCCDFileName_->hide();
	}
}

void VESPERS2DScanConfigurationView::updateRoiText()
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

void VESPERS2DScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + VESPERS::convertTimeToString(config_->totalTime()));
}

void VESPERS2DScanConfigurationView::onCustomContextMenuRequested(QPoint pos)
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

void VESPERS2DScanConfigurationView::onSetStartPosition()
{
	double h = 0;
	double v = 0;

	if (config_->motor() == (VESPERS::H | VESPERS::V)){

		h = VESPERSBeamline::vespers()->pseudoSampleStage()->horiz()->value();
		v = VESPERSBeamline::vespers()->pseudoSampleStage()->vert()->value();
	}
	else if (config_->motor() == (VESPERS::X | VESPERS::Z)){

		h = VESPERSBeamline::vespers()->sampleStageX()->value();
		v = VESPERSBeamline::vespers()->sampleStageZ()->value();
	}

	config_->setXStart(h);
	hStart_->setValue(h);
	config_->setYStart(v);
	vStart_->setValue(v);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS2DScanConfigurationView::onSetEndPosition()
{
	double h = 0;
	double v = 0;

	if (config_->motor() == (VESPERS::H | VESPERS::V)){

		h = VESPERSBeamline::vespers()->pseudoSampleStage()->horiz()->value();
		v = VESPERSBeamline::vespers()->pseudoSampleStage()->vert()->value();
	}
	else if (config_->motor() == (VESPERS::X | VESPERS::Z)){

		h = VESPERSBeamline::vespers()->sampleStageX()->value();
		v = VESPERSBeamline::vespers()->sampleStageZ()->value();
	}

	config_->setXEnd(h);
	hEnd_->setValue(h);
	config_->setYEnd(v);
	vEnd_->setValue(v);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS2DScanConfigurationView::onXStartChanged()
{
	config_->setXStart(hStart_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS2DScanConfigurationView::onXEndChanged()
{
	config_->setXEnd(hEnd_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS2DScanConfigurationView::onXStepChanged()
{
	config_->setXStep(hStep_->value()/1000);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS2DScanConfigurationView::onYStartChanged()
{
	config_->setYStart(vStart_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS2DScanConfigurationView::onYEndChanged()
{
	config_->setYEnd(vEnd_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS2DScanConfigurationView::onYStepChanged()
{
	config_->setYStep(vStep_->value()/1000);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS2DScanConfigurationView::onDwellTimeChanged()
{
	config_->setTimeStep(dwellTime_->value());
}

void VESPERS2DScanConfigurationView::updateMapInfo()
{
	double hSize = fabs(config_->xEnd()-config_->xStart());
	double vSize = fabs(config_->yEnd()-config_->yStart());

	int hPoints = int((hSize)/config_->yStep());
	if ((hSize - (hPoints + 0.01)*config_->yStep()) < 0)
		hPoints += 1;
	else
		hPoints += 2;

	int vPoints = int((vSize)/config_->yStep());
	if ((vSize - (vPoints + 0.01)*config_->yStep()) < 0)
		vPoints += 1;
	else
		vPoints += 2;

	mapInfo_->setText(QString("Map Size: %1 %2 x %3 %2\t Points: %4 x %5")
					  .arg(QString::number(hSize*1000, 'f', 1))
					  .arg(QString::fromUtf8("µm"))
					  .arg(QString::number(vSize*1000, 'f', 1))
					  .arg(hPoints)
					  .arg(vPoints)
					  );
}

void VESPERS2DScanConfigurationView::axesAcceptable()
{
	QPalette good(this->palette());
	QPalette bad(good);
	bad.setColor(QPalette::Base, Qt::red);

	hStep_->setPalette(config_->validXAxis() ? good : bad);
	vStep_->setPalette(config_->validYAxis() ? good : bad);
}
