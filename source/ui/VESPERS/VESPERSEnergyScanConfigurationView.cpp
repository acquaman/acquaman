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
#include "ui/dataman/AMStepScanAxisView.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>

VESPERSEnergyScanConfigurationView::~VESPERSEnergyScanConfigurationView(){}

VESPERSEnergyScanConfigurationView::VESPERSEnergyScanConfigurationView(VESPERSEnergyScanConfiguration *config, QWidget *parent)
	: VESPERSScanConfigurationView(parent)
{
	configuration_ = config;
	AMTopFrame *frame = new AMTopFrame("VESPERS Energy Scan Configuration");

	// Regions setup
	AMStepScanAxisView *regionsView = new AMStepScanAxisView("Energy Region Configuration", configuration_);

	// CCD label.
	ccdText_ = new QLabel;
	ccdHelpText_ = new QLabel;
	ccdTextBox_ = new QGroupBox("CCD Detector Info");
	QVBoxLayout *ccdTextLayout = new QVBoxLayout;
	ccdTextLayout->addWidget(ccdText_);
	ccdTextLayout->addWidget(ccdHelpText_);
	ccdTextBox_->setLayout(ccdTextLayout);
	ccdTextBox_->setVisible(configuration_->ccdDetector() != VESPERS::NoCCD);

	// The CCD detector setup.
	ccdComboBox_ = createCCDComboBox();
	connect(ccdComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onCCDDetectorChanged(int)));
	connect(configuration_->dbObject(), SIGNAL(ccdDetectorChanged(int)), this, SLOT(updateCCDDetectorComboBox(int)));

	// Scan name selection
	scanName_ = createScanNameView(configuration_->name());
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
	// Only connecting this signal because it is the only CCD available currently.  It would need some logic for switching which CCD it was actually connected to.
	connect(VESPERSBeamline::vespers()->vespersPilatusAreaDetector(), SIGNAL(ccdPathChanged(QString)), this, SLOT(onScanNameEdited()));
	onScanNameEdited();

	QFormLayout *scanNameLayout = new QFormLayout;
	scanNameLayout->addRow("Scan Name:", scanName_);

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
	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	onEstimatedTimeChanged();

	// Setting up the steps to show the time offset for scan time estimation.
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
	setContextMenuPolicy(Qt::CustomContextMenu);

	QGroupBox *timeOffsetBox = addTimeOffsetLabel(configuration_->timeOffset());
	connect(timeOffset_, SIGNAL(valueChanged(double)), this, SLOT(setTimeOffset(double)));

	// Label showing where the data will be saved.
	QLabel *exportPath = addExportPathLabel();

	ccdComboBox_->setCurrentIndex(ccdComboBox_->findData(int(configuration_->ccdDetector())));

	// Setting up the layout.
	QHBoxLayout *topRowLayout = new QHBoxLayout;
	topRowLayout->addStretch();
	topRowLayout->addWidget(regionsView);
	topRowLayout->addWidget(ccdComboBox_);
	topRowLayout->addWidget(goToPositionGroupBox);
	topRowLayout->addStretch();

	QHBoxLayout *secondRowLayout = new QHBoxLayout;
	secondRowLayout->addStretch();
	secondRowLayout->addLayout(scanNameLayout);
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
	configuration_->setName(name);
	configuration_->setUserScanName(name);

	if (configuration_->ccdDetector() != VESPERS::NoCCD){

		QString path;

		if (configuration_->ccdDetector() == VESPERS::Roper)
			path = VESPERSBeamline::vespers()->vespersRoperCCD()->ccdFilePath();

		else if (configuration_->ccdDetector() == VESPERS::Mar)
			path = VESPERSBeamline::vespers()->vespersMarCCD()->ccdFilePath();

		else if (configuration_->ccdDetector() == VESPERS::Pilatus)
			path = VESPERSBeamline::vespers()->vespersPilatusAreaDetector()->ccdFilePath();

		ccdText_->setText(QString("Path: %1\nName: %2").arg(path).arg(name));
		configuration_->setCCDFileName(name);
		checkCCDFileNames(name);
	}

	if (configuration_->ccdDetector() == VESPERS::Pilatus && name.contains(" ")){

		QPalette palette = scanName_->palette();
		palette.setColor(QPalette::Base, Qt::red);
		scanName_->setPalette(palette);
	}

	else
		scanName_->setPalette(this->palette());

	double n = 0;
	VESPERS::Motors motor = configuration_->motor();

	if (motor == (VESPERS::H | VESPERS::V))
		n = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->normalControl()->value();

	else if (motor == (VESPERS::X | VESPERS::Z))
		n = VESPERSBeamline::vespers()->sampleStageY()->value();

	configuration_->setNormalPosition(n);
}

void VESPERSEnergyScanConfigurationView::checkCCDFileNames(const QString &name) const
{
	QString path;

	if (configuration_->ccdDetector() == VESPERS::Roper){

		path = VESPERSBeamline::vespers()->vespersRoperCCD()->ccdFilePath();
		path.replace("Y:\\", "/nas/vespers/");
		path.replace('\\', '/');
	}

	else if (configuration_->ccdDetector() == VESPERS::Mar)
		path = VESPERSBeamline::vespers()->vespersMarCCD()->ccdFilePath();

	else if (configuration_->ccdDetector() == VESPERS::Pilatus)
		path = VESPERSBeamline::vespers()->vespersPilatusAreaDetector()->ccdFilePath();

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
	configuration_->setCCDDetector(ccdComboBox_->itemData(id).toInt());

	QString path;
	QString name = configuration_->ccdFileName().isEmpty() ? scanName_->text() : configuration_->ccdFileName();

	if (configuration_->ccdDetector() == VESPERS::Roper)
		path = VESPERSBeamline::vespers()->vespersRoperCCD()->ccdFilePath();

	else if (configuration_->ccdDetector() == VESPERS::Mar)
		path = VESPERSBeamline::vespers()->vespersMarCCD()->ccdFilePath();

	else if (configuration_->ccdDetector() == VESPERS::Pilatus)
		path = VESPERSBeamline::vespers()->vespersPilatusAreaDetector()->ccdFilePath();

	configuration_->setCCDFileName(name);
	ccdText_->setText(QString("Path: %1\nName: %2").arg(path).arg(name));
	checkCCDFileNames(name);
	onScanNameEdited();
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


void VESPERSEnergyScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time:\t" + VESPERS::convertTimeToString(configuration_->totalTime()));
}
