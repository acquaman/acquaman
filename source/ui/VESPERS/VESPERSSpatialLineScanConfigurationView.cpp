/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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

#include "util/AMDateTimeUtils.h"
#include "ui/AMTopFrame.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QPushButton>
#include <QSpinBox>
#include <QMenu>

VESPERSSpatialLineScanConfigurationView::~VESPERSSpatialLineScanConfigurationView(){}

VESPERSSpatialLineScanConfigurationView::VESPERSSpatialLineScanConfigurationView(VESPERSSpatialLineScanConfiguration *config, QWidget *parent)
	: VESPERSScanConfigurationView(parent)
{
	configuration_ = config;
	AMTopFrame *frame = new AMTopFrame("VESPERS Line Scan Configuration");

	// Setup the group box for setting the start and end points.
	QGroupBox *positionsBox = new QGroupBox("Positions");

	start_ = createPositionDoubleSpinBox("", " mm", configuration_->start(), 3);
	connect(start_, SIGNAL(editingFinished()), this, SLOT(onStartChanged()));
	connect(configuration_, SIGNAL(startChanged(double)), start_, SLOT(setValue(double)));

	QPushButton *startUseCurrentButton = new QPushButton("Use Current");
	connect(startUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetStartPosition()));

	QHBoxLayout *startPointLayout = new QHBoxLayout;
	startPointLayout->addWidget(new QLabel("Start:"));
	startPointLayout->addWidget(start_);
	startPointLayout->addWidget(startUseCurrentButton);

	end_ = createPositionDoubleSpinBox("", " mm", configuration_->end(), 3);
	connect(end_, SIGNAL(editingFinished()), this, SLOT(onEndChanged()));
	connect(configuration_, SIGNAL(endChanged(double)), end_, SLOT(setValue(double)));

	QPushButton *endUseCurrentButton = new QPushButton("Use Current");
	connect(endUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetEndPosition()));

	QHBoxLayout *endPointLayout = new QHBoxLayout;
	endPointLayout->addWidget(new QLabel("End:"));
	endPointLayout->addWidget(end_);
	endPointLayout->addWidget(endUseCurrentButton);

	step_ = createPositionDoubleSpinBox("", QString(" %1").arg(QString::fromUtf8("µm")), configuration_->step()*1000, 1);	// xStep needs to be in mm.
	connect(step_, SIGNAL(editingFinished()), this, SLOT(onStepChanged()));
	connect(configuration_, SIGNAL(stepChanged(double)), this, SLOT(updateStep(double)));

	QHBoxLayout *stepSizeLayout = new QHBoxLayout;
	stepSizeLayout->addWidget(new QLabel("Step Size:"));
	stepSizeLayout->addWidget(step_);
	stepSizeLayout->addStretch();

	otherPositionLabel_ = new QLabel(configuration_->otherMotorString(configuration_->motor()));
	otherPosition_ = createPositionDoubleSpinBox("", " mm", configuration_->hasOtherPosition() ? configuration_->otherPosition() : 0, 3);
	connect(otherPosition_, SIGNAL(editingFinished()), this, SLOT(onOtherPositionChanged()));
	connect(configuration_, SIGNAL(otherPositionChanged(double)), otherPosition_, SLOT(setValue(double)));

	QHBoxLayout *otherPositionLayout = new QHBoxLayout;
	otherPositionLayout->addWidget(otherPositionLabel_);
	otherPositionLayout->addWidget(otherPosition_);
	otherPositionLayout->addStretch();

	normalPosition_ = createPositionDoubleSpinBox("N: ", " mm", configuration_->normalPosition(), 3);
	connect(normalPosition_, SIGNAL(editingFinished()), this, SLOT(onNormalPositionChanged()));
	connect(configuration_->dbObject(), SIGNAL(normalPositionChanged(double)), normalPosition_, SLOT(setValue(double)));

	QPushButton *updateNormalPosition = new QPushButton("Set Normal");
	connect(updateNormalPosition, SIGNAL(clicked()), this, SLOT(onSetNormalPosition()));

	QHBoxLayout *normalLayout = new QHBoxLayout;
	normalLayout->addWidget(new QLabel("Focus Position:"));
	normalLayout->addWidget(normalPosition_);
	normalLayout->addWidget(updateNormalPosition);

	mapInfo_ = new QLabel;
	updateMapInfo();

	QVBoxLayout *positionsLayout = new QVBoxLayout;
	positionsLayout->addLayout(startPointLayout);
	positionsLayout->addLayout(endPointLayout);
	positionsLayout->addLayout(stepSizeLayout);
	positionsLayout->addLayout(otherPositionLayout);
	positionsLayout->addLayout(normalLayout);
	positionsLayout->addWidget(mapInfo_);

	positionsBox->setLayout(positionsLayout);

	// Dwell time.
	dwellTime_ = createDwellTimeSpinBox(configuration_->time());
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));
	connect(configuration_, SIGNAL(timeChanged(double)), dwellTime_, SLOT(setValue(double)));

	QHBoxLayout *timeLayout = new QHBoxLayout;
	timeLayout->addWidget(new QLabel("Dwell Time:"));
	timeLayout->addWidget(dwellTime_);

	// The estimated scan time.
	estimatedTime_ = new QLabel;
	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	onEstimatedTimeChanged();

	QVBoxLayout *timeBoxLayout = new QVBoxLayout;
	timeBoxLayout->addLayout(timeLayout);
	timeBoxLayout->addWidget(estimatedTime_);

	QGroupBox *timeGroupBox = new QGroupBox("Time");
	timeGroupBox->setLayout(timeBoxLayout);

	// CCD label.
	ccdText_ = new QLabel;
	ccdHelpText_ = new QLabel;
	ccdTextBox_ = new QGroupBox("CCD Detector Info");
	QVBoxLayout *ccdTextLayout = new QVBoxLayout;
	ccdTextLayout->addWidget(ccdText_);
	ccdTextLayout->addWidget(ccdHelpText_);
	ccdTextBox_->setLayout(ccdTextLayout);
	ccdTextBox_->setVisible(configuration_->ccdDetector() != VESPERS::NoCCD);

	// Using the CCD.
	ccdComboBox_ = createCCDComboBox();
	connect(ccdComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onCCDDetectorChanged(int)));
	connect(configuration_->dbObject(), SIGNAL(ccdDetectorChanged(int)), this, SLOT(updateCCDDetectorComboBox(int)));

	// The fluorescence detector setup
	fluorescenceDetectorComboBox_  = createFluorescenceComboBox();
	connect(fluorescenceDetectorComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onFluorescenceChoiceChanged(int)));
	connect(configuration_->dbObject(), SIGNAL(fluorescenceDetectorChanged(int)), this, SLOT(updateFluorescenceDetectorComboBox(int)));

	// Ion chamber selection
	itComboBox_ = createIonChamberComboBox();
	connect(itComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onItClicked(int)));
	connect(configuration_->dbObject(), SIGNAL(transmissionChoiceChanged(int)), this, SLOT(updateItComboBox(int)));

	i0ComboBox_ = createIonChamberComboBox();
	connect(i0ComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onI0Clicked(int)));
	connect(configuration_->dbObject(), SIGNAL(incomingChoiceChanged(int)), this, SLOT(updateI0ComboBox(int)));

	QHBoxLayout *ionChambersLayout = new QHBoxLayout;
	ionChambersLayout->addWidget(i0ComboBox_);
	ionChambersLayout->addWidget(itComboBox_);

	// Motor selection.
	motorSelectionComboBox_ = createMotorSelectionComboBox(
				QStringList() << "H"
								<< "X"
								<< "V"
								<< "Z"
								<< "Atto H"
								<< "Atto V"
								<< "Atto X"
								<< "Atto Z"
								<< "Atto Rz"
								<< "Atto Ry"
								<< "Atto Rx"
								<< "Big Beam X"
								<< "Big Beam Z"
								<< "Wire H"
								<< "Wire V"
								<< "Wire N",
				QList<int>() << VESPERS::H
								<< VESPERS::X
								<< VESPERS::V
								<< VESPERS::Z
								<< VESPERS::AttoH
								<< VESPERS::AttoV
								<< VESPERS::AttoX
								<< VESPERS::AttoZ
								<< VESPERS::AttoRz
								<< VESPERS::AttoRy
								<< VESPERS::AttoRx
								<< VESPERS::BigBeamX
								<< VESPERS::BigBeamZ
								<< VESPERS::WireH
								<< VESPERS::WireV
								<< VESPERS::WireN);
	connect(motorSelectionComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onMotorChanged(int)));
	connect(configuration_->dbObject(), SIGNAL(motorChanged(int)), this, SLOT(updateMotorSelectionComboBox(int)));

	// Scan name selection
	scanName_ = createScanNameView(configuration_->name());
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
	// Only connecting this signal because it is the only CCD available currently.  It would need some logic for switching which CCD it was actually connected to.
	connect(VESPERSBeamline::vespers()->vespersPilatusAreaDetector(), SIGNAL(ccdPathChanged(QString)), this, SLOT(onScanNameEdited()));
	onScanNameEdited();

	QFormLayout *scanNameLayout = new QFormLayout;
	scanNameLayout->addRow("Scan Name:", scanName_);

	QGroupBox *scanNameGroupBox = new QGroupBox("Scan Name");
	scanNameGroupBox->setLayout(scanNameLayout);

	// Label showing where the data will be saved.
	QLabel *exportPath = addExportPathLabel();

	QGroupBox *timeOffsetBox = addTimeOffsetLabel(configuration_->timeOffset());
	connect(timeOffset_, SIGNAL(valueChanged(double)), this, SLOT(setTimeOffset(double)));

	// Auto-export option.
	QGroupBox *autoExportGroupBox = addExporterOptionsView(QStringList(), configuration_->exportSpectraSources(), configuration_->exportSpectraInRows());
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), configuration_, SLOT(setExportSpectraSources(bool)));
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), configuration_, SLOT(setExportSpectraSources(bool)));
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), exportSpectraInRows_, SLOT(setEnabled(bool)));
	connect(exportSpectraInRows_, SIGNAL(toggled(bool)), this, SLOT(updateExportSpectraInRows(bool)));

	i0ComboBox_->setCurrentIndex((int)configuration_->incomingChoice());
	itComboBox_->setCurrentIndex((int)configuration_->transmissionChoice());
	fluorescenceDetectorComboBox_->setCurrentIndex((int)configuration_->fluorescenceDetector());
	ccdComboBox_->setCurrentIndex(ccdComboBox_->findData(int(configuration_->ccdDetector())));
	motorSelectionComboBox_->setCurrentIndex(motorSelectionComboBox_->findData(int(configuration_->motor())));

	disableStandardXRDOptions();
	disableStandardI0Options();
	disableStandardItOptions();

	QFormLayout *detectorLayout = new QFormLayout;
	detectorLayout->addRow("XRF:", fluorescenceDetectorComboBox_);
	detectorLayout->addRow("XRD:", ccdComboBox_);
	detectorLayout->addRow("I0:", i0ComboBox_);
	detectorLayout->addRow("It:", itComboBox_);
	detectorLayout->addRow("Stage:", motorSelectionComboBox_);

	QGroupBox *detectorGroupBox = new QGroupBox("Detectors");
	detectorGroupBox->setLayout(detectorLayout);

	QGroupBox *afterScanBox = createAfterScanOptionsBox(configuration_->closeFastShutter(), configuration_->returnToOriginalPosition(), configuration_->cleanupScaler());
	connect(closeFastShutterCheckBox_, SIGNAL(toggled(bool)), this, SLOT(setCloseFastShutter(bool)));
	connect(goToPositionCheckBox_, SIGNAL(toggled(bool)), this, SLOT(setReturnToOriginalPosition(bool)));
	connect(cleanupScalerCheckBox_, SIGNAL(toggled(bool)), this, SLOT(setCleanupScaler(bool)));

	// Setting up the layout.
	contentsLayout_ = new QGridLayout;
	contentsLayout_->addWidget(positionsBox, 0, 0, 2, 3);
	contentsLayout_->addWidget(timeGroupBox, 2, 0, 1, 3);
	contentsLayout_->addWidget(scanNameGroupBox, 3, 0, 1, 3);
	contentsLayout_->addWidget(ccdTextBox_, 4, 0, 1, 3);
	contentsLayout_->addWidget(timeOffsetBox, 5, 0, 1, 3);
	contentsLayout_->addWidget(detectorGroupBox, 0, 3, 2, 1);
	contentsLayout_->addWidget(autoExportGroupBox, 2, 3, 1, 1);
	contentsLayout_->addWidget(afterScanBox, 3, 3, 1, 1);

	QHBoxLayout *squeezeContents = new QHBoxLayout;
	squeezeContents->addStretch();
	squeezeContents->addLayout(contentsLayout_);
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

