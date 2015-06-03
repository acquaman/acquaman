#include "SXRMB2DMapScanConfigurationView.h"

#include "application/SXRMB/SXRMB.h"
#include "beamline/SXRMB/SXRMBBeamline.h"


#include <QGridLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QPushButton>
#include <QSpinBox>
#include <QMenu>

SXRMB2DMapScanConfigurationView::SXRMB2DMapScanConfigurationView(SXRMB2DMapScanConfiguration *configuration, QWidget *parent)
	: SXRMBScanConfigurationView(parent)
{
	SXRMBBeamline *sxrmbBL = SXRMBBeamline::sxrmb();

	configuration_ = configuration;
	excitationEnergyIsHidden_ = false;


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

	// 4th row: set the focus position
	normalPosition_ = createPositionDoubleSpinBox("N: ", " mm", configuration_->y(), 3);
	connect(normalPosition_, SIGNAL(editingFinished()), this, SLOT(onNormalPositionChanged()));
	connect(configuration_->dbObject(), SIGNAL(yChanged(double)), normalPosition_, SLOT(setValue(double)));

	QPushButton *updateNormalPosition = new QPushButton("Set Normal");
	connect(updateNormalPosition, SIGNAL(clicked()), this, SLOT(onSetNormalPosition()));

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

	positionGridLayout->addWidget(new QLabel("Focus Position:"), 3, 0, 1, 2);
	positionGridLayout->addWidget(normalPosition_, 3, 2, 1, 2);
	positionGridLayout->addWidget(updateNormalPosition, 3, 6, 1, 2);

	// the map information
	mapInfo_ = new QLabel;
	updateMapInfo();

	/// Setup the group box for setting the start and end points.
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

	/// Setup the group box for time.
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

	// Auto-export option.
	QGroupBox *autoExportGroupBox = addExporterOptionsView(QStringList() << "Ascii" << "SMAK");
	connect(autoExportButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(updateAutoExporter(int)));
	autoExportButtonGroup_->button(configuration_->exportAsAscii() ? 0 : 1)->click();

	// BL energy setting
	scanEnergySpinBox_ = createEnergySpinBox("eV", sxrmbBL->energy()->minimumValue(), sxrmbBL->energy()->maximumValue(), configuration_->energy());
	scanEnergySettingWarningLabel_ = new QLabel("Settings do not match beamline.");
	scanEnergySettingWarningLabel_->setStyleSheet("QLabel {color: red}");
	setScanEnergyFromBeamlineButton_ = new QPushButton("Set From Beamline");

	connect(scanEnergySpinBox_, SIGNAL(editingFinished()), this, SLOT(onScanEnergySpinBoxEditingFinished()));
	connect(setScanEnergyFromBeamlineButton_, SIGNAL(clicked()), this, SLOT(onSetScanEnergyFromBeamlineButtonClicked()));
	connect(SXRMBBeamline::sxrmb()->energy(), SIGNAL(valueChanged(double)), this, SLOT(onBeamlineEnergyChanged(double)));
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
	contentsLayout->addWidget(scanNameGroupBox, 3, 0, 1, 3);
	contentsLayout->addWidget(autoExportGroupBox, 3, 3, 1, 1);
	contentsLayout->addWidget(beamlineSettingsGroupBox_, 0, 4, 3, 2);
	contentsLayout->addWidget(detectorSettingGroupBox, 3, 4, 1, 2);
	contentsLayout->addWidget(errorLabel_, 4, 0, 2, 4);

	/// the squeeze layout of the window
	QHBoxLayout *squeezeContents = new QHBoxLayout;
	squeezeContents->addStretch();
	squeezeContents->addLayout(contentsLayout);
	squeezeContents->addStretch();

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addStretch();
	configViewLayout->addLayout(squeezeContents);
	configViewLayout->addStretch();

	setLayout(configViewLayout);

	connect(sxrmbBL, SIGNAL(endstationChanged(SXRMB::Endstation, SXRMB::Endstation)), this, SLOT(onBeamlineEndstationChanged(SXRMB::Endstation, SXRMB::Endstation)));
}

QLineEdit *SXRMB2DMapScanConfigurationView::createScanNameView(const QString &name)
{
	QLineEdit *scanName = new QLineEdit;
	scanName->setText(name);
	scanName->setAlignment(Qt::AlignCenter);

	return scanName;
}

