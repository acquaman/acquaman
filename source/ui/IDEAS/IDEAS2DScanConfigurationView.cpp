#include "IDEAS2DScanConfigurationView.h"

#include "application/IDEAS/IDEAS.h"
#include "beamline/IDEAS/IDEASBeamline.h"

#include "util/AMDateTimeUtils.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QPushButton>
#include <QSpinBox>
#include <QMenu>
#include <QStringBuilder>

IDEAS2DScanConfigurationView::IDEAS2DScanConfigurationView(IDEAS2DScanConfiguration *configuration, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	configuration_ = configuration;

	// 1st row: set the start position
	hStart_ = createPositionDoubleSpinBox("H: ", " mm", configuration_->scanAxisAt(0)->regionAt(0)->regionStart(), 3);
	connect(hStart_, SIGNAL(editingFinished()), this, SLOT(onXStartChanged()));
	connect(configuration_->scanAxisAt(0)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(setXAxisStart(AMNumber)));

	vStart_ = createPositionDoubleSpinBox("V: ", " mm", configuration_->scanAxisAt(1)->regionAt(0)->regionStart(), 3);
	connect(vStart_, SIGNAL(editingFinished()), this, SLOT(onYStartChanged()));
	connect(configuration_->scanAxisAt(1)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(setYAxisStart(AMNumber)));

	QPushButton *startUseCurrentButton = new QPushButton("Use Current");
	connect(startUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetStartPosition()));

	// 2nd row: set the end position
	hEnd_ = createPositionDoubleSpinBox("H: ", " mm", configuration_->scanAxisAt(0)->regionAt(0)->regionEnd(), 3);
	connect(hEnd_, SIGNAL(editingFinished()), this, SLOT(onXEndChanged()));
	connect(configuration_->scanAxisAt(0)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(setXAxisEnd(AMNumber)));

	vEnd_ = createPositionDoubleSpinBox("V: ", " mm", configuration_->scanAxisAt(1)->regionAt(0)->regionEnd(), 3);
	connect(vEnd_, SIGNAL(editingFinished()), this, SLOT(onYEndChanged()));
	connect(configuration_->scanAxisAt(1)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(setYAxisEnd(AMNumber)));

	QPushButton *endUseCurrentButton = new QPushButton("Use Current");
	connect(endUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetEndPosition()));

	// 3rd row: set the step size
	hStep_ = createPositionDoubleSpinBox("H: ", QString(" %1").arg(QString::fromUtf8("µm")), double(configuration_->scanAxisAt(0)->regionAt(0)->regionStep())*1000, 1);	// xStep needs to be in mm.
	hStep_->setMinimum(0);
	connect(hStep_, SIGNAL(editingFinished()), this, SLOT(onXStepChanged()));
	connect(configuration_->scanAxisAt(0)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(setXAxisStep(AMNumber)));

	vStep_ = createPositionDoubleSpinBox("V: ", QString(" %1").arg(QString::fromUtf8("µm")), double(configuration_->scanAxisAt(1)->regionAt(0)->regionStep())*1000, 1);	// yStep needs to be in mm.
	vStep_->setMinimum(0);
	connect(vStep_, SIGNAL(editingFinished()), this, SLOT(onYStepChanged()));
	connect(configuration_->scanAxisAt(1)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(setYAxisStep(AMNumber)));

	// the grid layout to hold the positions
	QGridLayout *positionGridLayout = new QGridLayout;
	positionGridLayout->addWidget(new QLabel("Start:"), 0, 0, 1, 2);
	positionGridLayout->addWidget(hStart_, 0, 2, 1, 2);
	positionGridLayout->addWidget(vStart_, 0, 4, 1, 2);
	positionGridLayout->addWidget(startUseCurrentButton, 0, 6, 1, 2);

	positionGridLayout->addWidget(new QLabel("End:"), 1, 0, 1, 2);
	positionGridLayout->addWidget(hEnd_, 1, 2, 1, 2);
	positionGridLayout->addWidget(vEnd_, 1, 4, 1, 2);
	positionGridLayout->addWidget(endUseCurrentButton, 1, 6, 1, 2);

	positionGridLayout->addWidget(new QLabel("Step Size:"), 2, 0, 1, 2);
	positionGridLayout->addWidget(hStep_, 2, 2, 1, 2);
	positionGridLayout->addWidget(vStep_, 2, 4, 1, 2);

	// the map information
	mapInfo_ = new QLabel;
	updateMapInfo();

	// Setup the group box for setting the start and end points.
	QVBoxLayout *positionsLayout = new QVBoxLayout;
	positionsLayout->addLayout(positionGridLayout);
	positionsLayout->addWidget(mapInfo_);

	QGroupBox *positionsBox = new QGroupBox("Positions");
	positionsBox->setLayout(positionsLayout);

	// Dwell time.
	dwellTime_ = createDwellTimeSpinBox(configuration_->scanAxisAt(0)->regionAt(0)->regionTime());
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));
	connect(configuration_->scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(setDwellTime(AMNumber)));
	connect(configuration_->scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), configuration_->scanAxisAt(1)->regionAt(0), SLOT(setRegionTime(AMNumber)));

	QHBoxLayout *timeLayout = new QHBoxLayout;
	timeLayout->addWidget(new QLabel("Dwell Time:"));
	timeLayout->addWidget(dwellTime_);

	// The estimated scan time.
	estimatedTime_ = new QLabel;
	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	onEstimatedTimeChanged();

	// Setup the group box for time.
	QVBoxLayout *timeBoxLayout = new QVBoxLayout;
	timeBoxLayout->addLayout(timeLayout);
	timeBoxLayout->addWidget(estimatedTime_);

	QGroupBox *timeGroupBox = new QGroupBox("Time");
	timeGroupBox->setLayout(timeBoxLayout);

	// Scan name selection
	scanName_ = createScanNameView(configuration_->userScanName());
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
	onScanNameEdited();

	QFormLayout *scanNameLayout = new QFormLayout;
	scanNameLayout->addRow("Scan Name:", scanName_);

	QGroupBox *scanNameGroupBox = new QGroupBox("Scan Information");
	scanNameGroupBox->setLayout(scanNameLayout);

	// BL energy setting
	scanEnergySpinBox_ = createEnergySpinBox("eV", 0, 15000, configuration_->energy());
	scanEnergySettingWarningLabel_ = new QLabel("Settings do not match beamline.");
	scanEnergySettingWarningLabel_->setStyleSheet("QLabel {color: red}");
	setScanEnergyFromBeamlineButton_ = new QPushButton("Set From Beamline");

	connect(scanEnergySpinBox_, SIGNAL(editingFinished()), this, SLOT(onScanEnergySpinBoxEditingFinished()));
	connect(setScanEnergyFromBeamlineButton_, SIGNAL(clicked()), this, SLOT(onSetScanEnergyFromBeamlineButtonClicked()));
	connect(IDEASBeamline::ideas()->monoEnergyControl(), SIGNAL(valueChanged(double)), this, SLOT(onBeamlineEnergyChanged(double)));
	onScanEnergySpinBoxEditingFinished();

	QFormLayout *scanEnergyFL = new QFormLayout();
	scanEnergyFL->addRow("Energy", scanEnergySpinBox_);

	QVBoxLayout *beamlineSettingsGroupBoxVL = new QVBoxLayout();
	beamlineSettingsGroupBoxVL->addLayout(scanEnergyFL);
	beamlineSettingsGroupBoxVL->addStretch();
	beamlineSettingsGroupBoxVL->addWidget(scanEnergySettingWarningLabel_);
	beamlineSettingsGroupBoxVL->addWidget(setScanEnergyFromBeamlineButton_);

	beamlineSettingsGroupBox_ = new QGroupBox("Beamline Settings");
	beamlineSettingsGroupBox_->setMinimumWidth(230);
	beamlineSettingsGroupBox_->setLayout(beamlineSettingsGroupBoxVL);

	// detector setting
	QGroupBox *detectorSettingGroupBox = createAndLayoutDetectorSettings(configuration_);

	// Error label.
	errorLabel_ = new QLabel;
	QFont font = this->font();
	font.setPixelSize(16);
	font.setBold(true);
	QPalette palette = this->palette();
	palette.setColor(QPalette::WindowText, Qt::red);
	errorLabel_->setFont(font);
	errorLabel_->setPalette(palette);

	// Setting up the layout.
	QGridLayout *contentsLayout = new QGridLayout;
	contentsLayout->addWidget(positionsBox, 0, 0, 2, 4);
	contentsLayout->addWidget(timeGroupBox, 2, 0, 1, 4);
	contentsLayout->addWidget(scanNameGroupBox, 3, 0, 1, 4);
	contentsLayout->addWidget(beamlineSettingsGroupBox_, 0, 4, 3, 2);
	contentsLayout->addWidget(detectorSettingGroupBox, 3, 4, 1, 2);
	contentsLayout->addWidget(errorLabel_, 4, 0, 2, 4);

	setLayout(contentsLayout);
}

