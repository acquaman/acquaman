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


#include "VESPERSSpatialLineScanConfigurationView.h"

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

VESPERSSpatialLineScanConfigurationView::VESPERSSpatialLineScanConfigurationView(VESPERSSpatialLineScanConfiguration *config, QWidget *parent)
	: VESPERSScanConfigurationView(parent)
{
	config_ = config;
	AMTopFrame *frame = new AMTopFrame("VESPERS Line Scan Configuration");

	// Setup the group box for setting the start and end points.
	QGroupBox *positionsBox = new QGroupBox("Positions");

	start_ = buildPositionDoubleSpinBox("", " mm", config_->start(), 3);
	connect(start_, SIGNAL(editingFinished()), this, SLOT(onStartChanged()));
	connect(config_, SIGNAL(startChanged(double)), start_, SLOT(setValue(double)));

	QPushButton *startUseCurrentButton = new QPushButton("Use Current");
	connect(startUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetStartPosition()));

	QHBoxLayout *startPointLayout = new QHBoxLayout;
	startPointLayout->addWidget(new QLabel("Start:"));
	startPointLayout->addWidget(start_);
	startPointLayout->addWidget(startUseCurrentButton);

	end_ = buildPositionDoubleSpinBox("", " mm", config_->end(), 3);
	connect(end_, SIGNAL(editingFinished()), this, SLOT(onEndChanged()));
	connect(config_, SIGNAL(endChanged(double)), end_, SLOT(setValue(double)));

	QPushButton *endUseCurrentButton = new QPushButton("Use Current");
	connect(endUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetEndPosition()));

	QHBoxLayout *endPointLayout = new QHBoxLayout;
	endPointLayout->addWidget(new QLabel("End:"));
	endPointLayout->addWidget(end_);
	endPointLayout->addWidget(endUseCurrentButton);

	step_ = buildPositionDoubleSpinBox("", QString(" %1").arg(QString::fromUtf8("µm")), config_->step()*1000, 1);	// xStep needs to be in mm.
	connect(step_, SIGNAL(editingFinished()), this, SLOT(onStepChanged()));
	connect(config_, SIGNAL(stepChanged(double)), this, SLOT(updateStep(double)));

	QHBoxLayout *stepSizeLayout = new QHBoxLayout;
	stepSizeLayout->addWidget(new QLabel("Step Size:"));
	stepSizeLayout->addWidget(step_);
	stepSizeLayout->addStretch();

	otherPositionLabel_ = new QLabel(config_->otherMotorString(config_->motor()));
	otherPosition_ = buildPositionDoubleSpinBox("", " mm", config_->hasOtherPosition() ? config_->otherPosition() : 0, 3);
	connect(otherPosition_, SIGNAL(editingFinished()), this, SLOT(onOtherPositionChanged()));
	connect(config_, SIGNAL(otherPositionChanged(double)), otherPosition_, SLOT(setValue(double)));

	QHBoxLayout *otherPositionLayout = new QHBoxLayout;
	otherPositionLayout->addWidget(otherPositionLabel_);
	otherPositionLayout->addWidget(otherPosition_);
	otherPositionLayout->addStretch();

	mapInfo_ = new QLabel;
	updateMapInfo();

	QVBoxLayout *positionsLayout = new QVBoxLayout;
	positionsLayout->addLayout(startPointLayout);
	positionsLayout->addLayout(endPointLayout);
	positionsLayout->addLayout(stepSizeLayout);
	positionsLayout->addLayout(otherPositionLayout);
	positionsLayout->addWidget(mapInfo_);

	positionsBox->setLayout(positionsLayout);

	// Dwell time.
	dwellTime_ = addDwellTimeWidget(config_->time());
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));
	connect(config_, SIGNAL(timeChanged(double)), dwellTime_, SLOT(setValue(double)));

	QHBoxLayout *timeLayout = new QHBoxLayout;
	timeLayout->addWidget(new QLabel("Dwell Time:"));
	timeLayout->addWidget(dwellTime_);

	// The estimated scan time.
	estimatedTime_ = new QLabel;
	connect(config_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	onEstimatedTimeChanged();

	QVBoxLayout *timeBoxLayout = new QVBoxLayout;
	timeBoxLayout->addLayout(timeLayout);
	timeBoxLayout->addWidget(estimatedTime_);

	QGroupBox *timeGroupBox = new QGroupBox("Time");
	timeGroupBox->setLayout(timeBoxLayout);

	// Using the CCD.
	QGroupBox *ccdBox = new QGroupBox("XRD maps");

	ccdCheckBox_ = new QCheckBox("Do XRD simultaneously");
	ccdCheckBox_->setChecked(config_->ccdDetector() == VESPERS::Roper ? true : false);
	connect(config_->dbObject(), SIGNAL(ccdDetectorChanged(int)), this, SLOT(onCCDDetectorChanged(int)));
	connect(ccdCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onCCDButtonClicked(bool)));

	currentCCDFileName_ = new QLabel;
	onCCDFileNameChanged(config_->ccdFileName());
	currentCCDFileName_->setVisible(config_->ccdDetector());
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
	connect(fluorescenceButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onFluorescenceDetectorChanged(int)));
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

	// Motor selection.
	QGroupBox *motorSetChoiceBox = addMotorSelectionView(QStringList() << "H" << "X" << "V" << "Z", QList<int>() << VESPERS::H << VESPERS::X << VESPERS::V << VESPERS::Z);
	motorButtonGroup_->button(int(config_->motor()))->click();
	connect(motorButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onMotorChanged(int)));

	// Scan name selection
	scanName_ = addScanNameView(config_->name());
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(config_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
	onScanNameEdited();

	QFormLayout *scanNameLayout = new QFormLayout;
	scanNameLayout->addRow("Scan Name:", scanName_);

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

	QGroupBox *timeOffsetBox = addTimeOffsetLabel(config_->timeOffset());
	connect(timeOffset_, SIGNAL(valueChanged(double)), this, SLOT(setTimeOffset(double)));

	// Auto-export option.
	QGroupBox *autoExportGroupBox = addExporterOptionsView(QStringList(), config_->exportSpectraSources());
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), config_, SLOT(setExportSpectraSources(bool)));
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), config_, SLOT(setExportSpectraSources(bool)));

	// Setting up the layout.
	QGridLayout *contentsLayout = new QGridLayout;
	contentsLayout->addWidget(positionsBox, 0, 0, 2, 3);
	contentsLayout->addWidget(timeGroupBox, 2, 0, 1, 1);
	contentsLayout->addWidget(ccdBox, 3, 0, 1, 1);
	contentsLayout->addLayout(scanNameLayout, 4, 0, 1, 1);
	contentsLayout->addWidget(timeOffsetBox, 5, 0, 1, 1);
	contentsLayout->addWidget(motorSetChoiceBox, 0, 3, 1, 1);
	contentsLayout->addWidget(fluorescenceDetectorGroupBox, 1, 3, 2, 1);
	contentsLayout->addLayout(ionChambersLayout, 3, 3, 2, 1);
	contentsLayout->addWidget(roiTextBox, 0, 5, 3, 3);
	contentsLayout->addWidget(autoExportGroupBox, 3, 5, 1, 3);

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

