#include "AMGenericStepScanConfigurationDetectorsView.h"

AMGenericStepScanConfigurationDetectorsView::AMGenericStepScanConfigurationDetectorsView(AMGenericStepScanConfiguration *configuration, AMDetectorSet *detectors, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	// Initialize class variables.

	configuration_ = 0;
	detectors_ = 0;

	// Create UI elements.

	detectorsButtonGroup_ = new QButtonGroup();
	detectorsButtonGroup_->setExclusive(false);

	// Create and set layouts.

	layout_ = new QVBoxLayout();
	layout_->setMargin(0);
	setLayout(layout_);

	// Make connections.

	connect( detectorsButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onDetectorSelectionChanged(int)) );

	// Current settings.

	setConfiguration(configuration);
	setDetectors(detectors);
}

AMGenericStepScanConfigurationDetectorsView::~AMGenericStepScanConfigurationDetectorsView()
{

}

const AMScanConfiguration* AMGenericStepScanConfigurationDetectorsView::configuration() const
{
	return configuration_;
}

void AMGenericStepScanConfigurationDetectorsView::setConfiguration(AMGenericStepScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration) {

		if (configuration_) {
			disconnect( configuration_, 0, this, 0 );
		}

		configuration_ = newConfiguration;

		if (configuration_) {
			connect( configuration_, SIGNAL(detectorConfigurationsChanged()), this, SLOT(refresh()) );
		}

		refresh();

		emit configurationChanged(configuration_);
	}
}

void AMGenericStepScanConfigurationDetectorsView::setDetectors(AMDetectorSet *newDetectors)
{
	if (detectors_ != newDetectors) {

		if (detectors_) {
			disconnect( detectors_, 0, this, 0 );
		}

		detectors_ = newDetectors;

		if (detectors_) {
			connect( detectors_, SIGNAL(detectorAdded(int)), this, SLOT(refresh()) );
			connect( detectors_, SIGNAL(detectorConnectedChanged(bool,AMDetector*)), this, SLOT(update()) );
			connect( detectors_, SIGNAL(detectorRemoved(int)), this, SLOT(refresh()) );
		}

		refresh();

		emit detectorsChanged(detectors_);
	}
}

void AMGenericStepScanConfigurationDetectorsView::clear()
{
	for (int buttonIndex = 0, buttonCount = detectorsButtonGroup_->buttons().count(); buttonIndex < buttonCount; buttonIndex++) {
		QAbstractButton *button = detectorsButtonGroup_->button(buttonIndex);

		layout_->removeWidget(button);
		detectorsButtonGroup_->removeButton(button);

		button->deleteLater();
	}
}

void AMGenericStepScanConfigurationDetectorsView::update()
{
	if (configuration_ && detectors_) {

		// Iterate through existing buttons to make sure they accurately reflect present detector information.

		detectorsButtonGroup_->blockSignals(true);

		for (int detectorIndex = 0, detectorCount = detectors_->count(); detectorIndex < detectorCount; detectorIndex++) {

			QAbstractButton *button = detectorsButtonGroup_->button(detectorIndex);
			AMDetector *detector = detectors_->detectorAt(detectorIndex);

			if (detector && button) {

				bool detectorConnected = detector->isConnected();
				bool detectorUsed = configuration_->detectorConfigurations().contains(detector->name());

				// If the detector is not connected, button should be disabled.

				if (detectorConnected)
					button->setEnabled(true);
				else
					button->setEnabled(false);

				// If the detector is used in the configuration, button should be checked.

				if (detectorUsed)
					button->setChecked(true);
				else
					button->setChecked(false);
			}
		}

		detectorsButtonGroup_->blockSignals(false);
	}
}

void AMGenericStepScanConfigurationDetectorsView::refresh()
{
	// Clear the view.

	clear();

	// Construct new view, according to present detector information.

	if (detectors_) {

		for (int detectorIndex = 0, detectorCount = detectors_->count(); detectorIndex < detectorCount; detectorIndex++) {

			QString detectorName = detectors_->at(detectorIndex)->name();

			QCheckBox *checkBox = new QCheckBox(detectorName);
			detectorsButtonGroup_->addButton(checkBox, detectorIndex);
			layout_->addWidget(checkBox);
		}
	}

	// Update the view, so the correct buttons are checked and enabled.

	update();
}

void AMGenericStepScanConfigurationDetectorsView::onDetectorSelectionChanged(int detectorIndex)
{
	QAbstractButton *button = detectorsButtonGroup_->button(detectorIndex);
	AMDetector *detector = detectors_->detectorAt(detectorIndex);

	// If the button is checked and corresponds to a detector, make sure that detector is
	// added to the configuration. Remove the detector if its button is unchecked.

	if (button && detector) {
		if (button->isChecked())
			configuration_->addDetector(detector->toInfo());
		else
			configuration_->removeDetector(detector->toInfo());
	}
}