QDoubleSpinBox *SXRMB2DMapScanConfigurationView::createPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals)
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

QDoubleSpinBox *SXRMB2DMapScanConfigurationView::createDwellTimeSpinBox(double time)
{
	QDoubleSpinBox *dwellTime = new QDoubleSpinBox;
	dwellTime->setRange(0, 1000000);
	dwellTime->setSuffix(" s");
	dwellTime->setAlignment(Qt::AlignCenter);
	dwellTime->setDecimals(1);
	dwellTime->setValue(time);

	return dwellTime;
}

QDoubleSpinBox *SXRMB2DMapScanConfigurationView::createEnergySpinBox(QString units, double minimumValue, double maximumValue, double defaultValue) {
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

void SXRMB2DMapScanConfigurationView::onScanNameEdited()
{
	QString name = scanName_->text();
	configuration_->setName(name);
	configuration_->setUserScanName(name);
}

void SXRMB2DMapScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + SXRMB::convertTimeToString(configuration_->totalTime()));
}

void SXRMB2DMapScanConfigurationView::onSetStartPosition()
{
	double h = SXRMBBeamline::sxrmb()->microprobeSampleStageX()->value();
	double v = SXRMBBeamline::sxrmb()->microprobeSampleStageZ()->value();
	double n = SXRMBBeamline::sxrmb()->microprobeSampleStageY()->value();;

	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStart(h);
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStart(v);
	configuration_->setY(n);
	hStart_->setValue(h);
	vStart_->setValue(v);
	updateMapInfo();
	checkScanAxisValidity();
}

void SXRMB2DMapScanConfigurationView::onSetEndPosition()
{
	double h = SXRMBBeamline::sxrmb()->microprobeSampleStageX()->value();
	double v = SXRMBBeamline::sxrmb()->microprobeSampleStageZ()->value();

	configuration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(h);
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(v);
	hEnd_->setValue(h);
	vEnd_->setValue(v);
	updateMapInfo();
	checkScanAxisValidity();
}

void SXRMB2DMapScanConfigurationView::onSetNormalPosition()
{
	double n = SXRMBBeamline::sxrmb()->microprobeSampleStageY()->value();
	configuration_->setY(n);
	updateMapInfo();
}

void SXRMB2DMapScanConfigurationView::onXStartChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStart(hStart_->value());
	updateMapInfo();
	checkScanAxisValidity();
}

void SXRMB2DMapScanConfigurationView::onXEndChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(hEnd_->value());
	updateMapInfo();
	checkScanAxisValidity();
}

void SXRMB2DMapScanConfigurationView::onXStepChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStep(hStep_->value()/1000);
	updateMapInfo();
	checkScanAxisValidity();
}

void SXRMB2DMapScanConfigurationView::onYStartChanged()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStart(vStart_->value());
	updateMapInfo();
	checkScanAxisValidity();
}

void SXRMB2DMapScanConfigurationView::onYEndChanged()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(vEnd_->value());
	updateMapInfo();
	checkScanAxisValidity();
}

void SXRMB2DMapScanConfigurationView::onYStepChanged()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStep(vStep_->value()/1000);
	updateMapInfo();
	checkScanAxisValidity();
}

void SXRMB2DMapScanConfigurationView::onNormalPositionChanged()
{
	configuration_->setY(normalPosition_->value());
	updateMapInfo();
	checkScanAxisValidity();
}

bool SXRMB2DMapScanConfigurationView::excitationEnergyIsHidden() const{
	return excitationEnergyIsHidden_;
}

void SXRMB2DMapScanConfigurationView::setExcitationEnergyIsHidden(bool excitationEnergyIsHidden){
	excitationEnergyIsHidden_ = excitationEnergyIsHidden;

	if(excitationEnergyIsHidden_)
		beamlineSettingsGroupBox_->hide();
	else
		beamlineSettingsGroupBox_->show();
}

void SXRMB2DMapScanConfigurationView::onDwellTimeChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionTime(dwellTime_->value());
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionTime(dwellTime_->value());
}

