#include "SGMXASScanConfigurationView.h"

#include "util/AMDateTimeUtils.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QScrollArea>
#include <QLineEdit>

SGMXASScanConfigurationView::SGMXASScanConfigurationView(SGMXASScanConfiguration *configuration, QWidget *parent)
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
	dwellTime_->setValue(configuration_->scanAxes().size() > 0 ? double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime()) : 1.0);
	dwellTime_->setMinimumWidth(100);
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));

	// The estimated scan time.
	estimatedTime_ = new QLabel;
	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	onEstimatedTimeChanged();

	scanInformation_ = new QLabel("Scan Size:");

	QVBoxLayout *timeBoxLayout = new QVBoxLayout;
	timeBoxLayout->addWidget(estimatedTime_);
	timeBoxLayout->addWidget(scanInformation_);

	QGroupBox *timeGroupBox = new QGroupBox("Time");
	timeGroupBox->setLayout(timeBoxLayout);

	// Setup the group box for setting the start and end points.
	QGroupBox *positionsBox = new QGroupBox("Energy Axis");

	axisStart_ = createPositionDoubleSpinBox("Start: ", "", double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()), 3);
	axisEnd_ = createPositionDoubleSpinBox("End: ", "", double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd()), 3);

	connect(axisStart_, SIGNAL(editingFinished()), this, SLOT(onStartChanged()));
	connect(axisEnd_, SIGNAL(editingFinished()), this, SLOT(onEndChanged()));
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));

	AMScanAxisRegion *region = configuration_->scanAxisAt(0)->regionAt(0);
	connect(region, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(setStart(AMNumber)));
	connect(region, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(setEnd(AMNumber)));
	connect(region, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(setDwellTime(AMNumber)));

	QHBoxLayout *axisLayout = new QHBoxLayout;
	axisLayout->addWidget(axisStart_);
	axisLayout->addWidget(axisEnd_);
	axisLayout->addWidget(dwellTime_);

	QVBoxLayout *positionLayout = new QVBoxLayout;
	positionLayout->addLayout(axisLayout);

	positionsBox->setLayout(positionLayout);

	QGroupBox *detectorGroupBox = new QGroupBox("Detectors");
	detectorGroupBox->setFlat(true);
	detectorGroup_ = new QButtonGroup;
	detectorGroup_->setExclusive(false);
	detectorLayout_ = new QVBoxLayout;

	connect(detectorGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onDetectorSelectionChanged(QAbstractButton*)));

	detectorGroupBox->setLayout(detectorLayout_);

	updateScanInformation();

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

SGMXASScanConfigurationView::~SGMXASScanConfigurationView()
{

}

void SGMXASScanConfigurationView::setDetectors(AMDetectorSet *newDetectors)
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

void SGMXASScanConfigurationView::setStart(const AMNumber &value)
{
	axisStart_->setValue(double(value));
}

void SGMXASScanConfigurationView::setEnd(const AMNumber &value)
{
	axisEnd_->setValue(double(value));
}

void SGMXASScanConfigurationView::setDwellTime(const AMNumber &value)
{
	dwellTime_->setValue(double(value));
}

void SGMXASScanConfigurationView::onStartChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStart(axisStart_->value());
	updateScanInformation();
}

void SGMXASScanConfigurationView::onEndChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(axisEnd_->value());
	updateScanInformation();
}

void SGMXASScanConfigurationView::onDwellTimeChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionTime(dwellTime_->value());
	updateScanInformation();
}

void SGMXASScanConfigurationView::onScanNameEdited()
{
	QString name = scanName_->text();
	configuration_->setName(name);
	configuration_->setUserScanName(name);
}

void SGMXASScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + AMDateTimeUtils::convertTimeToString(configuration_->totalTime()));
}

void SGMXASScanConfigurationView::onDetectorSelectionChanged(QAbstractButton *button)
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

void SGMXASScanConfigurationView::updateScanInformation()
{
	double size = qAbs(double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()));
	double resolution = size/double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime());

	scanInformation_->setText(QString("Scan Size: %1eV\t Resolution: %2 eV/s")
				  .arg(size, 0, 'f', 1)
				  .arg(resolution, 0, 'f', 1));
}

QDoubleSpinBox *SGMXASScanConfigurationView::createPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals)
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

