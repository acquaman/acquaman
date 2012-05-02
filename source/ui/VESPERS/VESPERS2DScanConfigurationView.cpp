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
	: AMScanConfigurationView(parent)
{
	config_ = config;
	AMTopFrame *frame = new AMTopFrame("VESPERS 2D Map Configuration");

	// Setup the group box for setting the start and end points.
	QGroupBox *positionsBox = new QGroupBox("Positions");

	hStart_ = new QDoubleSpinBox;
	hStart_->setPrefix("H: ");
	hStart_->setRange(-1000000, 1000000);
	hStart_->setSuffix(" mm");
	hStart_->setValue(config_->xStart());
	hStart_->setDecimals(3);
	hStart_->setAlignment(Qt::AlignCenter);
	connect(hStart_, SIGNAL(editingFinished()), this, SLOT(onXStartChanged()));
	connect(config_, SIGNAL(xStartChanged(double)), hStart_, SLOT(setValue(double)));

	vStart_ = new QDoubleSpinBox;
	vStart_->setPrefix("V: ");
	vStart_->setRange(-1000000, 1000000);
	vStart_->setSuffix(" mm");
	vStart_->setValue(config_->yStart());
	vStart_->setDecimals(3);
	vStart_->setAlignment(Qt::AlignCenter);
	connect(vStart_, SIGNAL(editingFinished()), this, SLOT(onYStartChanged()));
	connect(config_, SIGNAL(yStartChanged(double)), vStart_, SLOT(setValue(double)));

	QPushButton *startUseCurrentButton = new QPushButton("Use Current");
	connect(startUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetStartPosition()));

	QHBoxLayout *startPointLayout = new QHBoxLayout;
	startPointLayout->addWidget(new QLabel("Start:"));
	startPointLayout->addWidget(hStart_);
	startPointLayout->addWidget(vStart_);
	startPointLayout->addWidget(startUseCurrentButton);

	hEnd_ = new QDoubleSpinBox;
	hEnd_->setPrefix("H: ");
	hEnd_->setRange(-1000000, 1000000);
	hEnd_->setSuffix(" mm");
	hEnd_->setValue(config_->xEnd());
	hEnd_->setDecimals(3);
	hEnd_->setAlignment(Qt::AlignCenter);
	connect(hEnd_, SIGNAL(editingFinished()), this, SLOT(onXEndChanged()));
	connect(config_, SIGNAL(xEndChanged(double)), hEnd_, SLOT(setValue(double)));

	vEnd_ = new QDoubleSpinBox;
	vEnd_->setPrefix("V: ");
	vEnd_->setRange(-1000000, 1000000);
	vEnd_->setSuffix(" mm");
	vEnd_->setValue(config_->yEnd());
	vEnd_->setDecimals(3);
	vEnd_->setAlignment(Qt::AlignCenter);
	connect(vEnd_, SIGNAL(editingFinished()), this, SLOT(onYEndChanged()));
	connect(config_, SIGNAL(yEndChanged(double)), vEnd_, SLOT(setValue(double)));

	QPushButton *endUseCurrentButton = new QPushButton("Use Current");
	connect(endUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetEndPosition()));

	QHBoxLayout *endPointLayout = new QHBoxLayout;
	endPointLayout->addWidget(new QLabel("End:"));
	endPointLayout->addWidget(hEnd_);
	endPointLayout->addWidget(vEnd_);
	endPointLayout->addWidget(endUseCurrentButton);

	hStep_ = new QDoubleSpinBox;
	hStep_->setPrefix("H: ");
	hStep_->setRange(0, 1000000);
	hStep_->setSuffix(QString(" %1").arg(QString::fromUtf8("µm")));
	hStep_->setDecimals(1);
	hStep_->setAlignment(Qt::AlignCenter);
	hStep_->setValue(config_->xStep()*1000);		// xStep needs to be in mm.
	connect(hStep_, SIGNAL(editingFinished()), this, SLOT(onXStepChanged()));
	connect(config_, SIGNAL(xStepChanged(double)), this, SLOT(updateXStep(double)));

	vStep_ = new QDoubleSpinBox;
	vStep_->setPrefix("V: ");
	vStep_->setRange(0, 1000000);
	vStep_->setSuffix(QString(" %1").arg(QString::fromUtf8("µm")));
	vStep_->setDecimals(1);
	vStep_->setAlignment(Qt::AlignCenter);
	vStep_->setValue(config_->yStep()*1000);		// yStep needs to be in mm.
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
	dwellTime_ = new QDoubleSpinBox;
	dwellTime_->setRange(0, 1000000);
	dwellTime_->setValue(config_->timeStep());
	dwellTime_->setSuffix(" s");
	dwellTime_->setAlignment(Qt::AlignCenter);
	dwellTime_->setDecimals(1);
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));
	connect(config_, SIGNAL(timeStepChanged(double)), dwellTime_, SLOT(setValue(double)));

	QHBoxLayout *timeLayout = new QHBoxLayout;
	timeLayout->addWidget(new QLabel("Dwell Time:"));
	timeLayout->addWidget(dwellTime_);

	// Using the CCD.
	QGroupBox *ccdBox = new QGroupBox("XRD maps");

	usingCCDCheckBox_ = new QCheckBox("Do XRD simultaneously");
	usingCCDCheckBox_->setChecked(config_->usingCCD());
	connect(config_, SIGNAL(usingCCDChanged(bool)), this, SLOT(onUsingCCDChanged(bool)));
	connect(usingCCDCheckBox_, SIGNAL(toggled(bool)), config_, SLOT(setUsingCCD(bool)));

	currentCCDFileName_ = new QLabel;
	onCCDFileNameChanged(config_->ccdFileName());
	currentCCDFileName_->setVisible(config_->usingCCD());

	QPushButton *configureRoperDetectorButton = new QPushButton(QIcon(":/hammer-wrench.png"), "Configure Roper CCD");
	configureRoperDetectorButton->setEnabled(config_->usingCCD());
	connect(configureRoperDetectorButton, SIGNAL(clicked()), this, SLOT(onConfigureRoperDetectorClicked()));
	connect(usingCCDCheckBox_, SIGNAL(toggled(bool)), configureRoperDetectorButton, SLOT(setEnabled(bool)));

	QHBoxLayout *ccdBoxFirstRowLayout = new QHBoxLayout;
	ccdBoxFirstRowLayout->addWidget(usingCCDCheckBox_);
	ccdBoxFirstRowLayout->addWidget(configureRoperDetectorButton);

	QVBoxLayout *ccdBoxLayout = new QVBoxLayout;
	ccdBoxLayout->addLayout(ccdBoxFirstRowLayout);
	ccdBoxLayout->addWidget(currentCCDFileName_);

	ccdBox->setLayout(ccdBoxLayout);

	// The fluorescence detector setup
	fluorescenceButtonGroup_ = new QButtonGroup;
	QRadioButton *tempButton;
	QVBoxLayout *fluorescenceDetectorLayout = new QVBoxLayout;

	tempButton = new QRadioButton("Single Element Vortex");
	fluorescenceButtonGroup_->addButton(tempButton, 1);
	fluorescenceDetectorLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Four Element Vortex");
	fluorescenceButtonGroup_->addButton(tempButton, 2);
	fluorescenceDetectorLayout->addWidget(tempButton);

	connect(fluorescenceButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onFluorescenceChoiceChanged(int)));
	connect(config_, SIGNAL(fluorescenceDetectorChoiceChanged(int)), this, SLOT(updateFluorescenceDetector(int)));

	fluorescenceButtonGroup_->button((int)config_->fluorescenceDetectorChoice())->setChecked(true);

	QGroupBox *fluorescenceDetectorGroupBox = new QGroupBox("Fluorescence Detector");
	fluorescenceDetectorGroupBox->setLayout(fluorescenceDetectorLayout);

	// Ion chamber selection
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

	connect(I0Group_, SIGNAL(buttonClicked(int)), this, SLOT(onI0Clicked(int)));
	connect(config_, SIGNAL(incomingChoiceChanged(int)), this, SLOT(updateI0Buttons(int)));

	I0Group_->button((int)config_->incomingChoice())->click();
	QGroupBox *I0GroupBox = new QGroupBox("I0");
	I0GroupBox->setLayout(I0GroupLayout);

	// Motor selection.
	QGroupBox *motorSetChoiceBox = new QGroupBox("Motors Selection");
	QVBoxLayout *motorChoiceLayout = new QVBoxLayout;
	motorChoiceButtonGroup_ = new QButtonGroup;

	tempButton = new QRadioButton("H and V");
	motorChoiceButtonGroup_->addButton(tempButton, 0);
	motorChoiceLayout->addWidget(tempButton);
	tempButton = new QRadioButton("X and Z");
	motorChoiceButtonGroup_->addButton(tempButton, 1);
	motorChoiceLayout->addWidget(tempButton);

	connect(motorChoiceButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onMotorsChoiceChanged(int)));

	motorChoiceButtonGroup_->button(int(config_->motorsChoice()))->click();
	motorSetChoiceBox->setLayout(motorChoiceLayout);

	// Scan name selection
	scanName_ = new QLineEdit;
	scanName_->setText(config_->name());
	scanName_->setAlignment(Qt::AlignCenter);
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
	QLabel *exportPath = new QLabel(QString("Data exported to: %1exportData").arg(AMUserSettings::userDataFolder));

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
	contentsLayout->addLayout(timeOffsetLayout, 5, 0, 1, 1);
	contentsLayout->addWidget(motorSetChoiceBox, 0, 3);

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
	config_->setFluorescenceDetectorChoice(id);

	switch(id){

	case 1:
		config_->setRoiInfoList(*VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList());
		break;

	case 2:
		config_->setRoiInfoList(*VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList());
		break;
	}

	updateRoiText();
}