void SXRMB2DMapScanConfigurationView::onScanEnergySpinBoxEditingFinished() {
	configuration_->setEnergy(scanEnergySpinBox_->value());
	if (SXRMBBeamline::sxrmb()->energy()->withinTolerance(scanEnergySpinBox_->value()))
		scanEnergySettingWarningLabel_->hide();
	else
		scanEnergySettingWarningLabel_->show();
}

void SXRMB2DMapScanConfigurationView::onSetScanEnergyFromBeamlineButtonClicked() {
	scanEnergySpinBox_->setValue(SXRMBBeamline::sxrmb()->energy()->value());
	onScanEnergySpinBoxEditingFinished();
}

void SXRMB2DMapScanConfigurationView::onBeamlineEnergyChanged(double value){
	Q_UNUSED(value)

	onScanEnergySpinBoxEditingFinished();
}

void SXRMB2DMapScanConfigurationView::updateMapInfo()
{
	double hSize = fabs(double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()));
	double vSize = fabs(double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(1)->regionAt(0)->regionStart()));

	int hPoints = int((hSize)/double(configuration_->scanAxisAt(0)->regionAt(0)->regionStep()));
	if ((hSize - (hPoints + 0.01)*double(configuration_->scanAxisAt(0)->regionAt(0)->regionStep())) < 0)
		hPoints += 1;
	else
		hPoints += 2;

	int vPoints = int((vSize)/double(configuration_->scanAxisAt(1)->regionAt(0)->regionStep()));
	if ((vSize - (vPoints + 0.01)*double(configuration_->scanAxisAt(1)->regionAt(0)->regionStep())) < 0)
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

QGroupBox *SXRMB2DMapScanConfigurationView::addExporterOptionsView(QStringList list)
{
	QRadioButton *autoExportButton;
	QHBoxLayout *autoExportLayout = new QHBoxLayout;
	autoExportButtonGroup_ = new QButtonGroup;

	for (int i = 0, iSize = list.size(); i < iSize; i++){

		autoExportButton = new QRadioButton(list.at(i));
		autoExportButtonGroup_->addButton(autoExportButton, i);
		autoExportLayout->addWidget(autoExportButton);
	}

	QGroupBox *autoExportGroupBox = new QGroupBox("Export Options");
	autoExportGroupBox->setLayout(autoExportLayout);

	return autoExportGroupBox;
}

void SXRMB2DMapScanConfigurationView::onBeamlineEndstationChanged(SXRMB::Endstation fromEndstation, SXRMB::Endstation toEndstation)
{
	Q_UNUSED(fromEndstation)

	updatePowerOnHVControlCheckBoxText();
}

void SXRMB2DMapScanConfigurationView::setXAxisStart(const AMNumber &value)
{
	hStart_->setValue(double(value));
}

void SXRMB2DMapScanConfigurationView::setYAxisStart(const AMNumber &value)
{
	vStart_->setValue(double(value));
}

void SXRMB2DMapScanConfigurationView::setXAxisStep(const AMNumber &value)
{
	hStep_->setValue(double(value)*1000);
}

void SXRMB2DMapScanConfigurationView::setYAxisStep(const AMNumber &value)
{
	vStep_->setValue(double(value)*1000);
}

void SXRMB2DMapScanConfigurationView::setXAxisEnd(const AMNumber &value)
{
	hEnd_->setValue(double(value));
}

void SXRMB2DMapScanConfigurationView::setYAxisEnd(const AMNumber &value)
{
	vEnd_->setValue(double(value));
}

void SXRMB2DMapScanConfigurationView::setDwellTime(const AMNumber &value)
{
	dwellTime_->setValue(double(value));
}

void SXRMB2DMapScanConfigurationView::updateAutoExporter(int useAscii)
{
	configuration_->setExportAsAscii(useAscii == 0);
}

void SXRMB2DMapScanConfigurationView::onFluorescenceDetectorChanged(int detector)
{
	configuration_->setFluorescenceDetector((SXRMB::FluorescenceDetectors)detector);
}

void SXRMB2DMapScanConfigurationView::onPowerOnTEYHVControlEnabled(bool value)
{
	configuration_->setPowerOnHVControlEnabled(value);
}

void SXRMB2DMapScanConfigurationView::checkScanAxisValidity()
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