void VESPERSSpatialLineScanConfigurationView::onFluorescenceDetectorChanged(int id)
{
	config_->setFluorescenceDetector(id);
	updateRoiText();
}

void VESPERSSpatialLineScanConfigurationView::onMotorChanged(int id)
{
	config_->setMotor(id);

	otherPositionLabel_->setText(config_->otherMotorString(config_->motor()));

	switch(config_->otherMotor(config_->motor())){

	case VESPERS::H:
		otherPosition_->setValue(VESPERSBeamline::vespers()->pseudoSampleStage()->horiz()->value());
		break;

	case VESPERS::V:
		otherPosition_->setValue(VESPERSBeamline::vespers()->pseudoSampleStage()->vert()->value());
		break;

	case VESPERS::X:
		otherPosition_->setValue(VESPERSBeamline::vespers()->realSampleStage()->horiz()->value());
		break;

	case VESPERS::Z:
		otherPosition_->setValue(VESPERSBeamline::vespers()->realSampleStage()->vert()->value());
		break;

	default:
		break;
	}
}

void VESPERSSpatialLineScanConfigurationView::onConfigureRoperDetectorClicked()
{
	emit configureDetector("Roper CCD");
}

void VESPERSSpatialLineScanConfigurationView::onItClicked(int id)
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

