#include "AMGenericContinuousScanConfigurationView.h"

#include <QScrollArea>
#include <QGroupBox>
#include <QCheckBox>

#include "util/AMDateTimeUtils.h"

AMGenericContinuousScanConfigurationView::AMGenericContinuousScanConfigurationView(AMGenericContinuousScanConfiguration *configuration, AMControlSet *controls, AMDetectorSet *detectors, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	configuration_ = configuration;
	controls_ = 0;
	detectors_ = 0;

	scanName_ = new QLineEdit;
	scanName_->setText(configuration_->name());
	scanName_->setAlignment(Qt::AlignCenter);
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
	onScanNameEdited();

	// Dwell time.
	dwellTime_ = new QDoubleSpinBox;
	dwellTime_->setRange(0, 1000000);
	dwellTime_->setSuffix(" s");
	dwellTime_->setAlignment(Qt::AlignCenter);
	dwellTime_->setDecimals(1);
	dwellTime_->setValue(configuration_->scanAxes().size() > 0 ? double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime()) : 1.0);
	dwellTime_->setMinimumWidth(100);
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));

	QHBoxLayout *timeLayout = new QHBoxLayout;
	timeLayout->addWidget(new QLabel("Dwell Time:"));
	timeLayout->addWidget(dwellTime_);

	// The estimated scan time.
	estimatedTime_ = new QLabel;
	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	onEstimatedTimeChanged();

	scanInformation_ = new QLabel("Scan Size:");

	QVBoxLayout *timeBoxLayout = new QVBoxLayout;
	timeBoxLayout->addLayout(timeLayout);
	timeBoxLayout->addWidget(estimatedTime_);
	timeBoxLayout->addWidget(scanInformation_);

	QGroupBox *timeGroupBox = new QGroupBox("Time");
	timeGroupBox->setLayout(timeBoxLayout);

	axisControlChoice1_ = new QComboBox;

	setControls(controls);

	connect(axisControlChoice1_, SIGNAL(currentIndexChanged(int)), this, SLOT(onAxisControlChoice1Changed()));

	// Setup the group box for setting the start and end points.
	QGroupBox *positionsBox = new QGroupBox("Positions");

	axisStart1_ = createPositionDoubleSpinBox("Start: ", "", configuration_->scanAxes().size() > 0 ? double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()) : -1.0, 3);
	axisStep1_ = createPositionDoubleSpinBox("Step: ", "", configuration_->scanAxes().size() > 0 ? double(configuration_->scanAxisAt(0)->regionAt(0)->regionStep()) : -1.0, 3);
	axisEnd1_ = createPositionDoubleSpinBox("End: ", "", configuration_->scanAxes().size() > 0 ? double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd()) : -1.0, 3);

	if(configuration_->scanAxes().count() > 0) {
		// In cases where the scan axis has already been added we need connect our widgets now
		onScanAxisAdded(configuration_->scanAxes().at(0));
	}
	connect(configuration_, SIGNAL(scanAxisAdded(AMScanAxis*)), this, SLOT(onScanAxisAdded(AMScanAxis*)));

	QHBoxLayout *axis1Layout = new QHBoxLayout;
	axis1Layout->addWidget(axisStart1_);
	axis1Layout->addWidget(axisStep1_);
	axis1Layout->addWidget(axisEnd1_);
	axis1Layout->addWidget(axisControlChoice1_);

	QVBoxLayout *positionLayout = new QVBoxLayout;
	positionLayout->addLayout(axis1Layout);

	positionsBox->setLayout(positionLayout);

	// Set initial spinbox values in cases where we don't yet have axes:
	if(configuration_->scanAxes().count() == 0) {
		// Set defaults for axis 1
		onAxisControlChoice1Changed();
	}

	QScrollArea *detectorScrollArea = new QScrollArea;
	QGroupBox *detectorGroupBox = new QGroupBox("Detectors");
	detectorGroupBox->setFlat(true);
	detectorGroup_ = new QButtonGroup;
	detectorGroup_->setExclusive(false);
	detectorLayout_ = new QVBoxLayout;

	setDetectors(detectors);

	connect(detectorGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onDetectorSelectionChanged(QAbstractButton*)));

	detectorGroupBox->setLayout(detectorLayout_);
	detectorScrollArea->setWidget(detectorGroupBox);
	detectorScrollArea->setFrameStyle(QFrame::NoFrame);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(positionsBox);
	layout->addWidget(scanName_);
	layout->addWidget(timeGroupBox);

	QHBoxLayout *moreLayout = new QHBoxLayout;
	moreLayout->addStretch();
	moreLayout->addLayout(layout);
	moreLayout->addWidget(detectorScrollArea);
	moreLayout->addStretch();

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addStretch();
	configViewLayout->addLayout(moreLayout);
	configViewLayout->addStretch();

	setLayout(configViewLayout);
}

AMGenericContinuousScanConfigurationView::~AMGenericContinuousScanConfigurationView()
{

}

void AMGenericContinuousScanConfigurationView::setControls(AMControlSet *newControls)
{
	if (controls_ != newControls) {

		// Clear previously dislayed controls.

		axisControlChoice1_->clear();

		controlNameMap_.clear();

		// Set new controls.

		controls_ = newControls;

		// Add new controls.

		axisControlChoice1_->addItem("None");

		for (int controlIndex = 0, controlCount = controls_->count(); controlIndex < controlCount; controlIndex++) {
			AMControl *control = controls_->at(controlIndex);

			if (control) {
				QString name = control->name();

				axisControlChoice1_->addItem(name);
				controlNameMap_.insert(name, control);
			}
		}

		// Update current indexes.

		if (configuration_ && configuration_->axisControlInfos().count() > 0) {

			axisControlChoice1_->setCurrentIndex(axisControlChoice1_->findText(configuration_->axisControlInfos().at(0).name()));
		}
	}
}