void VESPERSSpatialLineScanConfigurationView::onFluorescenceChoiceChanged(int id)
{
	configuration_->setFluorescenceDetector(id);
}

void VESPERSSpatialLineScanConfigurationView::onScanNameEdited()
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
}

void VESPERSSpatialLineScanConfigurationView::checkCCDFileNames(const QString &name) const
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

void VESPERSSpatialLineScanConfigurationView::onMotorChanged(int id)
{
	configuration_->setMotor(motorSelectionComboBox_->itemData(id).toInt());

	otherPositionLabel_->setText(configuration_->otherMotorString(configuration_->motor()));

	switch(configuration_->motor()){

	case VESPERS::H:
		otherPosition_->setValue(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalAxis()->translationMotor()->value());
		break;

	case VESPERS::V:
		otherPosition_->setValue(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalAxis()->translationMotor()->value());
		break;

	case VESPERS::X:
		otherPosition_->setValue(VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->verticalAxis()->translationMotor()->value());
		break;

	case VESPERS::Z:
		otherPosition_->setValue(VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->horizontalAxis()->translationMotor()->value());
		break;

	case VESPERS::AttoH:
		otherPosition_->setValue(VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->verticalAxis()->translationMotor()->value());
		break;

	case VESPERS::AttoV:
		otherPosition_->setValue(VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->horizontalAxis()->translationMotor()->value());
		break;

	case VESPERS::AttoX:
		otherPosition_->setValue(VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->verticalAxis()->translationMotor()->value());
		break;

	case VESPERS::AttoZ:
		otherPosition_->setValue(VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->horizontalAxis()->translationMotor()->value());
		break;

	case VESPERS::AttoRz:
		otherPosition_->setValue(-123456789.0);
		break;

	case VESPERS::AttoRy:
		otherPosition_->setValue(-123456789.0);
		break;

	case VESPERS::AttoRx:
		otherPosition_->setValue(-123456789.0);
		break;

	case VESPERS::BigBeamX:
		otherPosition_->setValue(VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->verticalAxis()->translationMotor()->value());
		break;

	case VESPERS::BigBeamZ:
		otherPosition_->setValue(VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->horizontalAxis()->translationMotor()->value());
		break;

	case VESPERS::WireH:
		otherPosition_->setValue(VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->verticalAxis()->translationMotor()->value());
		break;

	case VESPERS::WireV:
		otherPosition_->setValue(VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->horizontalAxis()->translationMotor()->value());
		break;

	default:
		break;
	}

	otherPositionLabel_->setVisible(configuration_->otherMotor(configuration_->motor()) != VESPERS::NoMotor);
	otherPosition_->setVisible(configuration_->otherMotor(configuration_->motor()) != VESPERS::NoMotor);
}

