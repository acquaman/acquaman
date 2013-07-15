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
	QGroupBox *ccdDetectorGroupBox = addCCDDetectorSelectionView();
	ccdButtonGroup_->button(int(VESPERS::NoCCD))->setDisabled(true);
	ccdButtonGroup_->button((int)VESPERS::Roper)->hide();
	ccdButtonGroup_->button((int)VESPERS::Mar)->hide();
	connect(ccdButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onCCDDetectorChanged(int)));
	connect(config_->dbObject(), SIGNAL(ccdDetectorChanged(int)), this, SLOT(updateCCDDetectorButtons(int)));
	ccdButtonGroup_->button(int(config_->ccdDetector()))->setChecked(true);

	// CCD label.
	ccdText_ = new QLabel;
	ccdHelpText_ = new QLabel;
	ccdTextBox_ = new QGroupBox("CCD Detector Info");
	QVBoxLayout *ccdTextLayout = new QVBoxLayout;
	ccdTextLayout->addWidget(ccdText_);
	ccdTextLayout->addWidget(ccdHelpText_);
	ccdTextBox_->setLayout(ccdTextLayout);
	ccdTextBox_->setVisible(config_->ccdDetector() != VESPERS::NoCCD);

	// Scan name selection
	scanName_ = addScanNameView(config_->name());
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(config_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
	onScanNameEdited();

	QFormLayout *scanNameLayout = new QFormLayout;
	scanNameLayout->addRow("Scan Name:", scanName_);

	// Setting the scan position.
	QGroupBox *goToPositionGroupBox = addGoToPositionView(config_->goToPosition(), config_->x(), config_->y());

	connect(config_, SIGNAL(gotoPositionChanged(bool)), goToPositionCheckBox_, SLOT(setChecked(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), config_, SLOT(setGoToPosition(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), setCurrentPositionButton_, SLOT(setEnabled(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), savedXPosition_, SLOT(setEnabled(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), savedYPosition_, SLOT(setEnabled(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), positionsSaved_, SLOT(setEnabled(bool)));
	connect(setCurrentPositionButton_, SIGNAL(clicked()), this, SLOT(setScanPosition()));
	connect(config_->dbObject(), SIGNAL(motorChanged(int)), this, SLOT(onMotorsUpdated(int)));

	onMotorsUpdated(config_->motor());

	QPushButton *configureRoperDetectorButton = new QPushButton(QIcon(":/hammer-wrench.png"), "Configure CCD");
	connect(configureRoperDetectorButton, SIGNAL(clicked()), this, SLOT(onConfigureCCDDetectorClicked()));

	QHBoxLayout *ccdBoxFirstRowLayout = new QHBoxLayout;
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
	contentsLayout->addWidget(ccdTextBox_);

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addWidget(frame);
	configViewLayout->addStretch();
	configViewLayout->addLayout(contentsLayout);
	configViewLayout->addStretch();
	configViewLayout->addWidget(exportPath, 0, Qt::AlignCenter);
	configViewLayout->addSpacing(30);

	setLayout(configViewLayout);
}

void VESPERSEnergyScanConfigurationView::onScanNameEdited()
{
	QString name = scanName_->text();
	config_->setName(name);
	config_->setUserScanName(name);

	if (config_->ccdDetector() != VESPERS::NoCCD){

		QString path;

		if (config_->ccdDetector() == VESPERS::Roper)
			path = VESPERSBeamline::vespers()->roperCCD()->ccdFilePath();

		else if (config_->ccdDetector() == VESPERS::Mar)
			path = VESPERSBeamline::vespers()->marCCD()->ccdFilePath();

		else if (config_->ccdDetector() == VESPERS::Pilatus)
			path = VESPERSBeamline::vespers()->pilatusCCD()->ccdFilePath();

		ccdText_->setText(QString("Path: %1\nName: %2").arg(path).arg(name));
		config_->setCCDFileName(name);
		checkCCDFileNames(name);
	}
}

void VESPERSEnergyScanConfigurationView::checkCCDFileNames(const QString &name) const
{
	QString path;

	if (config_->ccdDetector() == VESPERS::Roper){

		path = VESPERSBeamline::vespers()->roperCCD()->ccdFilePath();
		path.replace("Y:\\", "/nas/vespers/");
		path.replace('\\', '/');
	}

	else if (config_->ccdDetector() == VESPERS::Mar)
		path = VESPERSBeamline::vespers()->marCCD()->ccdFilePath();

	else if (config_->ccdDetector() == VESPERS::Pilatus)
		path = VESPERSBeamline::vespers()->pilatusCCD()->ccdFilePath();

	if (VESPERS::fileNameExists(path, name)){

		ccdHelpText_->show();
		ccdHelpText_->setText(QString("The scan name you have chosen conflicts with existing CCD file names.\nIf you don't a random suffix will be added to avoid name conflicts.\neg. %1").arg(VESPERS::appendUniqueIdentifier(name)));
	}

	else{

		ccdHelpText_->setText("");
		ccdHelpText_->hide();
	}
}

void VESPERSEnergyScanConfigurationView::onCCDDetectorChanged(int id)
{
	config_->setCCDDetector(id);

	QString path;
	QString name = config_->ccdFileName().isEmpty() ? scanName_->text() : config_->ccdFileName();

	if (config_->ccdDetector() == VESPERS::Roper)
		path = VESPERSBeamline::vespers()->roperCCD()->ccdFilePath();

	else if (config_->ccdDetector() == VESPERS::Mar)
		path = VESPERSBeamline::vespers()->marCCD()->ccdFilePath();

	else if (config_->ccdDetector() == VESPERS::Pilatus)
		path = VESPERSBeamline::vespers()->pilatusCCD()->ccdFilePath();

	config_->setCCDFileName(name);
	ccdText_->setText(QString("Path: %1\nName: %2").arg(path).arg(name));
	checkCCDFileNames(name);
}

void VESPERSEnergyScanConfigurationView::onMotorsUpdated(int id)
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

void VESPERSEnergyScanConfigurationView::setScanPosition()
{
	double x = 0;
	double y = 0;

	switch(int(config_->motor())){

	case VESPERS::H | VESPERS::V:

		x = VESPERSBeamline::vespers()->pseudoSampleStage()->horiz()->value();
		y = VESPERSBeamline::vespers()->pseudoSampleStage()->vert()->value();
		savedXPosition_->setText(QString("H: %1 mm").arg(x, 0, 'g', 3));
		savedYPosition_->setText(QString("V: %1 mm").arg(y, 0, 'g', 3));
		break;

	case VESPERS::X | VESPERS::Z:

		x = VESPERSBeamline::vespers()->realSampleStage()->horiz()->value();
		y = VESPERSBeamline::vespers()->realSampleStage()->vert()->value();
		savedXPosition_->setText(QString("X: %1 mm").arg(x, 0, 'g', 3));
		savedYPosition_->setText(QString("Z: %1 mm").arg(y, 0, 'g', 3));
		break;

	case VESPERS::AttoH | VESPERS::AttoV:

		x = VESPERSBeamline::vespers()->pseudoAttoStage()->horiz()->value();
		y = VESPERSBeamline::vespers()->pseudoAttoStage()->vert()->value();
		savedXPosition_->setText(QString("H: %1 mm").arg(x, 0, 'g', 3));
		savedYPosition_->setText(QString("V: %1 mm").arg(y, 0, 'g', 3));
		break;

	case VESPERS::AttoX | VESPERS::AttoZ:


		x = VESPERSBeamline::vespers()->realAttoStage()->horiz()->value();
		y = VESPERSBeamline::vespers()->realAttoStage()->vert()->value();
		savedXPosition_->setText(QString("X: %1 mm").arg(x, 0, 'g', 3));
		savedYPosition_->setText(QString("Z: %1 mm").arg(y, 0, 'g', 3));
		break;
	}

	config_->setPosition(x, y);
	positionsSaved_->setText("Saved");
	QPalette palette(this->palette());
	palette.setColor(QPalette::Active, QPalette::WindowText, Qt::darkGreen);
	positionsSaved_->setPalette(palette);
}


void VESPERSEnergyScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time:\t" + VESPERS::convertTimeToString(config_->totalTime()));
}