void VESPERSSpatialLineScanConfigurationView::onCCDDetectorChanged(int useCCD)
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

void VESPERSSpatialLineScanConfigurationView::updateRoiText()
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

void VESPERSSpatialLineScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + VESPERS::convertTimeToString(config_->totalTime()));
}

void VESPERSSpatialLineScanConfigurationView::onSetStartPosition()
{
	double position = 0;
	double otherPosition = 0;

	switch(int(config_->motor())){

	case VESPERS::H:
		position = VESPERSBeamline::vespers()->pseudoSampleStage()->horiz()->value();
		otherPosition = VESPERSBeamline::vespers()->pseudoSampleStage()->vert()->value();
		break;

	case VESPERS::X:
		position = VESPERSBeamline::vespers()->sampleStageX()->value();
		otherPosition = VESPERSBeamline::vespers()->sampleStageZ()->value();
		break;

	case VESPERS::V:
		position = VESPERSBeamline::vespers()->pseudoSampleStage()->vert()->value();
		otherPosition = VESPERSBeamline::vespers()->pseudoSampleStage()->horiz()->value();
		break;

	case VESPERS::Z:
		position = VESPERSBeamline::vespers()->sampleStageZ()->value();
		otherPosition = VESPERSBeamline::vespers()->sampleStageX()->value();
		break;
	}

	config_->setStart(position);
	start_->setValue(position);
	config_->setOtherPosition(otherPosition);
	otherPosition_->setValue(otherPosition);
	updateMapInfo();
	axesAcceptable();
}

void VESPERSSpatialLineScanConfigurationView::onSetEndPosition()
{
	double position = 0;
	double otherPosition = 0;

	switch(int(config_->motor())){

	case VESPERS::H:
		position = VESPERSBeamline::vespers()->pseudoSampleStage()->horiz()->value();
		otherPosition = VESPERSBeamline::vespers()->pseudoSampleStage()->vert()->value();
		break;

	case VESPERS::X:
		position = VESPERSBeamline::vespers()->sampleStageX()->value();
		otherPosition = VESPERSBeamline::vespers()->sampleStageZ()->value();
		break;

	case VESPERS::V:
		position = VESPERSBeamline::vespers()->pseudoSampleStage()->vert()->value();
		otherPosition = VESPERSBeamline::vespers()->pseudoSampleStage()->horiz()->value();
		break;

	case VESPERS::Z:
		position = VESPERSBeamline::vespers()->sampleStageZ()->value();
		otherPosition = VESPERSBeamline::vespers()->sampleStageX()->value();
		break;
	}

	config_->setEnd(position);
	end_->setValue(position);
	config_->setOtherPosition(otherPosition);
	otherPosition_->setValue(otherPosition);
	updateMapInfo();
	axesAcceptable();
}

void VESPERSSpatialLineScanConfigurationView::onStartChanged()
{
	config_->setStart(start_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERSSpatialLineScanConfigurationView::onEndChanged()
{
	config_->setEnd(end_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERSSpatialLineScanConfigurationView::onStepChanged()
{
	config_->setStep(step_->value()/1000);
	updateMapInfo();
	axesAcceptable();
}

void VESPERSSpatialLineScanConfigurationView::onDwellTimeChanged()
{
	config_->setTime(dwellTime_->value());
}

void VESPERSSpatialLineScanConfigurationView::onOtherPositionChanged()
{
	config_->setOtherPosition(otherPosition_->value());
}

void VESPERSSpatialLineScanConfigurationView::updateMapInfo()
{
	double size = fabs(config_->end()-config_->start());
	int points = int(fabs(size/config_->step()));
	if ((size - (points + 0.01)*config_->step()) < 0)
		points += 1;
	else
		points += 2;

	mapInfo_->setText(QString("Scan Size: %1 %2\t Points: %3")
					  .arg(QString::number(size*1000, 'f', 1))
					  .arg(QString::fromUtf8("µm"))
					  .arg(points)
					  );
}

void VESPERSSpatialLineScanConfigurationView::axesAcceptable()
{
	QPalette good(this->palette());
	QPalette bad(good);
	bad.setColor(QPalette::Base, Qt::red);

	step_->setPalette(config_->validAxis() ? good : bad);
}

