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

	normalPosition_ = buildPositionDoubleSpinBox("N: ", " mm", config_->normalPosition(), 3);
	connect(normalPosition_, SIGNAL(editingFinished()), this, SLOT(onNormalPositionChanged()));
	connect(config_->dbObject(), SIGNAL(normalPositionChanged(double)), normalPosition_, SLOT(setValue(double)));

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
	positionsLayout->addLayout(normalLayout);
	positionsLayout->addWidget(mapInfo_);

	positionsBox->setLayout(positionsLayout);

	// Dwell time.
	dwellTime_ = addDwellTimeWidget(config_->timeStep());
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));
	connect(config_, SIGNAL(timeStepChanged(double)), dwellTime_, SLOT(setValue(double)));

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
	QGroupBox *ccdBox = addCCDDetectorSelectionView();
	connect(ccdButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onCCDDetectorChanged(int)));
	connect(config_->dbObject(), SIGNAL(ccdDetectorChanged(int)), this, SLOT(updateCCDDetectorButtons(int)));
	ccdButtonGroup_->button(int(config_->ccdDetector()))->setChecked(true);
	ccdButtonGroup_->button((int)VESPERS::Roper)->hide();
	ccdButtonGroup_->button((int)VESPERS::Mar)->hide();

	configureCCDButton_ = new QPushButton(QIcon(":/hammer-wrench.png"), "Configure Area Detector");
	configureCCDButton_->setEnabled(config_->ccdDetector());
	connect(configureCCDButton_, SIGNAL(clicked()), this, SLOT(onConfigureCCDDetectorClicked()));

	// The fluorescence detector setup
	QGroupBox *fluorescenceDetectorGroupBox  = addFluorescenceDetectorSelectionView();
	fluorescenceButtonGroup_->button(int(VESPERS::NoXRF))->setDisabled(true);
	connect(fluorescenceButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onFluorescenceChoiceChanged(int)));
	connect(config_->dbObject(), SIGNAL(fluorescenceDetectorChanged(int)), this, SLOT(updateFluorescenceDetector(int)));
	fluorescenceButtonGroup_->button((int)config_->fluorescenceDetector())->setChecked(true);

	// Ion chamber selection
	QGroupBox *I0GroupBox = addI0SelectionView();
	connect(I0Group_, SIGNAL(buttonClicked(int)), this, SLOT(onI0Clicked(int)));
	connect(config_->dbObject(), SIGNAL(incomingChoiceChanged(int)), this, SLOT(updateI0Buttons(int)));
	I0Group_->button((int)config_->incomingChoice())->click();

	// Motor selection.
	QGroupBox *motorSetChoiceBox = addMotorSelectionView(
				QStringList() << "H && V" << "X && Z" << "Atto H && V" << "Atto X && Z" << "Big Beam X && Z",
				QList<int>() << (VESPERS::H | VESPERS::V) << (VESPERS::X | VESPERS::Z) << (VESPERS::AttoH | VESPERS::AttoV) << (VESPERS::AttoX | VESPERS::AttoZ) << (VESPERS::BigBeamX | VESPERS::BigBeamZ));
	connect(motorButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onMotorChanged(int)));
	connect(config_->dbObject(), SIGNAL(motorChanged(int)), this, SLOT(onMotorUpdated(int)));
	motorButtonGroup_->button(int(config_->motor()))->click();

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
	// Only connecting this signal because it is the only CCD available currently.  It would need some logic for switching which CCD it was actually connected to.
	connect(VESPERSBeamline::vespers()->pilatusCCD(), SIGNAL(ccdPathChanged(QString)), this, SLOT(onScanNameEdited()));
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
	QGroupBox *autoExportGroupBox = addExporterOptionsView(QStringList() << "Ascii" << "SMAK", config_->exportSpectraSources(), config_->exportSpectraInRows());
	connect(autoExportButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(updateAutoExporter(int)));
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), config_, SLOT(setExportSpectraSources(bool)));
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), exportSpectraInRows_, SLOT(setEnabled(bool)));
	connect(exportSpectraInRows_, SIGNAL(toggled(bool)), this, SLOT(updateExportSpectraInRows(bool)));
	autoExportButtonGroup_->button(config_->exportAsAscii() ? 0 : 1)->click();

	// Auto-export option.
	QVBoxLayout *autoExportLayout = new QVBoxLayout;
	QButtonGroup *autoExportButtonGroup = new QButtonGroup;

	QRadioButton *autoExportButton = new QRadioButton("Ascii");
	autoExportButtonGroup->addButton(autoExportButton, 0);
	autoExportLayout->addWidget(autoExportButton);

	autoExportButton = new QRadioButton("SMAK");
	autoExportButtonGroup->addButton(autoExportButton, 1);
	autoExportLayout->addWidget(autoExportButton);

	connect(autoExportButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(updateAutoExporter(int)));

	autoExportButtonGroup->button(config_->exportAsAscii() ? 0 : 1)->click();
	autoExportGroupBox->setLayout(autoExportLayout);

	// Setting up the layout.
	QGridLayout *contentsLayout = new QGridLayout;
	contentsLayout->addWidget(positionsBox, 0, 0, 2, 3);
	contentsLayout->addWidget(timeGroupBox, 2, 0, 1, 1);
	contentsLayout->addWidget(ccdBox, 3, 0, 1, 1);
	contentsLayout->addLayout(scanNameLayout, 4, 0, 1, 1);
	contentsLayout->addWidget(timeOffsetBox, 5, 0, 1, 1);
	contentsLayout->addWidget(configureCCDButton_, 6, 0, 1, 1);
	contentsLayout->addWidget(motorSetChoiceBox, 0, 3, 1, 1);
	contentsLayout->addWidget(fluorescenceDetectorGroupBox, 1, 3, 2, 1);
	contentsLayout->addWidget(I0GroupBox, 3, 3, 2, 1);
	contentsLayout->addWidget(ccdTextBox_, 7, 0, 1, 6);
	contentsLayout->addWidget(roiTextBox, 0, 5, 3, 3);
	contentsLayout->addWidget(autoExportGroupBox, 3, 5, 2, 3);

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