void VESPERSSpatialLineScanConfigurationView::onItClicked(int index)
{
	// If the new It is at or upstream of I0, move I0.  Using id-1 is safe because Isplit can't be chosen for It.
	if (index <= i0ComboBox_->currentIndex())
		i0ComboBox_->setCurrentIndex(index-1);

	QStandardItemModel *model = qobject_cast<QStandardItemModel *>(i0ComboBox_->model());

	for (int i = 0; i < i0ComboBox_->count(); i++)
		model->item(i)->setFlags(i < index ? (Qt::ItemIsEnabled | Qt::ItemIsSelectable) : Qt::NoItemFlags);

	configuration_->setTransmissionChoice(index);
}

void VESPERSSpatialLineScanConfigurationView::onCCDDetectorChanged(int id)
{
	configuration_->setCCDDetector(ccdComboBox_->itemData(id).toInt());

	if (configuration_->ccdDetector() != VESPERS::NoCCD){

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
	}

	onScanNameEdited();
	ccdTextBox_->setVisible(configuration_->ccdDetector() != VESPERS::NoCCD);
}

void VESPERSSpatialLineScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + AMDateTimeUtils::convertTimeToString(configuration_->totalTime()));
}

void VESPERSSpatialLineScanConfigurationView::onSetStartPosition()
{
	double position = 0;
	double otherPosition = 0;

	switch(int(configuration_->motor())){

	case VESPERS::H:
		position = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalAxis()->translationMotor()->value();
		break;

	case VESPERS::X:
		position = VESPERSBeamline::vespers()->sampleStageX()->value();
		otherPosition = VESPERSBeamline::vespers()->sampleStageZ()->value();
		break;

	case VESPERS::V:
		position = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		break;

	case VESPERS::Z:
		position = VESPERSBeamline::vespers()->sampleStageZ()->value();
		otherPosition = VESPERSBeamline::vespers()->sampleStageX()->value();
		break;

	case VESPERS::AttoH:
		position = VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->verticalAxis()->translationMotor()->value();
		break;

	case VESPERS::AttoV:
		position = VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->verticalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		break;

	case VESPERS::AttoX:
		position = VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->verticalAxis()->translationMotor()->value();
		break;

	case VESPERS::AttoZ:
		position = VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->verticalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		break;

	case VESPERS::AttoRz:
		position = VESPERSBeamline::vespers()->attoStageRz()->value();
		otherPosition = -123456789.0;
		break;

	case VESPERS::AttoRy:
		position = VESPERSBeamline::vespers()->attoStageRy()->value();
		otherPosition = -123456789.0;
		break;

	case VESPERS::AttoRx:
		position = VESPERSBeamline::vespers()->attoStageRx()->value();
		otherPosition = -123456789.0;
		break;

	case VESPERS::BigBeamX:
		position = VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->verticalAxis()->translationMotor()->value();
		break;

	case VESPERS::BigBeamZ:
		position = VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->verticalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		break;

	case VESPERS::WireH:
		position = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->verticalAxis()->translationMotor()->value();
		break;

	case VESPERS::WireV:
		position = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->verticalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		break;

	case VESPERS::WireN:
		position = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->normalAxis()->translationMotor()->value();
		otherPosition = -123456789.0;
		break;
	}

	configuration_->setStart(position);
	start_->setValue(position);
	configuration_->setOtherPosition(otherPosition);
	otherPosition_->setValue(otherPosition);
	updateMapInfo();
	axesAcceptable();
}