IDEAS2DScanConfigurationView::~IDEAS2DScanConfigurationView()
{

}

const AMScanConfiguration *IDEAS2DScanConfigurationView::configuration() const
{
	return configuration_;
}

QLineEdit *IDEAS2DScanConfigurationView::createScanNameView(const QString &name)
{
	QLineEdit *scanName = new QLineEdit;
	scanName->setText(name);
	scanName->setAlignment(Qt::AlignCenter);

	return scanName;
}

QDoubleSpinBox *IDEAS2DScanConfigurationView::createPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals)
{
	QDoubleSpinBox *box = new QDoubleSpinBox;
	box->setPrefix(prefix);
	box->setRange(-100000, 100000);
	box->setSuffix(suffix);
	box->setDecimals(decimals);
	box->setValue(value);
	box->setAlignment(Qt::AlignCenter);

	return box;
}

QDoubleSpinBox *IDEAS2DScanConfigurationView::createDwellTimeSpinBox(double time)
{
	QDoubleSpinBox *dwellTime = new QDoubleSpinBox;
	dwellTime->setRange(0, 1000000);
	dwellTime->setSuffix(" s");
	dwellTime->setAlignment(Qt::AlignCenter);
	dwellTime->setDecimals(1);
	dwellTime->setValue(time);

	return dwellTime;
}