void VESPERS2DScanConfigurationView::onMotorsChoiceChanged(int id)
{
	config_->setMotorsChoice(id);
}

void VESPERS2DScanConfigurationView::onConfigureXRFDetectorClicked()
{
	switch(config_->fluorescenceDetectorChoice()){

	case VESPERS2DScanConfiguration::None:
		break;

	case VESPERS2DScanConfiguration::SingleElement:
		emit configureDetector("Single Element");
		break;

	case VESPERS2DScanConfiguration::FourElement:
		emit configureDetector("Four Element");
		break;
	}
}

void VESPERS2DScanConfigurationView::onConfigureRoperDetectorClicked()
{
	emit configureDetector("Roper CCD");
}

void VESPERS2DScanConfigurationView::onUsingCCDChanged(bool useCCD)
{
	usingCCDCheckBox_->setChecked(useCCD);

	if (useCCD){

		connect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(ccdNameChanged(QString)), config_, SLOT(setCCDFileName(QString)));
		config_->setCCDFileName(VESPERSBeamline::vespers()->roperCCD()->ccdFileName());
		onCCDFileNameChanged(VESPERSBeamline::vespers()->roperCCD()->ccdFileName());
		currentCCDFileName_->show();
	}
	else {

		disconnect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(ccdNameChanged(QString)), config_, SLOT(setCCDFileName(QString)));
		config_->setCCDFileName("");
		onCCDFileNameChanged("");
		currentCCDFileName_->hide();
	}
}

