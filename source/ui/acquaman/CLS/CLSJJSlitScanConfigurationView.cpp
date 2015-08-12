#include "CLSJJSlitScanConfigurationView.h"

#include "beamline/CLS/CLSBeamline.h"

CLSJJSlitScanConfigurationView::CLSJJSlitScanConfigurationView(CLSJJSlitScanConfiguration *configuration, QWidget *parent) :
    AMScanConfigurationView(parent)
{
	// Initialize class variables.

	configuration_ = configuration;
	direction_ = CLSJJSlits::Direction::Invalid;
	property_ = CLSJJSlits::Property::Invalid;

	// Create JJ slit direction options.

	QButtonGroup *directionOptionsGroup = new QButtonGroup();

	QRadioButton *verticalOption = new QRadioButton("Vertical");
	directionOptionsGroup->addButton(verticalOption);

	QRadioButton *horizontalOption = new QRadioButton("Horizontal");
	directionOptionsGroup->addButton(horizontalOption);

	QVBoxLayout *directionOptionsLayout = new QVBoxLayout();
	directionOptionsLayout->addWidget(verticalOption);
	directionOptionsLayout->addWidget(horizontalOption);

	QGroupBox *directionOptions = new QGroupBox("Blades Options");
	directionOptions->setFlat(true);
	directionOptions->setLayout(directionOptionsLayout);

	// Create JJ slit property options.

	QButtonGroup *propertyOptionsGroup = new QButtonGroup();

	QRadioButton *gapOption = new QRadioButton("Gap");
	propertyOptionsGroup->addButton(gapOption);

	QRadioButton *centerOption = new QRadioButton("Center");
	propertyOptionsGroup->addButton(centerOption);

	QVBoxLayout *propertyOptionsLayout = new QVBoxLayout();
	propertyOptionsLayout->addWidget(gapOption);
	propertyOptionsLayout->addWidget(centerOption);

	QGroupBox *propertyOptions = new QGroupBox("Property Options");
	propertyOptions->setFlat(true);
	propertyOptions->setLayout(propertyOptionsLayout);

	// Create detector options.

	QVBoxLayout *detectorLayout = new QVBoxLayout;
	QButtonGroup *detectorGroup = new QButtonGroup;
	detectorGroup->setExclusive(false);

	AMDetectorSet *detectors = AMBeamline::bl()->exposedDetectors();
	for (int i = 0, size = detectors->count(); i < size; i++){

		QCheckBox *checkBox = new QCheckBox(detectors->at(i)->name());
		detectorGroup->addButton(checkBox);
		detectorLayout->addWidget(checkBox);

		if (configuration_->detectorConfigurations().contains(detectors->at(i)->name()))
			checkBox->setChecked(true);
	}

	QGroupBox *detectorGroupBox = new QGroupBox("Detectors");
	detectorGroupBox->setLayout(detectorLayout);
	detectorGroupBox->setFlat(true);

	// Create and set main layouts.

	QVBoxLayout *optionsLayout = new QVBoxLayout();
	optionsLayout->addWidget(directionOptions);
	optionsLayout->addWidget(propertyOptions);

	QScrollArea *detectorScrollArea = new QScrollArea;
	detectorScrollArea->setWidget(detectorGroupBox);
	detectorScrollArea->setFrameStyle(QFrame::NoFrame);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addLayout(optionsLayout);
	layout->addWidget(detectorScrollArea);

	setLayout(layout);

	// Initial settings.

	direction_ = CLSJJSlits::Direction::Vertical;
	verticalOption->setChecked(true);

	property_ = CLSJJSlits::Property::Gap;
	gapOption->setChecked(true);

	updateScanConfigurationControls(direction_, property_);

	// Make connections.

	connect( directionOptionsGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onDirectionSelectionChanged(QAbstractButton*)) );
	connect( propertyOptionsGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onPropertySelectionChanged(QAbstractButton*)) );
	connect( detectorGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onDetectorSelectionChanged(QAbstractButton*)) );
}

CLSJJSlitScanConfigurationView::~CLSJJSlitScanConfigurationView()
{

}

void CLSJJSlitScanConfigurationView::onDirectionSelectionChanged(QAbstractButton *button)
{
	if (button) {

		// If the given button is checked, the corresponding direction should be added to the configuration.
		// Removed if unchecked.

		CLSJJSlits::Direction::Option direction = CLSJJSlits::Direction::Invalid;

		if (button->isChecked()) {
			direction = CLSJJSlits::stringToDirection(button->text());
		}

		updateScanConfigurationControls(direction, property_);
	}
}

void CLSJJSlitScanConfigurationView::onPropertySelectionChanged(QAbstractButton *button)
{
	if (button) {

		// If the given button is checked, the corresponding property should be added to the configuration.
		// Removed if unchecked.

		CLSJJSlits::Property::Option property = CLSJJSlits::Property::Invalid;

		if (button->isChecked())
			property = CLSJJSlits::stringToProperty(button->text());

		updateScanConfigurationControls(direction_, property);
	}
}

void CLSJJSlitScanConfigurationView::onDetectorSelectionChanged(QAbstractButton *button)
{
	if (button) {

		// If the given button is checked, the corresponding detector should be added to the configuration.
		// Removed if unchecked.

		if (button->isChecked())
			configuration_->addDetector(AMBeamline::bl()->exposedDetectorByName(button->text())->toInfo());
		else
			configuration_->removeDetector(AMBeamline::bl()->exposedDetectorByName(button->text())->toInfo());
	}
}

void CLSJJSlitScanConfigurationView::updateScanConfigurationControls(CLSJJSlits::Direction::Option direction, CLSJJSlits::Property::Option property)
{
	CLSJJSlits *jjSlits = CLSBeamline::clsBeamline()->jjSlits();

	if (jjSlits) {
		AMControl *control = jjSlits->control(direction, property);

		if (control)
			configuration_->setControl(0, control->toInfo());
		else
			configuration_->removeControl(0);
	}
}
