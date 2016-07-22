#include "AMGenericStepScanConfigurationDetectorsView.h"

AMGenericStepScanConfigurationDetectorsView::AMGenericStepScanConfigurationDetectorsView(AMGenericStepScanConfiguration *configuration, AMDetectorSet *detectors, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	// Initialize class variables.

	configuration_ = 0;
	detectors_ = 0;

	// Create UI elements.

	buttonGroup_ = new QButtonGroup();
	buttonGroup_->setExclusive(false);

	// Create and set layouts.

	layout_ = new QVBoxLayout();
	layout_->setMargin(0);
	setLayout(layout_);

	// Make connections.

	connect( buttonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onButtonSelectionChanged(int)) );

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
			connect( configuration_, SIGNAL(detectorsChanged()), this, SLOT(refresh()) );
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
	// Clear the detector/button mapping.

	detectorButtonMap_.clear();

	// Iterate through all buttons and remove them from the layout and the
	// button group.

	for (int buttonIndex = 0, buttonCount = buttonGroup_->buttons().count(); buttonIndex < buttonCount; buttonIndex++) {
		QAbstractButton *button = buttonGroup_->button(buttonIndex);

		if (button) {
			layout_->removeWidget(button);
			buttonGroup_->removeButton(button);

			button->deleteLater();
		}
	}
}

void AMGenericStepScanConfigurationDetectorsView::update()
{
	// Iterate through existing buttons to make sure they accurately reflect present detector information.

	buttonGroup_->blockSignals(true);

	for (int buttonIndex = 0, buttonCount = buttonGroup_->buttons().count(); buttonIndex < buttonCount; buttonIndex++) {

		QAbstractButton *button = buttonGroup_->button(buttonIndex);
		AMDetector *detector = detectorButtonMap_.key(button, 0);

		if (button) {

			button->blockSignals(true);

			// Buttons are unchecked and disabled by default.

			button->setChecked(false);
			button->setEnabled(false);

			if (configuration_ && detector) {

				// If the detector is connected, the button should be listed as normal.
				// If the detector is connected and used in the configuration, button should be checked.

				if (detector->isConnected()) {
					button->setChecked(configuration_->detectorConfigurations().contains(detector->name()));
					button->setEnabled(true);
				}
			}

			button->blockSignals(false);
		}
	}

	buttonGroup_->blockSignals(false);
}

void AMGenericStepScanConfigurationDetectorsView::refresh()
{
	// Clear the view.

	clear();

	// Construct new view, according to present detector information.

	if (detectors_) {

		for (int detectorIndex = 0, detectorCount = detectors_->count(); detectorIndex < detectorCount; detectorIndex++) {

			AMDetector *detector = detectors_->at(detectorIndex);

			if (detector) {

				// Create new checkbox for the detector.

				QString detectorText = (detector->description().isEmpty() ? detector->name() : detector->description());

				QCheckBox *checkBox = new QCheckBox(detectorText);
				buttonGroup_->addButton(checkBox, detectorIndex);
				layout_->addWidget(checkBox);

				// Add detector and checkbox mapping.

				detectorButtonMap_.insert(detector, checkBox);
			}
		}
	}

	// Update the view, so the correct buttons are enabled and checked.

	update();
}

void AMGenericStepScanConfigurationDetectorsView::onButtonSelectionChanged(int buttonIndex)
{
	QAbstractButton *button = buttonGroup_->button(buttonIndex);
	AMDetector *detector = detectorButtonMap_.key(button, 0);

	// If the button is checked and corresponds to a detector, make sure that detector is
	// added to the configuration. Remove the detector if its button is unchecked.

	if (configuration_ && button && detector) {
		if (button->isChecked())
			configuration_->addDetector(detector->toInfo());
		else
			configuration_->removeDetector(detector->toInfo());
	}
}