void AMGenericContinuousScanConfigurationView::setDetectors(AMDetectorSet *newDetectors)
{
	if (detectors_ != newDetectors) {

		// Clear previously displayed detectors.

		for (int buttonIndex = 0, buttonCount = detectorGroup_->buttons().count(); buttonIndex < buttonCount; buttonIndex++) {
			QAbstractButton *button = detectorGroup_->button(buttonIndex);
			detectorLayout_->removeWidget(button);
			detectorGroup_->removeButton(button);
			button->deleteLater();
		}

		detectorButtonMap_.clear();

		// Set new detectors.

		detectors_ = newDetectors;

		// Add new detectors.

		for (int detectorIndex = 0, detectorCount = detectors_->count(); detectorIndex < detectorCount; detectorIndex++) {
			AMDetector *detector = detectors_->at(detectorIndex);

			if (detector) {
				QCheckBox *checkBox = new QCheckBox(detector->name());
				detectorGroup_->addButton(checkBox);
				detectorLayout_->addWidget(checkBox);

				detectorButtonMap_.insert(detector, checkBox);

				// Update current selection.

				if (configuration_ && configuration_->detectorConfigurations().contains(detector->name())) {
					checkBox->blockSignals(true);
					checkBox->setChecked(true);
					checkBox->blockSignals(false);
				}
			}
		}
	}
}

void AMGenericContinuousScanConfigurationView::onScanNameEdited()
{
	QString name = scanName_->text();
	configuration_->setName(name);
	configuration_->setUserScanName(name);
}

void AMGenericContinuousScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + AMDateTimeUtils::convertTimeToString(configuration_->totalTime()));
}

QDoubleSpinBox *AMGenericContinuousScanConfigurationView::createPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals)
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

void AMGenericContinuousScanConfigurationView::onAxisControlChoice1Changed()
{
	if (axisControlChoice1_->currentIndex() == 0){

		axisStart1_->setEnabled(false);
		axisStep1_->setEnabled(false);
		axisEnd1_->setEnabled(false);

		configuration_->removeControl(0);

		setStart1(-1.0);
		setStep1(-1.0);
		setEnd1(-1.0);
	}

	else{

		axisStart1_->setEnabled(true);
		axisStep1_->setEnabled(true);
		axisEnd1_->setEnabled(true);

		AMControl *control = controlNameMap_.value( axisControlChoice1_->itemText(axisControlChoice1_->currentIndex()), 0 );

		if (control) {
			configuration_->setControl(0, control->toInfo());			
			setStart1(control->value());
			setStep1(1.0);
			setEnd1(control->value()+10);
		}

		onStart1Changed();
		onStep1Changed();
		onEnd1Changed();
		onDwellTimeChanged();
	}

	updateScanInformation();
}

void AMGenericContinuousScanConfigurationView::onStart1Changed()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStart(axisStart1_->value());
	updateScanInformation();
}

void AMGenericContinuousScanConfigurationView::onStep1Changed()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStep(axisStep1_->value());
	updateScanInformation();
}

void AMGenericContinuousScanConfigurationView::onEnd1Changed()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(axisEnd1_->value());
	updateScanInformation();
}

void AMGenericContinuousScanConfigurationView::onDwellTimeChanged()
{
	if (configuration_->scanAxes().size() > 0){

		configuration_->scanAxisAt(0)->regionAt(0)->setRegionTime(dwellTime_->value());
	}
}

void AMGenericContinuousScanConfigurationView::updateScanInformation()
{
	if (configuration_->scanAxes().size() == 1){

		double size = fabs(double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()));
		int points = configuration_->scanAxisAt(0)->numberOfPoints();

		scanInformation_->setText(QString("Scan Size: %1\t Points: %2")
						  .arg(QString::number(size, 'f', 1))
						  .arg(points)
						  );
	}
}

void AMGenericContinuousScanConfigurationView::setStart1(const AMNumber &value)
{
	axisStart1_->setValue(double(value));
}

void AMGenericContinuousScanConfigurationView::setStep1(const AMNumber &value)
{
	axisStep1_->setValue(double(value));
}

void AMGenericContinuousScanConfigurationView::setEnd1(const AMNumber &value)
{
	axisEnd1_->setValue(double(value));
}

void AMGenericContinuousScanConfigurationView::setDwellTime(const AMNumber &value)
{
	dwellTime_->setValue(double(value));
}

void AMGenericContinuousScanConfigurationView::onScanAxisAdded(AMScanAxis *axis)
{
	if (configuration_->scanAxes().size() == 1){

		connect(axisStart1_, SIGNAL(editingFinished()), this, SLOT(onStart1Changed()));
		connect(axisStep1_, SIGNAL(editingFinished()), this, SLOT(onStep1Changed()));
		connect(axisEnd1_, SIGNAL(editingFinished()), this, SLOT(onEnd1Changed()));
		connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));

		connect(axis->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(setStart1(AMNumber)));
		connect(axis->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(setStep1(AMNumber)));
		connect(axis->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(setEnd1(AMNumber)));
		connect(axis->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(setDwellTime(AMNumber)));
	}
}

void AMGenericContinuousScanConfigurationView::onDetectorSelectionChanged(QAbstractButton *button)
{
	if (button) {
		AMDetector *detector = detectorButtonMap_.key(button, 0);

		if (detector) {
			if (button->isChecked())
				configuration_->addDetector(detector->toInfo());
			else
				configuration_->removeDetector(detector->toInfo());
		}
	}
}
