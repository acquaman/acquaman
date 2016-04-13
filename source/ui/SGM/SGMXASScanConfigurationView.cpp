#include "SGMXASScanConfigurationView.h"

#include "util/AMDateTimeUtils.h"
#include "beamline/SGM/energy/SGMGratingAngleControl.h"
#include "beamline/SGM/energy/SGMGratingTranslationStepControl.h"
#include "beamline/SGM/energy/SGMEnergyPosition.h"

#include <QGroupBox>
#include <QLineEdit>

SGMXASScanConfigurationView::SGMXASScanConfigurationView(SGMXASScanConfiguration *configuration, AMDetectorSet *detectors, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	configuration_ = configuration;
	detectors_ = 0;

	gratingAngleControl_ = new SGMGratingAngleControl(this);
	gratingTranslationStepControl_ = new SGMGratingTranslationStepControl(this);

	connect(gratingTranslationStepControl_, SIGNAL(valueChanged(double)), this, SLOT(updateTimeBounds()));
	connect(gratingTranslationStepControl_, SIGNAL(connected(bool)), this, SLOT(onGratingControlsConnected(bool)));
	connect(gratingAngleControl_, SIGNAL(connected(bool)), this, SLOT(onGratingControlsConnected(bool)));

	timeBoundsLabel_ = new QLabel();

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
	QGroupBox *positionsBox = new QGroupBox("Energy Axis");

	axisStart_ = createPositionDoubleSpinBox("Start: ", " eV", double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()), 2);
	axisEnd_ = createPositionDoubleSpinBox("End: ", " eV", double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd()), 2);

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

	automaticDirectionAssessmentCheckBox_ = new QCheckBox("Automatic Up/Down Scanning");
	increaseRadioButton_ = new QRadioButton("Increase");
	decreaseRadioButton_ = new QRadioButton("Decrease");

	connect(automaticDirectionAssessmentCheckBox_, SIGNAL(toggled(bool)), configuration_, SLOT(setAutomaticDirectionAssessment(bool)));
	connect(configuration_, SIGNAL(automaticDirectionAssessmentChanged(bool)), automaticDirectionAssessmentCheckBox_, SLOT(setChecked(bool)));
	connect(automaticDirectionAssessmentCheckBox_, SIGNAL(toggled(bool)), increaseRadioButton_, SLOT(setHidden(bool)));
	connect(automaticDirectionAssessmentCheckBox_, SIGNAL(toggled(bool)), decreaseRadioButton_, SLOT(setHidden(bool)));
	connect(increaseRadioButton_, SIGNAL(toggled(bool)), this, SLOT(onDirectionChanged()));
	connect(decreaseRadioButton_, SIGNAL(toggled(bool)), this, SLOT(onDirectionChanged()));
	connect(configuration_, SIGNAL(directionChanged(AMScanConfiguration::Direction)), this, SLOT(setDirection(AMScanConfiguration::Direction)));

	automaticDirectionAssessmentCheckBox_->setChecked(configuration_->automaticDirectionAssessment());
	increaseRadioButton_->setChecked(configuration_->direction() == AMScanConfiguration::Increase);

	QVBoxLayout *directionLayout = new QVBoxLayout;
	directionLayout->addWidget(automaticDirectionAssessmentCheckBox_);
	directionLayout->addWidget(increaseRadioButton_);
	directionLayout->addWidget(decreaseRadioButton_);

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
	layout->addWidget(timeBoundsLabel_);
	layout->addWidget(scanName_);
	layout->addWidget(timeGroupBox);
	layout->insertStretch(2,0);

	QHBoxLayout *moreLayout = new QHBoxLayout;
	moreLayout->addStretch();
	moreLayout->addLayout(layout);
	moreLayout->addWidget(detectorGroupBox);
	moreLayout->addStretch();

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addStretch();
	configViewLayout->addLayout(moreLayout);
	configViewLayout->addLayout(directionLayout);
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
	updateTimeBounds();
}

void SGMXASScanConfigurationView::setEnd(const AMNumber &value)
{
	axisEnd_->setValue(double(value));
	updateTimeBounds();
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

void SGMXASScanConfigurationView::onEstimatedTimeChanged(double newTime)
{
	estimatedTime_->setText("Estimated time per scan:\t" + AMDateTimeUtils::convertTimeToString(newTime));
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

void SGMXASScanConfigurationView::onDirectionChanged()
{
	if (increaseRadioButton_->isChecked())
		configuration_->setDirection(AMScanConfiguration::Increase);

	else if (decreaseRadioButton_->isChecked())
		configuration_->setDirection(AMScanConfiguration::Decrease);
}

void SGMXASScanConfigurationView::setDirection(AMScanConfiguration::Direction newDirection)
{
	if (newDirection == AMScanConfiguration::Increase)
		increaseRadioButton_->setChecked(true);

	else if (newDirection == AMScanConfiguration::Decrease)
		decreaseRadioButton_->setChecked(true);
}

void SGMXASScanConfigurationView::updateTimeBounds()
{
	if(gratingAngleControl_->isConnected() &&
	        gratingTranslationStepControl_->isConnected()) {

		SGMGratingSupport::GratingTranslation currentTranslation = SGMGratingSupport::encoderCountToEnum(gratingTranslationStepControl_->value());

		if(currentTranslation != SGMGratingSupport::UnknownGrating) {

			AMRange timeBounds = gratingAngleControl_->timeBoundsForEnergyMove(axisStart_->value(),
			                                                                   axisEnd_->value(),
			                                                                   currentTranslation);

			if(timeBounds.isValid()) {

				if(dwellTime_->value() > timeBounds.maximum()) {

					dwellTime_->setValue(timeBounds.maximum());

				} else if(dwellTime_->value() < timeBounds.minimum()) {

					dwellTime_->setValue(timeBounds.minimum());
				}

				dwellTime_->setRange(timeBounds.minimum(), timeBounds.maximum());

				timeBoundsLabel_->setText(QString("Valid times for energy range are: Min:%3s  Max:%4s")
										  .arg(timeBounds.minimum(), 0, 'f', 2)
										  .arg(timeBounds.maximum(), 0, 'f', 2));
			} else {
				// Time bounds were not valid
				timeBoundsLabel_ ->setText("Energy range is not valid");
			}

		} else {
			// Grating translation is unknown
			timeBoundsLabel_->setText("Grating selection is moving, please wait till complete before selecting scan parameters");
		}
	}
}

void SGMXASScanConfigurationView::onGratingControlsConnected(bool /*isConnected*/)
{
	if(gratingAngleControl_->isConnected() &&
	        gratingTranslationStepControl_->isConnected()) {

		updateTimeBounds();
		timeBoundsLabel_->setVisible(true);
	} else {

		timeBoundsLabel_->setVisible(false);
	}
}

void SGMXASScanConfigurationView::updateScanInformation()
{
	double size = qAbs(double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()));
	double resolution = size/double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime());

	scanInformation_->setText(QString("Scan Size: %1eV\t Resolution: %2 eV/s")
				  .arg(size, 0, 'f', 1)
				  .arg(resolution, 0, 'f', 2));
}

QDoubleSpinBox *SGMXASScanConfigurationView::createPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals)
{
	QDoubleSpinBox *box = new QDoubleSpinBox;
	box->setPrefix(prefix);
	box->setRange(SGMENERGY_MIN_VALUE, SGMENERGY_MAX_VALUE);
	box->setSuffix(suffix);
	box->setDecimals(decimals);
	box->setValue(value);
	box->setAlignment(Qt::AlignCenter);

	return box;
}