QDoubleSpinBox *IDEAS2DScanConfigurationView::createEnergySpinBox(QString units, double minimumValue, double maximumValue, double defaultValue)
{
	QDoubleSpinBox *sampleStageSpinBox = new QDoubleSpinBox();
	sampleStageSpinBox->setSuffix(" " % units);
	sampleStageSpinBox->setSingleStep(0.01);
	sampleStageSpinBox->setDecimals(2);
	sampleStageSpinBox->setAlignment(Qt::AlignCenter);
	sampleStageSpinBox->setFixedWidth(110);

	sampleStageSpinBox->setRange(minimumValue, maximumValue);
	sampleStageSpinBox->setValue(defaultValue);

	return sampleStageSpinBox;
}

void IDEAS2DScanConfigurationView::onScanNameEdited()
{
	QString name = scanName_->text();
	configuration_->setName(name);
	configuration_->setUserScanName(name);
}

void IDEAS2DScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + AMDateTimeUtils::convertTimeToString(configuration_->totalTime()));
}

void IDEAS2DScanConfigurationView::onSetStartPosition()
{
	double h = IDEASBeamline::ideas()->samplePlatformHorizontal()->value();
	double v = IDEASBeamline::ideas()->samplePlatformVertical()->value();

	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStart(h);
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStart(v);

	hStart_->setValue(h);
	vStart_->setValue(v);

	updateMapInfo();
	checkScanAxisValidity();
}