void VESPERS2DScanConfigurationView::updateRoiText()
{
	switch(config_->fluorescenceDetectorChoice()){

	case VESPERS2DScanConfiguration::None:
		break;

	case VESPERS2DScanConfiguration::SingleElement:
		config_->setRoiInfoList(*VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList());
		break;

	case VESPERS2DScanConfiguration::FourElement:
		config_->setRoiInfoList(*VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList());
		break;
	}

	roiText_->clear();
	roiText_->insertPlainText("Name\tLow (eV)\tHigh (eV)\n");

	for (int i = 0; i < config_->roiList().count(); i++)
		roiText_->insertPlainText(GeneralUtilities::addGreek(config_->roiList().at(i).name())+"\t" + QString::number(config_->roiList().at(i).low()) + "\t" + QString::number(config_->roiList().at(i).high()) +"\n");
}

void VESPERS2DScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + convertTimeToString(config_->totalTime()));
}

QString VESPERS2DScanConfigurationView::convertTimeToString(double time)
{
	QString timeString;

	int days = int(time/3600.0/24.0);

	if (days > 0){

		time -= time/3600/24;
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
	double h = VESPERSBeamline::vespers()->pseudoSampleStage()->horiz()->value();
	double v = VESPERSBeamline::vespers()->pseudoSampleStage()->vert()->value();

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

	if (config_->motorsChoice() == VESPERS2DScanConfiguration::HAndV){

		h = VESPERSBeamline::vespers()->pseudoSampleStage()->horiz()->value();
		v = VESPERSBeamline::vespers()->pseudoSampleStage()->vert()->value();
	}
	else if (config_->motorsChoice() == VESPERS2DScanConfiguration::XAndZ){

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
	int hPoints = int(fabs(hSize/config_->xStep()));
	int vPoints = int(fabs(vSize/config_->yStep()));

	if (hPoints < 0)
		hPoints = 0;
	else
		hPoints++;

	if (vPoints < 0)
		vPoints = 0;
	else
		vPoints++;

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