void VESPERSSpatialLineScanConfigurationView::onSetEndPosition()
{
	double position = 0;
	double otherPosition = 0;
	double n = 0;

	switch(int(configuration_->motor())){

	case VESPERS::H:
		position = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalAxis()->translationMotor()->value();
		  n = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->normalAxis()->translationMotor()->value();
		break;

	case VESPERS::X:
		position = VESPERSBeamline::vespers()->sampleStageX()->value();
		otherPosition = VESPERSBeamline::vespers()->sampleStageZ()->value();
		n = VESPERSBeamline::vespers()->sampleStageY()->value();
		break;

	case VESPERS::V:
		position = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		n = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->normalAxis()->translationMotor()->value();
		break;

	case VESPERS::Z:
		position = VESPERSBeamline::vespers()->sampleStageZ()->value();
		otherPosition = VESPERSBeamline::vespers()->sampleStageX()->value();
		n = VESPERSBeamline::vespers()->sampleStageY()->value();
		break;

	case VESPERS::AttoH:
		position = VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->verticalAxis()->translationMotor()->value();
		n = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->normalAxis()->translationMotor()->value();
		break;

	case VESPERS::AttoV:
		position = VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->verticalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		n = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->normalAxis()->translationMotor()->value();
		break;

	case VESPERS::AttoX:
		position = VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		n = VESPERSBeamline::vespers()->sampleStageY()->value();
		break;

	case VESPERS::AttoZ:
		position = VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->verticalAxis()->translationMotor()->value();
		n = VESPERSBeamline::vespers()->sampleStageY()->value();
		break;

	case VESPERS::AttoRz:
		position = VESPERSBeamline::vespers()->attoStageRz()->value();
		otherPosition = -123456789.0;
		n = 888888.88;
		break;

	case VESPERS::AttoRy:
		position = VESPERSBeamline::vespers()->attoStageRy()->value();
		otherPosition = -123456789.0;
		n = 888888.88;
		break;

	case VESPERS::AttoRx:
		position = VESPERSBeamline::vespers()->attoStageRx()->value();
		otherPosition = -123456789.0;
		n = 888888.88;
		break;

	case VESPERS::BigBeamX:
		position = VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->verticalAxis()->translationMotor()->value();
		n = 888888.88;
		break;

	case VESPERS::BigBeamZ:
		position = VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->verticalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		n = 888888.88;
		break;

	case VESPERS::WireH:
		position = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->verticalAxis()->translationMotor()->value();
		n = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->normalAxis()->translationMotor()->value();
		break;

	case VESPERS::WireV:
		position = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->verticalAxis()->translationMotor()->value();
		otherPosition = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->horizontalAxis()->translationMotor()->value();
		n = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->normalAxis()->translationMotor()->value();
		break;

	case VESPERS::WireN:
		position = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->normalAxis()->translationMotor()->value();
		otherPosition = -123456789.0;
		n = 888888.88;
		break;
	}

	configuration_->setEnd(position);
	end_->setValue(position);
	configuration_->setOtherPosition(otherPosition);
	configuration_->setNormalPosition(n);
	otherPosition_->setValue(otherPosition);
	updateMapInfo();
	axesAcceptable();
}

