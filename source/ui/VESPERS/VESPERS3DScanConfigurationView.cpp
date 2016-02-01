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


#include "VESPERS3DScanConfigurationView.h"

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

VESPERS3DScanConfigurationView::~VESPERS3DScanConfigurationView(){}

VESPERS3DScanConfigurationView::VESPERS3DScanConfigurationView(VESPERS3DScanConfiguration *config, QWidget *parent)
	: VESPERSScanConfigurationView(parent)
{
	configuration_ = config;
	AMTopFrame *frame = new AMTopFrame("VESPERS 3D Map Configuration");

	// Setup the group box for setting the start and end points.
	QGroupBox *positionsBox = new QGroupBox("Positions");

	hStart_ = createPositionDoubleSpinBox("H: ", " mm", configuration_->scanAxisAt(0)->regionAt(0)->regionStart(), 3);
	connect(hStart_, SIGNAL(editingFinished()), this, SLOT(onXStartChanged()));
	connect(configuration_->scanAxisAt(0)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(setXAxisStart(AMNumber)));

	vStart_ = createPositionDoubleSpinBox("V: ", " mm", configuration_->scanAxisAt(1)->regionAt(0)->regionStart(), 3);
	connect(vStart_, SIGNAL(editingFinished()), this, SLOT(onYStartChanged()));
	connect(configuration_->scanAxisAt(1)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(setYAxisStart(AMNumber)));

	QPushButton *startUseCurrentButton = new QPushButton("Use Current");
	connect(startUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetStartPosition()));

	QHBoxLayout *startPointLayout = new QHBoxLayout;
	startPointLayout->addWidget(new QLabel("Start:"));
	startPointLayout->addWidget(hStart_);
	startPointLayout->addWidget(vStart_);
	startPointLayout->addWidget(startUseCurrentButton);

	hEnd_ = createPositionDoubleSpinBox("H: ", " mm", configuration_->scanAxisAt(0)->regionAt(0)->regionEnd(), 3);
	connect(hEnd_, SIGNAL(editingFinished()), this, SLOT(onXEndChanged()));
	connect(configuration_->scanAxisAt(0)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(setXAxisEnd(AMNumber)));

	vEnd_ = createPositionDoubleSpinBox("V: ", " mm", configuration_->scanAxisAt(1)->regionAt(0)->regionEnd(), 3);
	connect(vEnd_, SIGNAL(editingFinished()), this, SLOT(onYEndChanged()));
	connect(configuration_->scanAxisAt(1)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(setYAxisEnd(AMNumber)));

	QPushButton *endUseCurrentButton = new QPushButton("Use Current");
	connect(endUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetEndPosition()));

	QHBoxLayout *endPointLayout = new QHBoxLayout;
	endPointLayout->addWidget(new QLabel("End:"));
	endPointLayout->addWidget(hEnd_);
	endPointLayout->addWidget(vEnd_);
	endPointLayout->addWidget(endUseCurrentButton);

	hStep_ = createPositionDoubleSpinBox("H: ", QString(" %1").arg(QString::fromUtf8("µm")), double(configuration_->scanAxisAt(0)->regionAt(0)->regionStep())*1000, 1);	// xStep needs to be in mm.
	connect(hStep_, SIGNAL(editingFinished()), this, SLOT(onXStepChanged()));
	connect(configuration_->scanAxisAt(0)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(setXAxisStep(AMNumber)));

	vStep_ = createPositionDoubleSpinBox("V: ", QString(" %1").arg(QString::fromUtf8("µm")), double(configuration_->scanAxisAt(1)->regionAt(0)->regionStep())*1000, 1);	// yStep needs to be in mm.
	connect(vStep_, SIGNAL(editingFinished()), this, SLOT(onYStepChanged()));
	connect(configuration_->scanAxisAt(1)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(setYAxisStep(AMNumber)));

	QHBoxLayout *stepSizeLayout = new QHBoxLayout;
	stepSizeLayout->addWidget(new QLabel("Step Size:"));
	stepSizeLayout->addWidget(hStep_);
	stepSizeLayout->addWidget(vStep_);
	stepSizeLayout->addStretch();

	wireStart_ = createPositionDoubleSpinBox("", " mm", double(configuration_->scanAxisAt(2)->regionAt(0)->regionStart()), 3);
	connect(wireStart_, SIGNAL(editingFinished()), this, SLOT(onWireStartChanged()));
	connect(configuration_->scanAxisAt(2)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(setWireAxisStart(AMNumber)));

	QPushButton *wireStartCurrentButton = new QPushButton("Use Current");
	connect(wireStartCurrentButton, SIGNAL(clicked()), this, SLOT(onSetWireStartPosition()));

	QHBoxLayout *wireStartLayout = new QHBoxLayout;
	wireStartLayout->addWidget(new QLabel("Wire Start:"));
	wireStartLayout->addWidget(wireStart_, 0, Qt::AlignCenter);
	wireStartLayout->addWidget(wireStartCurrentButton);

	wireEnd_ = createPositionDoubleSpinBox("", " mm", double(configuration_->scanAxisAt(2)->regionAt(0)->regionEnd()), 3);
	connect(wireEnd_, SIGNAL(editingFinished()), this, SLOT(onWireEndChanged()));
	connect(configuration_->scanAxisAt(2)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(setWireAxisEnd(AMNumber)));

	QPushButton *wireEndCurrentButton = new QPushButton("Use Current");
	connect(wireEndCurrentButton, SIGNAL(clicked()), this, SLOT(onSetWireEndPosition()));

	QHBoxLayout *wireEndLayout = new QHBoxLayout;
	wireEndLayout->addWidget(new QLabel("Wire End:"));
	wireEndLayout->addWidget(wireEnd_, 0, Qt::AlignCenter);
	wireEndLayout->addWidget(wireEndCurrentButton);

	wireStep_ = createPositionDoubleSpinBox("", QString(" %1").arg(QString::fromUtf8("µm")), double(configuration_->scanAxisAt(2)->regionAt(0)->regionStep())*1000, 1);	// yStep needs to be in mm.
	connect(wireStep_, SIGNAL(editingFinished()), this, SLOT(onWireStepChanged()));
	connect(configuration_->scanAxisAt(0)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(setWireAxisStep(AMNumber)));

	QHBoxLayout *wireStepLayout = new QHBoxLayout;
	wireStepLayout->addWidget(new QLabel("Wire Step:"));
	wireStepLayout->addWidget(wireStep_);

	mapInfo_ = new QLabel;
	updateMapInfo();

	QVBoxLayout *positionsLayout = new QVBoxLayout;
	positionsLayout->addLayout(startPointLayout);
	positionsLayout->addLayout(endPointLayout);
	positionsLayout->addLayout(stepSizeLayout);
	positionsLayout->addLayout(wireStartLayout);
	positionsLayout->addLayout(wireEndLayout);
	positionsLayout->addLayout(wireStepLayout);
	positionsLayout->addWidget(mapInfo_);

	positionsBox->setLayout(positionsLayout);

	// Dwell time.
	dwellTime_ = createDwellTimeSpinBox(configuration_->scanAxisAt(0)->regionAt(0)->regionTime());
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));
	connect(configuration_->scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(setDwellTime(AMNumber)));
	connect(configuration_->scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), configuration_->scanAxisAt(1)->regionAt(0), SLOT(setRegionTime(AMNumber)));
	connect(configuration_->scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), configuration_->scanAxisAt(2)->regionAt(0), SLOT(setRegionTime(AMNumber)));

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
	i0ComboBox_ = createIonChamberComboBox();
	connect(i0ComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onI0Clicked(int)));
	connect(configuration_->dbObject(), SIGNAL(incomingChoiceChanged(int)), this, SLOT(updateI0ComboBox(int)));

	// Motor selection.
	motorSelectionComboBox_ = createMotorSelectionComboBox(QStringList() << "H & V", QList<int>() << (VESPERS::H | VESPERS::V));
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
	QGroupBox *autoExportGroupBox = addExporterOptionsView(QStringList() << "Ascii" << "SMAK", configuration_->exportSpectraSources(), configuration_->exportSpectraInRows());
	connect(autoExportButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(updateAutoExporter(int)));
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), configuration_, SLOT(setExportSpectraSources(bool)));
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), exportSpectraInRows_, SLOT(setEnabled(bool)));
	connect(exportSpectraInRows_, SIGNAL(toggled(bool)), this, SLOT(updateExportSpectraInRows(bool)));
	autoExportButtonGroup_->button(configuration_->exportAsAscii() ? 0 : 1)->click();

	ccdComboBox_->setCurrentIndex(ccdComboBox_->findData(int(configuration_->ccdDetector())));
	i0ComboBox_->setCurrentIndex((int)configuration_->incomingChoice());
	fluorescenceDetectorComboBox_->setCurrentIndex((int)configuration_->fluorescenceDetector());
	motorSelectionComboBox_->setCurrentIndex(motorSelectionComboBox_->findData(int(configuration_->motor())));

	disableStandardFluorescenceOptions();
	disableStandardI0Options();
	disableStandardXRDOptions();

	QFormLayout *detectorLayout = new QFormLayout;
	detectorLayout->addRow("XRF:", fluorescenceDetectorComboBox_);
	detectorLayout->addRow("XRD:", ccdComboBox_);
	detectorLayout->addRow("I0:", i0ComboBox_);
	detectorLayout->addRow("Stage:", motorSelectionComboBox_);

	QGroupBox *detectorGroupBox = new QGroupBox("Detectors");
	detectorGroupBox->setLayout(detectorLayout);

	// Setting up the layout.
	QGridLayout *contentsLayout = new QGridLayout;
	contentsLayout->addWidget(positionsBox, 0, 0, 2, 3);
	contentsLayout->addWidget(timeGroupBox, 2, 0, 1, 3);
	contentsLayout->addWidget(scanNameGroupBox, 3, 0, 1, 3);
	contentsLayout->addWidget(timeOffsetBox, 5, 0, 1, 3);
	contentsLayout->addWidget(detectorGroupBox, 0, 3, 1, 1);
	contentsLayout->addWidget(autoExportGroupBox, 1, 3, 2, 1);
	contentsLayout->addWidget(ccdTextBox_, 3, 3, 1, 1);

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

