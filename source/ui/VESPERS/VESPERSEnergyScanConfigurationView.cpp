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


#include "VESPERSEnergyScanConfigurationView.h"

#include "ui/AMTopFrame.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>

VESPERSEnergyScanConfigurationView::VESPERSEnergyScanConfigurationView(VESPERSEnergyScanConfiguration *config, QWidget *parent)
	: VESPERSScanConfigurationView(parent)
{
	config_ = config;
	AMTopFrame *frame = new AMTopFrame("VESPERS Energy Scan Configuration");

	// Regions setup
	regionsView_ = new AMRegionsView(config_->regions());
	regionsView_->setMinimumWidth(400);
	regionsView_->setMaximumHeight(100);

	// The CCD detector setup.
	ccdButtonGroup_ = new QButtonGroup;
	QRadioButton *tempButton;
	QVBoxLayout *ccdDetectorLayout = new QVBoxLayout;

	tempButton = new QRadioButton("Roper");
	ccdButtonGroup_->addButton(tempButton, 1);
	ccdDetectorLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Mar");
	ccdButtonGroup_->addButton(tempButton, 2);
	ccdDetectorLayout->addWidget(tempButton);

	connect(ccdButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onCCDDetectorChanged(int)));
	connect(config_->dbObject(), SIGNAL(ccdDetectorChanged(int)), this, SLOT(updateCCDDetectorButtons(int)));

	ccdButtonGroup_->button(int(config_->ccdDetector()))->setChecked(true);

	QGroupBox *ccdDetectorGroupBox = new QGroupBox("CCD Detector");
	ccdDetectorGroupBox->setLayout(ccdDetectorLayout);

	// Scan name selection
	scanName_ = addScanNameView(config_->name());
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(config_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
	onScanNameEdited();

	QFormLayout *scanNameLayout = new QFormLayout;
	scanNameLayout->addRow("Scan Name:", scanName_);

	// Setting the scan position.
	QGroupBox *goToPositionGroupBox = addGoToPositionView(config_->goToPosition(), config_->x(), config_->y());

	setSampleStage(VESPERSBeamline::vespers()->experimentConfiguration()->sampleStageChoice());

	connect(config_, SIGNAL(gotoPositionChanged(bool)), goToPositionCheckBox_, SLOT(setChecked(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), config_, SLOT(setGoToPosition(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), setCurrentPositionButton_, SLOT(setEnabled(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), xPosition_, SLOT(setEnabled(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), yPosition_, SLOT(setEnabled(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), savedXPosition_, SLOT(setEnabled(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), savedYPosition_, SLOT(setEnabled(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), positionsSaved_, SLOT(setEnabled(bool)));
	connect(setCurrentPositionButton_, SIGNAL(clicked()), this, SLOT(setScanPosition()));

	currentCCDFileName_ = new QLabel;
	onCCDFileNameChanged(config_->ccdFileName());
	connect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(ccdNameChanged(QString)), config_->dbObject(), SLOT(setCCDFileName(QString)));
	connect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(ccdNameChanged(QString)), this, SLOT(onCCDFileNameChanged(QString)));

	QPushButton *configureRoperDetectorButton = new QPushButton(QIcon(":/hammer-wrench.png"), "Configure CCD");
	connect(configureRoperDetectorButton, SIGNAL(clicked()), this, SLOT(onConfigureCCDDetectorClicked()));

	QHBoxLayout *ccdBoxFirstRowLayout = new QHBoxLayout;
	ccdBoxFirstRowLayout->addWidget(currentCCDFileName_);
	ccdBoxFirstRowLayout->addWidget(configureRoperDetectorButton);

	// The estimated scan time.
	estimatedTime_ = new QLabel;
	connect(config_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	onEstimatedTimeChanged();

	// Setting up the steps to show the time offset for scan time estimation.
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
	setContextMenuPolicy(Qt::CustomContextMenu);

	QGroupBox *timeOffsetBox = addTimeOffsetLabel(config_->timeOffset());
	connect(timeOffset_, SIGNAL(valueChanged(double)), this, SLOT(setTimeOffset(double)));

	// Label showing where the data will be saved.
	QLabel *exportPath = addExportPathLabel();

	// Setting up the layout.
	QHBoxLayout *topRowLayout = new QHBoxLayout;
	topRowLayout->addStretch();
	topRowLayout->addWidget(regionsView_, 0, Qt::AlignLeft);
	topRowLayout->addWidget(ccdDetectorGroupBox);
	topRowLayout->addWidget(goToPositionGroupBox);
	topRowLayout->addStretch();

	QHBoxLayout *secondRowLayout = new QHBoxLayout;
	secondRowLayout->addStretch();
	secondRowLayout->addLayout(scanNameLayout);
	secondRowLayout->addLayout(ccdBoxFirstRowLayout);
	secondRowLayout->addStretch();

	QHBoxLayout *thirdRowLayout = new QHBoxLayout;
	thirdRowLayout->addStretch();
	thirdRowLayout->addWidget(estimatedTime_, 0, Qt::AlignLeft);
	thirdRowLayout->addWidget(timeOffsetBox);
	thirdRowLayout->addStretch();

	QVBoxLayout *contentsLayout = new QVBoxLayout;
	contentsLayout->addLayout(topRowLayout);
	contentsLayout->addLayout(secondRowLayout);
	contentsLayout->addLayout(thirdRowLayout);

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addWidget(frame);
	configViewLayout->addStretch();
	configViewLayout->addLayout(contentsLayout);
	configViewLayout->addStretch();
	configViewLayout->addWidget(exportPath, 0, Qt::AlignCenter);
	configViewLayout->addSpacing(30);

	setLayout(configViewLayout);
}

void VESPERSEnergyScanConfigurationView::onConfigureCCDDetectorClicked()
{
	if (config_->ccdDetector() == VESPERS::Roper)
		emit configureDetector("Roper CCD");
	else if (config_->ccdDetector() == VESPERS::Mar)
		emit configureDetector("Mar CCD");
}

void VESPERSEnergyScanConfigurationView::updateCCDDetectorButtons(int detector)
{
	ccdButtonGroup_->button(detector)->setChecked(true);

	// Roper
	if (detector == 1){

		disconnect(VESPERSBeamline::vespers()->marCCD(), SIGNAL(ccdNameChanged(QString)), config_->dbObject(), SLOT(setCCDFileName(QString)));
		disconnect(VESPERSBeamline::vespers()->marCCD(), SIGNAL(ccdNameChanged(QString)), this, SLOT(onCCDFileNameChanged(QString)));
		connect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(ccdNameChanged(QString)), config_->dbObject(), SLOT(setCCDFileName(QString)));
		connect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(ccdNameChanged(QString)), this, SLOT(onCCDFileNameChanged(QString)));
		onCCDFileNameChanged(VESPERSBeamline::vespers()->roperCCD()->ccdFileName());
		config_->setCCDFileName(VESPERSBeamline::vespers()->roperCCD()->ccdFileName());
	}

	// Mar
	else if (detector == 2){

		disconnect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(ccdNameChanged(QString)), config_->dbObject(), SLOT(setCCDFileName(QString)));
		disconnect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(ccdNameChanged(QString)), this, SLOT(onCCDFileNameChanged(QString)));
		connect(VESPERSBeamline::vespers()->marCCD(), SIGNAL(ccdNameChanged(QString)), config_->dbObject(), SLOT(setCCDFileName(QString)));
		connect(VESPERSBeamline::vespers()->marCCD(), SIGNAL(ccdNameChanged(QString)), this, SLOT(onCCDFileNameChanged(QString)));
		onCCDFileNameChanged(VESPERSBeamline::vespers()->marCCD()->ccdFileName());
		config_->setCCDFileName(VESPERSBeamline::vespers()->marCCD()->ccdFileName());
	}
}

void VESPERSEnergyScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time:\t" + VESPERS::convertTimeToString(config_->totalTime()));
}

void VESPERSEnergyScanConfigurationView::setSampleStage(bool sampleStage)
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