void IDEAS2DScanConfigurationView::onSetEndPosition()
{
	double h = IDEASBeamline::ideas()->samplePlatformHorizontal()->value();
	double v = IDEASBeamline::ideas()->samplePlatformVertical()->value();

	configuration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(h);
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(v);

	hEnd_->setValue(h);
	vEnd_->setValue(v);

	updateMapInfo();
	checkScanAxisValidity();
}

void IDEAS2DScanConfigurationView::onXStartChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStart(hStart_->value());
	updateMapInfo();
	checkScanAxisValidity();
}

void IDEAS2DScanConfigurationView::onXEndChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(hEnd_->value());
	updateMapInfo();
	checkScanAxisValidity();
}

void IDEAS2DScanConfigurationView::onXStepChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStep(hStep_->value()/1000);
	updateMapInfo();
	checkScanAxisValidity();
}

void IDEAS2DScanConfigurationView::onYStartChanged()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStart(vStart_->value());
	updateMapInfo();
	checkScanAxisValidity();
}

void IDEAS2DScanConfigurationView::onYEndChanged()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(vEnd_->value());
	updateMapInfo();
	checkScanAxisValidity();
}

void IDEAS2DScanConfigurationView::onYStepChanged()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStep(vStep_->value()/1000);
	updateMapInfo();
	checkScanAxisValidity();
}

void IDEAS2DScanConfigurationView::onDwellTimeChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionTime(dwellTime_->value());
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionTime(dwellTime_->value());
}

void IDEAS2DScanConfigurationView::onScanEnergySpinBoxEditingFinished()
{
	configuration_->setEnergy(scanEnergySpinBox_->value());

	if (IDEASBeamline::ideas()->monoEnergyControl()->withinTolerance(scanEnergySpinBox_->value()))
		scanEnergySettingWarningLabel_->hide();

	else
		scanEnergySettingWarningLabel_->show();
}

void IDEAS2DScanConfigurationView::onSetScanEnergyFromBeamlineButtonClicked()
{
	scanEnergySpinBox_->setValue(IDEASBeamline::ideas()->monoEnergyControl()->value());
	onScanEnergySpinBoxEditingFinished();
}

void IDEAS2DScanConfigurationView::onBeamlineEnergyChanged(double value)
{
	Q_UNUSED(value)
	onScanEnergySpinBoxEditingFinished();
}

void IDEAS2DScanConfigurationView::updateMapInfo()
{
	double hSize = fabs(double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()));
	double vSize = fabs(double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(1)->regionAt(0)->regionStart()));

	int hPoints = configuration_->scanAxisAt(0)->numberOfPoints();
	int vPoints = configuration_->scanAxisAt(1)->numberOfPoints();

	mapInfo_->setText(QString("Map Size: %1 %2 x %3 %2\t Points: %4 x %5")
			  .arg(QString::number(hSize*1000, 'f', 1))
			  .arg(QString::fromUtf8("µm"))
			  .arg(QString::number(vSize*1000, 'f', 1))
			  .arg(hPoints)
			  .arg(vPoints)
			  );
}

void IDEAS2DScanConfigurationView::setXAxisStart(const AMNumber &value)
{
	hStart_->setValue(double(value));
}

void IDEAS2DScanConfigurationView::setYAxisStart(const AMNumber &value)
{
	vStart_->setValue(double(value));
}

void IDEAS2DScanConfigurationView::setXAxisStep(const AMNumber &value)
{
	hStep_->setValue(double(value)*1000);
}

void IDEAS2DScanConfigurationView::setYAxisStep(const AMNumber &value)
{
	vStep_->setValue(double(value)*1000);
}