void VESPERS2DScanConfigurationView::onScanNameEdited()
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

	if (config_->ccdDetector() == VESPERS::Pilatus && name.contains(" ")){

		QPalette palette = scanName_->palette();
		palette.setColor(QPalette::Base, Qt::red);
		scanName_->setPalette(palette);
	}

	else
		scanName_->setPalette(this->palette());
}

void VESPERS2DScanConfigurationView::checkCCDFileNames(const QString &name) const
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
		ccdHelpText_->setText(QString("The scan name you have chosen conflicts with existing CCD file names.\nIf you don't change the name a random suffix will be added to avoid name conflicts.\neg. %1").arg(VESPERS::appendUniqueIdentifier(name)));
	}

	else{

		ccdHelpText_->setText("");
		ccdHelpText_->hide();
	}
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

void VESPERS2DScanConfigurationView::onMotorUpdated(int id)
{
	motorButtonGroup_->button(id)->setChecked(true);
}

void VESPERS2DScanConfigurationView::onCCDDetectorChanged(int id)
{
	config_->setCCDDetector(id);

	if (config_->ccdDetector() != VESPERS::NoCCD){

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

	onScanNameEdited();
	ccdTextBox_->setVisible(config_->ccdDetector() != VESPERS::NoCCD);
	configureCCDButton_->setDisabled(config_->ccdDetector() == VESPERS::NoCCD);
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

void VESPERS2DScanConfigurationView::onSetStartPosition()
{
	double h = 0;
	double v = 0;
	double n = 0;

	switch(int(config_->motor())){

	case VESPERS::H | VESPERS::V:

		h = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalControl()->value();
		v = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalControl()->value();
		n = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->normalControl()->value();
		break;

	case VESPERS::X | VESPERS::Z:

		h = VESPERSBeamline::vespers()->sampleStageX()->value();
		v = VESPERSBeamline::vespers()->sampleStageZ()->value();
		n = VESPERSBeamline::vespers()->sampleStageY()->value();
		break;

	case VESPERS::AttoH | VESPERS::AttoV:

		h = VESPERSBeamline::vespers()->attoStageHorizontal()->value();
		v = VESPERSBeamline::vespers()->attoStageVertical()->value();
		n = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->normalControl()->value();  // focusing isn't done with attocube motors.
		break;

	case VESPERS::AttoX | VESPERS::AttoZ:

		h = VESPERSBeamline::vespers()->attoStageX()->value();
		v = VESPERSBeamline::vespers()->attoStageZ()->value();
		n = VESPERSBeamline::vespers()->sampleStageY()->value();
		break;

	case VESPERS::BigBeamX | VESPERS::BigBeamZ:

		h = VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->horizontalControl()->value();
		v = VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->verticalControl()->value();
		n = 888888.88;
		break;
	}

	config_->setXStart(h);
	hStart_->setValue(h);
	config_->setYStart(v);
	vStart_->setValue(v);
	config_->setNormalPosition(n);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS2DScanConfigurationView::onSetEndPosition()
{
	double h = 0;
	double v = 0;

	switch(int(config_->motor())){

	case VESPERS::H | VESPERS::V:

		h = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalControl()->value();
		v = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalControl()->value();
		break;

	case VESPERS::X | VESPERS::Z:

		h = VESPERSBeamline::vespers()->sampleStageX()->value();
		v = VESPERSBeamline::vespers()->sampleStageZ()->value();
		break;

	case VESPERS::AttoH | VESPERS::AttoV:

		h = VESPERSBeamline::vespers()->attoStageHorizontal()->value();
		v = VESPERSBeamline::vespers()->attoStageVertical()->value();
		break;

	case VESPERS::AttoX | VESPERS::AttoZ:

		h = VESPERSBeamline::vespers()->attoStageX()->value();
		v = VESPERSBeamline::vespers()->attoStageZ()->value();
		break;

	case VESPERS::BigBeamX | VESPERS::BigBeamZ:

		h = VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->horizontalControl()->value();
		v = VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->verticalControl()->value();
		break;
	}

	config_->setXEnd(h);
	hEnd_->setValue(h);
	config_->setYEnd(v);
	vEnd_->setValue(v);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS2DScanConfigurationView::onSetNormalPosition()
{
	double n = 0;

	switch(int(config_->motor())){

	case VESPERS::H | VESPERS::V:

		n = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->normalControl()->value();
		break;

	case VESPERS::X | VESPERS::Z:

		n = VESPERSBeamline::vespers()->sampleStageY()->value();
		break;

	case VESPERS::AttoH | VESPERS::AttoV:

		n = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->normalControl()->value();  // focusing isn't done with attocube motors.
		break;

	case VESPERS::AttoX | VESPERS::AttoZ:

		n = VESPERSBeamline::vespers()->sampleStageY()->value();
		break;

	case VESPERS::BigBeamX | VESPERS::BigBeamZ:

		n = 888888.88;
		break;
	}

	config_->setNormalPosition(n);
	updateMapInfo();
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

void VESPERS2DScanConfigurationView::onNormalPositionChanged()
{
	config_->setNormalPosition(normalPosition_->value());
	updateMapInfo();
}

void VESPERS2DScanConfigurationView::onDwellTimeChanged()
{
	config_->setTimeStep(dwellTime_->value());
}

void VESPERS2DScanConfigurationView::updateMapInfo()
{
	double hSize = fabs(config_->xEnd()-config_->xStart());
	double vSize = fabs(config_->yEnd()-config_->yStart());

	int hPoints = int((hSize)/config_->xStep());
	if ((hSize - (hPoints + 0.01)*config_->xStep()) < 0)
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
