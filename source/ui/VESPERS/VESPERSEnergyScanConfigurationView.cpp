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
	: AMScanConfigurationView(parent)
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
	ccdButtonGroup_->addButton(tempButton, 0);
	ccdDetectorLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Mar");
	ccdButtonGroup_->addButton(tempButton, 1);
	ccdDetectorLayout->addWidget(tempButton);

	connect(ccdButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onCcdDetectorChoiceChanged(int)));
	connect(config_, SIGNAL(ccdDetectorChanged(int)), this, SLOT(updateCcdDetectorChoiceButtons(int)));

	ccdButtonGroup_->button(int(config_->ccdDetector()))->setChecked(true);

	QGroupBox *ccdDetectorGroupBox = new QGroupBox("CCD Detector");
	ccdDetectorGroupBox->setLayout(ccdDetectorLayout);

	// Scan name selection
	scanName_ = new QLineEdit;
	scanName_->setText(config_->name());
	scanName_->setAlignment(Qt::AlignCenter);
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(config_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
	onScanNameEdited();

	QFormLayout *scanNameLayout = new QFormLayout;
	scanNameLayout->addRow("Scan Name:", scanName_);

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

	currentCCDFileName_ = new QLabel;
	onCCDFileNameChanged(config_->ccdFileName());
	connect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(ccdNameChanged(QString)), config_, SLOT(setCCDFileName(QString)));
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

	// Label showing where the data will be saved.
	QString exportString =  AMUserSettings::userDataFolder;
	exportString.remove("/userData");
	QLabel *exportPath = new QLabel(QString("Data exported to: %1exportData").arg(exportString));

	// Setting up the layout.
	QHBoxLayout *topRowLayout = new QHBoxLayout;
	topRowLayout->addStretch();
	topRowLayout->addWidget(regionsView_, 0, Qt::AlignLeft);
	topRowLayout->addWidget(ccdDetectorGroupBox);
	topRowLayout->addLayout(positionLayout);
	topRowLayout->addStretch();

	QHBoxLayout *secondRowLayout = new QHBoxLayout;
	secondRowLayout->addStretch();
	secondRowLayout->addLayout(scanNameLayout);
	secondRowLayout->addLayout(ccdBoxFirstRowLayout);
	secondRowLayout->addStretch();

	QHBoxLayout *thirdRowLayout = new QHBoxLayout;
	thirdRowLayout->addStretch();
	thirdRowLayout->addWidget(estimatedTime_, 0, Qt::AlignLeft);
	thirdRowLayout->addLayout(timeOffsetLayout);
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
	if (config_->ccdDetector() == VESPERSEnergyScanConfiguration::Roper)
		emit configureDetector("Roper CCD");
	else if (config_->ccdDetector() == VESPERSEnergyScanConfiguration::Mar)
		emit configureDetector("Mar CCD");
}

void VESPERSEnergyScanConfigurationView::updateCcdDetectorChoiceButtons(int detector)
{
	ccdButtonGroup_->button(detector)->setChecked(true);

	// Roper
	if (detector == 0){

		disconnect(VESPERSBeamline::vespers()->marCCD(), SIGNAL(ccdNameChanged(QString)), config_, SLOT(setCCDFileName(QString)));
		disconnect(VESPERSBeamline::vespers()->marCCD(), SIGNAL(ccdNameChanged(QString)), this, SLOT(onCCDFileNameChanged(QString)));
		connect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(ccdNameChanged(QString)), config_, SLOT(setCCDFileName(QString)));
		connect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(ccdNameChanged(QString)), this, SLOT(onCCDFileNameChanged(QString)));
		onCCDFileNameChanged(VESPERSBeamline::vespers()->roperCCD()->ccdFileName());
		config_->setCCDFileName(VESPERSBeamline::vespers()->roperCCD()->ccdFileName());
	}

	// Mar
	else if (detector == 1){

		disconnect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(ccdNameChanged(QString)), config_, SLOT(setCCDFileName(QString)));
		disconnect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(ccdNameChanged(QString)), this, SLOT(onCCDFileNameChanged(QString)));
		connect(VESPERSBeamline::vespers()->marCCD(), SIGNAL(ccdNameChanged(QString)), config_, SLOT(setCCDFileName(QString)));
		connect(VESPERSBeamline::vespers()->marCCD(), SIGNAL(ccdNameChanged(QString)), this, SLOT(onCCDFileNameChanged(QString)));
		onCCDFileNameChanged(VESPERSBeamline::vespers()->marCCD()->ccdFileName());
		config_->setCCDFileName(VESPERSBeamline::vespers()->marCCD()->ccdFileName());
	}
}

void VESPERSEnergyScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time:\t" + convertTimeToString(config_->totalTime()));
}

QString VESPERSEnergyScanConfigurationView::convertTimeToString(double time)
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

void VESPERSEnergyScanConfigurationView::onCustomContextMenuRequested(QPoint pos)
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
