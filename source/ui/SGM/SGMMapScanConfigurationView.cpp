#include "SGMMapScanConfigurationView.h"

#include "util/AMDateTimeUtils.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QLineEdit>

SGMMapScanConfigurationView::SGMMapScanConfigurationView(SGMMapScanConfiguration *configuration, AMDetectorSet *detectors, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	configuration_ = configuration;
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
	dwellTime_->setPrefix("Time: ");
	dwellTime_->setSuffix(" s");
	dwellTime_->setAlignment(Qt::AlignCenter);
	dwellTime_->setDecimals(1);
	dwellTime_->setValue(double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime()));
	dwellTime_->setMinimumWidth(100);
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));

	// The estimated scan time.
	estimatedTime_ = new QLabel;
	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged(double)));
	onEstimatedTimeChanged(configuration_->totalTime());

	scanInformation_ = new QLabel("Scan Size:");

	QVBoxLayout *timeBoxLayout = new QVBoxLayout;
	timeBoxLayout->addWidget(estimatedTime_);
	timeBoxLayout->addWidget(scanInformation_);

	QGroupBox *timeGroupBox = new QGroupBox("Time");
	timeGroupBox->setLayout(timeBoxLayout);

	// Setup the group box for setting the start and end points.
	QGroupBox *positionsBox = new QGroupBox("Spatial Axis");

	axisStart1_ = createPositionDoubleSpinBox("Start: ", " mm", double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()), 2);
	axisEnd1_ = createPositionDoubleSpinBox("End: ", " mm", double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd()), 2);

	connect(axisStart1_, SIGNAL(editingFinished()), this, SLOT(onStart1Changed()));
	connect(axisEnd1_, SIGNAL(editingFinished()), this, SLOT(onEnd1Changed()));
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));

	AMScanAxisRegion *region = configuration_->scanAxisAt(0)->regionAt(0);
	connect(region, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(setStart1(AMNumber)));
	connect(region, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(setEnd1(AMNumber)));
	connect(region, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(setDwellTime(AMNumber)));

	QHBoxLayout *axis1Layout = new QHBoxLayout;
	axis1Layout->addWidget(new QLabel("H:"));
	axis1Layout->addWidget(axisStart1_);
	axis1Layout->addWidget(axisEnd1_);
	axis1Layout->addWidget(dwellTime_);

	axisStart2_ = createPositionDoubleSpinBox("Start: ", " mm", double(configuration_->scanAxisAt(1)->regionAt(0)->regionStart()), 2);
	axisStep2_ = createPositionDoubleSpinBox("Step: ", " mm", double(configuration_->scanAxisAt(1)->regionAt(0)->regionStep()), 2);
	axisEnd2_ = createPositionDoubleSpinBox("End: ", " mm", double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd()), 2);

	connect(axisStart2_, SIGNAL(editingFinished()), this, SLOT(onStart2Changed()));
	connect(axisStep2_, SIGNAL(editingFinished()), this, SLOT(onStep2Changed()));
	connect(axisEnd2_, SIGNAL(editingFinished()), this, SLOT(onEnd2Changed()));

	region = configuration_->scanAxisAt(1)->regionAt(0);
	connect(region, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(setStart2(AMNumber)));
	connect(region, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(setStep2(AMNumber)));
	connect(region, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(setEnd2(AMNumber)));

	QHBoxLayout *axis2Layout = new QHBoxLayout;
	axis2Layout->addWidget(new QLabel("V:"));
	axis2Layout->addWidget(axisStart2_);
	axis2Layout->addWidget(axisStep2_);
	axis2Layout->addWidget(axisEnd2_);

	QVBoxLayout *positionLayout = new QVBoxLayout;
	positionLayout->addLayout(axis1Layout);
	positionLayout->addLayout(axis2Layout);

	positionsBox->setLayout(positionLayout);

	QGroupBox *detectorGroupBox = new QGroupBox("Detectors");
	detectorGroupBox->setFlat(true);
	detectorGroup_ = new QButtonGroup;
	detectorGroup_->setExclusive(false);
	detectorLayout_ = new QVBoxLayout;

	connect(detectorGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onDetectorSelectionChanged(QAbstractButton*)));

	detectorGroupBox->setLayout(detectorLayout_);

	updateScanInformation();
	setDetectors(detectors);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(positionsBox);
	layout->addWidget(scanName_);
	layout->addWidget(timeGroupBox);

	QHBoxLayout *moreLayout = new QHBoxLayout;
	moreLayout->addStretch();
	moreLayout->addLayout(layout);
	moreLayout->addWidget(detectorGroupBox);
	moreLayout->addStretch();

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addStretch();
	configViewLayout->addLayout(moreLayout);
	configViewLayout->addStretch();

	setLayout(configViewLayout);
}


SGMMapScanConfigurationView::~SGMMapScanConfigurationView()
{

}

void SGMMapScanConfigurationView::setDetectors(AMDetectorSet *newDetectors)
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

void SGMMapScanConfigurationView::setStart1(const AMNumber &value)
{
	axisStart1_->setValue(double(value));
}

void SGMMapScanConfigurationView::setEnd1(const AMNumber &value)
{
	axisEnd1_->setValue(double(value));
}

void SGMMapScanConfigurationView::setDwellTime(const AMNumber &value)
{
	dwellTime_->setValue(double(value));
}

void SGMMapScanConfigurationView::setStart2(const AMNumber &value)
{
	axisStart2_->setValue(double(value));
}

void SGMMapScanConfigurationView::setStep2(const AMNumber &value)
{
	axisStep2_->setValue(double(value));
}

void SGMMapScanConfigurationView::setEnd2(const AMNumber &value)
{
	axisEnd2_->setValue(double(value));
}

void SGMMapScanConfigurationView::onStart1Changed()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStart(axisStart1_->value());
	updateScanInformation();
}

void SGMMapScanConfigurationView::onEnd1Changed()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(axisEnd1_->value());
	updateScanInformation();
}

void SGMMapScanConfigurationView::onDwellTimeChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionTime(dwellTime_->value());
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionTime(dwellTime_->value());
	updateScanInformation();
}

void SGMMapScanConfigurationView::onStart2Changed()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStart(axisStart2_->value());
	updateScanInformation();
}

void SGMMapScanConfigurationView::onStep2Changed()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStep(axisStep2_->value());
	updateScanInformation();
}

void SGMMapScanConfigurationView::onEnd2Changed()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(axisEnd2_->value());
	updateScanInformation();
}

void SGMMapScanConfigurationView::onScanNameEdited()
{
	QString name = scanName_->text();
	configuration_->setName(name);
	configuration_->setUserScanName(name);
}

void SGMMapScanConfigurationView::onEstimatedTimeChanged(double newTime)
{
	estimatedTime_->setText("Estimated time per scan:\t" + AMDateTimeUtils::convertTimeToString(newTime));
}

void SGMMapScanConfigurationView::onDetectorSelectionChanged(QAbstractButton *button)
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

void SGMMapScanConfigurationView::updateScanInformation()
{
	double xSize = qAbs(double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()));
	double ySize = qAbs(double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(1)->regionAt(0)->regionStart()));
	double resolution = xSize/double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime());

	scanInformation_->setText(QString("Scan Size: %1mm x %2mm\t Resolution: %3 mm/s")
				  .arg(xSize, 0, 'f', 3)
				  .arg(ySize, 0, 'f', 3)
				  .arg(resolution, 0, 'f', 3));
}

QDoubleSpinBox *SGMMapScanConfigurationView::createPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals)
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
