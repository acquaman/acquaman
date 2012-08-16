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
	: AMScanConfigurationView(parent)
{
	config_ = config;
	AMTopFrame *frame = new AMTopFrame("VESPERS Line Scan Configuration");

	// Setup the group box for setting the start and end points.
	QGroupBox *positionsBox = new QGroupBox("Positions");

	start_ = new QDoubleSpinBox;
	start_->setRange(-1000000, 1000000);
	start_->setSuffix(" mm");
	start_->setValue(config_->start());
	start_->setDecimals(3);
	start_->setAlignment(Qt::AlignCenter);
	connect(start_, SIGNAL(editingFinished()), this, SLOT(onStartChanged()));
	connect(config_, SIGNAL(startChanged(double)), start_, SLOT(setValue(double)));

	QPushButton *startUseCurrentButton = new QPushButton("Use Current");
	connect(startUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetStartPosition()));

	QHBoxLayout *startPointLayout = new QHBoxLayout;
	startPointLayout->addWidget(new QLabel("Start:"));
	startPointLayout->addWidget(start_);
	startPointLayout->addWidget(startUseCurrentButton);

	end_ = new QDoubleSpinBox;
	end_->setRange(-1000000, 1000000);
	end_->setSuffix(" mm");
	end_->setValue(config_->end());
	end_->setDecimals(3);
	end_->setAlignment(Qt::AlignCenter);
	connect(end_, SIGNAL(editingFinished()), this, SLOT(onEndChanged()));
	connect(config_, SIGNAL(endChanged(double)), end_, SLOT(setValue(double)));

	QPushButton *endUseCurrentButton = new QPushButton("Use Current");
	connect(endUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetEndPosition()));

	QHBoxLayout *endPointLayout = new QHBoxLayout;
	endPointLayout->addWidget(new QLabel("End:"));
	endPointLayout->addWidget(end_);
	endPointLayout->addWidget(endUseCurrentButton);

	step_ = new QDoubleSpinBox;
	step_->setRange(0, 1000000);
	step_->setSuffix(QString(" %1").arg(QString::fromUtf8("µm")));
	step_->setDecimals(1);
	step_->setAlignment(Qt::AlignCenter);
	step_->setValue(config_->step()*1000);		// xStep needs to be in mm.
	connect(step_, SIGNAL(editingFinished()), this, SLOT(onStepChanged()));
	connect(config_, SIGNAL(stepChanged(double)), this, SLOT(updateStep(double)));

	QHBoxLayout *stepSizeLayout = new QHBoxLayout;
	stepSizeLayout->addWidget(new QLabel("Step Size:"));
	stepSizeLayout->addWidget(step_);
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
	dwellTime_->setValue(config_->time());
	dwellTime_->setSuffix(" s");
	dwellTime_->setAlignment(Qt::AlignCenter);
	dwellTime_->setDecimals(1);
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));
	connect(config_, SIGNAL(timeChanged(double)), dwellTime_, SLOT(setValue(double)));

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
	connect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(ccdNameChanged(QString)), this, SLOT(onCCDFileNameChanged(QString)));

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
	QGroupBox *motorSetChoiceBox = new QGroupBox("Motor");
	QVBoxLayout *motorChoiceLayout = new QVBoxLayout;
	motorChoiceButtonGroup_ = new QButtonGroup;

	tempButton = new QRadioButton("H");
	motorChoiceButtonGroup_->addButton(tempButton, 0);
	motorChoiceLayout->addWidget(tempButton);
	tempButton = new QRadioButton("X");
	motorChoiceButtonGroup_->addButton(tempButton, 1);
	motorChoiceLayout->addWidget(tempButton);
	tempButton = new QRadioButton("V");
	motorChoiceButtonGroup_->addButton(tempButton, 2);
	motorChoiceLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Z");
	motorChoiceButtonGroup_->addButton(tempButton, 3);
	motorChoiceLayout->addWidget(tempButton);

	connect(motorChoiceButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onMotorChoiceChanged(int)));

	motorChoiceButtonGroup_->button(int(config_->motorChoice()))->click();
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
	QString exportString =  AMUserSettings::userDataFolder;
	exportString.remove("/userData");
	QLabel *exportPath = new QLabel(QString("Data exported to: %1exportData").arg(exportString));

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