void VESPERSSpatialLineScanConfigurationView::onNormalPositionChanged()
{
	configuration_->setNormalPosition(normalPosition_->value());
	updateMapInfo();
}

void VESPERSSpatialLineScanConfigurationView::onStartChanged()
{
	configuration_->setStart(start_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERSSpatialLineScanConfigurationView::onEndChanged()
{
	configuration_->setEnd(end_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERSSpatialLineScanConfigurationView::onStepChanged()
{
	configuration_->setStep(step_->value()/1000);
	updateMapInfo();
	axesAcceptable();
}

void VESPERSSpatialLineScanConfigurationView::onDwellTimeChanged()
{
	configuration_->setTime(dwellTime_->value());
}

void VESPERSSpatialLineScanConfigurationView::onOtherPositionChanged()
{
	configuration_->setOtherPosition(otherPosition_->value());
}

void VESPERSSpatialLineScanConfigurationView::onSetNormalPosition()
{
	double n = 0;
	VESPERS::Motors motor = configuration_->motor();

	if (motor.testFlag(VESPERS::H) || motor.testFlag(VESPERS::V))
		n = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->normalAxis()->translationMotor()->value();

	if (motor.testFlag(VESPERS::X) || motor.testFlag(VESPERS::Z))
		n = VESPERSBeamline::vespers()->sampleStageY()->value();

	if (motor.testFlag(VESPERS::AttoH) || motor.testFlag(VESPERS::AttoV))
		n = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->normalAxis()->translationMotor()->value();  // focusing isn't done with attocube motors.

	if (motor.testFlag(VESPERS::AttoX) || motor.testFlag(VESPERS::AttoZ))
		n = VESPERSBeamline::vespers()->sampleStageY()->value();

	if (motor.testFlag(VESPERS::BigBeamX) || motor.testFlag(VESPERS::BigBeamZ))
		n = 888888.88;

	configuration_->setNormalPosition(n);
	updateMapInfo();
}

void VESPERSSpatialLineScanConfigurationView::updateMapInfo()
{
	double size = fabs(configuration_->end()-configuration_->start());
	int points = int(fabs(size/configuration_->step()));
	if ((size - (points + 0.01)*configuration_->step()) < 0)
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

	AMScanAxisRegion *region = configuration_->scanAxisAt(0)->regionAt(0);

	if (double(region->regionStart()) < double(region->regionEnd()) && double(region->regionStep()) > 0)
		step_->setPalette(good);

	else
		step_->setPalette(bad);
}