void VESPERS3DScanConfigurationView::onScanNameEdited()
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

void VESPERS3DScanConfigurationView::checkCCDFileNames(const QString &name) const
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

void VESPERS3DScanConfigurationView::onFluorescenceChoiceChanged(int id)
{
	configuration_->setFluorescenceDetector(id);
}

void VESPERS3DScanConfigurationView::onMotorChanged(int id)
{
	configuration_->setMotor(motorSelectionComboBox_->itemData(id).toInt());
}

void VESPERS3DScanConfigurationView::onCCDDetectorChanged(int id)
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

void VESPERS3DScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + AMDateTimeUtils::convertTimeToString(configuration_->totalTime()));
}

void VESPERS3DScanConfigurationView::onSetStartPosition()
{
	double h = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalAxis()->translationMotor()->value();
	double v = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalAxis()->translationMotor()->value();
	double n = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->normalAxis()->translationMotor()->value();

	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStart(h);
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStart(v);
	configuration_->setNormalPosition(n);
	hStart_->setValue(h);
	vStart_->setValue(v);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onSetEndPosition()
{
	double h = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalAxis()->translationMotor()->value();
	double v = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalAxis()->translationMotor()->value();

	configuration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(h);
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(v);
	hEnd_->setValue(h);
	vEnd_->setValue(v);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onSetWireStartPosition()
{
	double wire = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->verticalAxis()->translationMotor()->value();

	configuration_->scanAxisAt(2)->regionAt(0)->setRegionStart(wire);
	wireStart_->setValue(wire);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onSetWireEndPosition()
{
	double wire = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->verticalAxis()->translationMotor()->value();

	configuration_->scanAxisAt(2)->regionAt(0)->setRegionEnd(wire);
	wireEnd_->setValue(wire);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onXStartChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStart(hStart_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onXEndChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(hEnd_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onXStepChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStep(hStep_->value()/1000);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onYStartChanged()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStart(vStart_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onYEndChanged()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(vEnd_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onYStepChanged()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStep(vStep_->value()/1000);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onWireStartChanged()
{
	configuration_->scanAxisAt(2)->regionAt(0)->setRegionStart(wireStart_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onWireEndChanged()
{
	configuration_->scanAxisAt(2)->regionAt(0)->setRegionEnd(wireEnd_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onWireStepChanged()
{
	configuration_->scanAxisAt(2)->regionAt(0)->setRegionStep(wireStep_->value()/1000);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onDwellTimeChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionTime(dwellTime_->value());
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionTime(dwellTime_->value());
	configuration_->scanAxisAt(2)->regionAt(0)->setRegionTime(dwellTime_->value());
}

void VESPERS3DScanConfigurationView::updateMapInfo()
{
	double hSize = fabs(double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()));
	double vSize = fabs(double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(1)->regionAt(0)->regionStart()));
	double wireSize = fabs(double(configuration_->scanAxisAt(2)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(2)->regionAt(0)->regionStart()));

	int hPoints = configuration_->scanAxisAt(0)->numberOfPoints();
	int vPoints = configuration_->scanAxisAt(1)->numberOfPoints();
	int wirePoints = configuration_->scanAxisAt(2)->numberOfPoints();

	mapInfo_->setText(QString("Map Size: %1 %2 x %3 %2 x %6 %2\t Points: %4 x %5 x %7")
					  .arg(QString::number(hSize*1000, 'f', 1))
					  .arg(QString::fromUtf8("µm"))
					  .arg(QString::number(vSize*1000, 'f', 1))
					  .arg(hPoints)
					  .arg(vPoints)
					  .arg(QString::number(wireSize*1000, 'f', 1))
					  .arg(wirePoints)
					  );
}

void VESPERS3DScanConfigurationView::axesAcceptable()
{
	QPalette good(this->palette());
	QPalette bad(good);
	bad.setColor(QPalette::Base, Qt::red);

	/*
	hStep_->setPalette(configuration_->validXAxis() ? good : bad);
	vStep_->setPalette(configuration_->validYAxis() ? good : bad);
	wireStep_->setPalette(configuration_->validZAxis() ? good : bad);
	*/
}

void VESPERS3DScanConfigurationView::setXAxisStart(const AMNumber &value)
{
	hStart_->setValue(double(value));
}

void VESPERS3DScanConfigurationView::setYAxisStart(const AMNumber &value)
{
	vStart_->setValue(double(value));
}

void VESPERS3DScanConfigurationView::setWireAxisStart(const AMNumber &value)
{
	wireStart_->setValue(double(value));
}

void VESPERS3DScanConfigurationView::setXAxisStep(const AMNumber &value)
{
	hStep_->setValue(double(value)*1000);
}

void VESPERS3DScanConfigurationView::setYAxisStep(const AMNumber &value)
{
	vStep_->setValue(double(value)*1000);
}

void VESPERS3DScanConfigurationView::setWireAxisStep(const AMNumber &value)
{
	wireStep_->setValue(double(value)*1000);
}

void VESPERS3DScanConfigurationView::setXAxisEnd(const AMNumber &value)
{
	hEnd_->setValue(double(value));
}

void VESPERS3DScanConfigurationView::setYAxisEnd(const AMNumber &value)
{
	vEnd_->setValue(double(value));
}

void VESPERS3DScanConfigurationView::setWireAxisEnd(const AMNumber &value)
{
	wireEnd_->setValue(double(value));
}

void VESPERS3DScanConfigurationView::setDwellTime(const AMNumber &value)
{
	dwellTime_->setValue(double(value));
}