void VESPERSSpatialLineScanConfigurationView::onFluorescenceChoiceChanged(int id)
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

void VESPERSSpatialLineScanConfigurationView::onMotorChoiceChanged(int id)
{
	config_->setMotorChoice(id);
}

void VESPERSSpatialLineScanConfigurationView::onConfigureXRFDetectorClicked()
{
	switch(config_->fluorescenceDetectorChoice()){

	case VESPERSSpatialLineScanConfiguration::None:
		break;

	case VESPERSSpatialLineScanConfiguration::SingleElement:
		emit configureDetector("Single Element");
		break;

	case VESPERSSpatialLineScanConfiguration::FourElement:
		emit configureDetector("Four Element");
		break;
	}
}

void VESPERSSpatialLineScanConfigurationView::onConfigureRoperDetectorClicked()
{
	emit configureDetector("Roper CCD");
}

void VESPERSSpatialLineScanConfigurationView::onUsingCCDChanged(bool useCCD)
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

void VESPERSSpatialLineScanConfigurationView::updateRoiText()
{
	switch(config_->fluorescenceDetectorChoice()){

	case VESPERSSpatialLineScanConfiguration::None:
		break;

	case VESPERSSpatialLineScanConfiguration::SingleElement:
		config_->setRoiInfoList(*VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList());
		break;

	case VESPERSSpatialLineScanConfiguration::FourElement:
		config_->setRoiInfoList(*VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList());
		break;
	}

	roiText_->clear();
	roiText_->insertPlainText("Name\tLow (eV)\tHigh (eV)\n");

	for (int i = 0; i < config_->roiList().count(); i++)
		roiText_->insertPlainText(GeneralUtilities::addGreek(config_->roiList().at(i).name())+"\t" + QString::number(config_->roiList().at(i).low()) + "\t" + QString::number(config_->roiList().at(i).high()) +"\n");
}

void VESPERSSpatialLineScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + convertTimeToString(config_->totalTime()));
}

QString VESPERSSpatialLineScanConfigurationView::convertTimeToString(double time)
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

void VESPERSSpatialLineScanConfigurationView::onCustomContextMenuRequested(QPoint pos)
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

void VESPERSSpatialLineScanConfigurationView::onSetStartPosition()
{
	double position = 0;

	switch(config_->motorChoice()){

	case VESPERSSpatialLineScanConfiguration::H:
		position = VESPERSBeamline::vespers()->pseudoSampleStage()->horiz()->value();
		break;

	case VESPERSSpatialLineScanConfiguration::X:
		position = VESPERSBeamline::vespers()->sampleStageX()->value();
		break;

	case VESPERSSpatialLineScanConfiguration::V:
		position = VESPERSBeamline::vespers()->pseudoSampleStage()->vert()->value();
		break;

	case VESPERSSpatialLineScanConfiguration::Z:
		position = VESPERSBeamline::vespers()->sampleStageZ()->value();
		break;
	}

	config_->setStart(position);
	start_->setValue(position);
	updateMapInfo();
	axesAcceptable();
}

void VESPERSSpatialLineScanConfigurationView::onSetEndPosition()
{
	double position = 0;

	switch(config_->motorChoice()){

	case VESPERSSpatialLineScanConfiguration::H:
		position = VESPERSBeamline::vespers()->pseudoSampleStage()->horiz()->value();
		break;

	case VESPERSSpatialLineScanConfiguration::X:
		position = VESPERSBeamline::vespers()->sampleStageX()->value();
		break;

	case VESPERSSpatialLineScanConfiguration::V:
		position = VESPERSBeamline::vespers()->pseudoSampleStage()->vert()->value();
		break;

	case VESPERSSpatialLineScanConfiguration::Z:
		position = VESPERSBeamline::vespers()->sampleStageZ()->value();
		break;
	}

	config_->setEnd(position);
	end_->setValue(position);
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