void IDEAS2DScanConfigurationView::setXAxisEnd(const AMNumber &value)
{
	hEnd_->setValue(double(value));
}

void IDEAS2DScanConfigurationView::setYAxisEnd(const AMNumber &value)
{
	vEnd_->setValue(double(value));
}

void IDEAS2DScanConfigurationView::setDwellTime(const AMNumber &value)
{
	dwellTime_->setValue(double(value));
}

void IDEAS2DScanConfigurationView::onFluorescenceDetectorChanged(int detector)
{
	configuration_->setFluorescenceDetector((IDEAS::FluorescenceDetectors)detector);
}

void IDEAS2DScanConfigurationView::updateFluorescenceDetectorComboBoxGe13Element(bool connected)
{
	int currentIndex = fluorescenceDetectorComboBox_->currentIndex();
	fluorescenceDetectorComboBox_->removeItem((int)IDEAS::Ge13Element);

	if (connected)
		fluorescenceDetectorComboBox_->insertItem((int)IDEAS::Ge13Element, "13-el Ge");

	updateFluorescenceDetectorComboBox((IDEAS::FluorescenceDetectors)currentIndex);
}


void IDEAS2DScanConfigurationView::checkScanAxisValidity()
{
	QString errorString = "";

	if (hEnd_->value() <= hStart_->value()){

		hStep_->setStyleSheet("QDoubleSpinBox { background-color: red; }");
		errorString.append("Horizontal end value must be larger than start value.\n");
	}

	else {

		hStep_->setStyleSheet(this->styleSheet());
	}

	if (vEnd_->value() <= vStart_->value()){

		vStep_->setStyleSheet("QDoubleSpinBox { background-color: red; }");
		errorString.append("Vertical end value must be larger than start value.\n");
	}

	else{
		vStep_->setStyleSheet(this->styleSheet());
	}

	errorLabel_->setText(errorString);
}

void IDEAS2DScanConfigurationView::updateFluorescenceDetectorComboBox(IDEAS::FluorescenceDetectors detector)
{
	fluorescenceDetectorComboBox_->setCurrentIndex(int(detector));
}

QComboBox *IDEAS2DScanConfigurationView::createFluorescenceComboBox()
{
	QComboBox *newComboBox = new QComboBox;
	newComboBox->insertItem(0, "None");
	newComboBox->insertItem(1, "Ketek");
	if (IDEASBeamline::ideas()->ge13Element()->isConnected())
		newComboBox->insertItem(2, "13-el Ge");

	return newComboBox;

}

QGroupBox *IDEAS2DScanConfigurationView::createAndLayoutDetectorSettings(IDEASScanConfiguration *configuration)
{
	fluorescenceDetectorComboBox_ = createFluorescenceComboBox();

	QHBoxLayout *brukerDetectorHLayout = new QHBoxLayout;
	brukerDetectorHLayout->addWidget(new QLabel("XRF:"));
	brukerDetectorHLayout->addWidget(fluorescenceDetectorComboBox_);

	QVBoxLayout *detectorBoxLayout = new QVBoxLayout;
	detectorBoxLayout->addLayout(brukerDetectorHLayout);

	QGroupBox * detectorSettingGroupBox = new QGroupBox("Detector Setting");
	detectorSettingGroupBox->setLayout(detectorBoxLayout);

	connect(fluorescenceDetectorComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onFluorescenceDetectorChanged(int)));
	connect(IDEASBeamline::ideas()->ge13Element(), SIGNAL(connected(bool)), this, SLOT(updateFluorescenceDetectorComboBoxGe13Element(bool)));
	connect(configuration->dbObject(), SIGNAL(fluorescenceDetectorChanged(IDEAS::FluorescenceDetectors)), this, SLOT(updateFluorescenceDetectorComboBox(IDEAS::FluorescenceDetectors)));

	// default using bruker
	updateFluorescenceDetectorComboBox(IDEAS::Ketek);

	return detectorSettingGroupBox;
}
