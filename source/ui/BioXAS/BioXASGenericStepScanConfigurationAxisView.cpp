#include "BioXASGenericStepScanConfigurationAxisView.h"

BioXASGenericStepScanConfigurationAxisView::BioXASGenericStepScanConfigurationAxisView(AMGenericStepScanConfiguration *configuration, int axisNumber, BioXASScanAxisRegionView::ViewMode viewMode, AMControlSet *controls, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	viewMode_ = BioXASScanAxisRegionView::Absolute;
	configuration_ = 0;
	axisNumber_ = -1;
	controls_ = 0;

	// Create UI elements.

	controlsBox_ = new QComboBox();
	connect( controlsBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onControlsBoxCurrentIndexChanged()) );

	regionValueBox_ = new QDoubleSpinBox();
	connect( regionValueBox_, SIGNAL(valueChanged(double)), this, SLOT(onRegionValueBoxValueChanged()) );

	regionView_ = new BioXASScanAxisRegionView(0, viewMode_);

	// Create and set main layout.

	QHBoxLayout *upperLayout = new QHBoxLayout();
	upperLayout->addWidget(controlsBox_);
	upperLayout->addWidget(regionValueBox_);

	QHBoxLayout *lowerLayout = new QHBoxLayout();
	lowerLayout->addWidget(regionView_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(upperLayout);
	layout->addLayout(lowerLayout);

	setLayout(layout);

	// Current settings.

	setViewMode(viewMode);
	setConfiguration(configuration);
	setAxisNumber(axisNumber);
	setControls(controls);
}

BioXASGenericStepScanConfigurationAxisView::~BioXASGenericStepScanConfigurationAxisView()
{

}

void BioXASGenericStepScanConfigurationAxisView::setViewMode(BioXASScanAxisRegionView::ViewMode newMode)
{
	if (viewMode_ != newMode) {
		viewMode_ = newMode;
		emit viewModeChanged(viewMode_);
	}

	updateRegionValueBox();
	updateRegionView();
}

void BioXASGenericStepScanConfigurationAxisView::setConfiguration(AMGenericStepScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration) {

		if (configuration_)
			disconnect( configuration_, 0, this, 0 );

		configuration_ = newConfiguration;

		if (configuration_) {
			connect( configuration_, SIGNAL(scanAxisAdded(AMScanAxis*)), this, SLOT(updateRegionView()) );
			connect( configuration_, SIGNAL(scanAxisRemoved(AMScanAxis*)), this, SLOT(updateRegionView()) );
		}

		emit configurationChanged(configuration_);
	}

	updateRegionValueBox();
	updateRegionView();
}

void BioXASGenericStepScanConfigurationAxisView::setAxisNumber(int newNumber)
{
	if (axisNumber_ != newNumber) {
		axisNumber_ = newNumber;
		emit axisNumberChanged(axisNumber_);
	}

	updateRegionValueBox();
	updateRegionView();
}

void BioXASGenericStepScanConfigurationAxisView::setControls(AMControlSet *newControls)
{
	if (controls_ != newControls) {

		if (controls_)
			disconnect( controls_, 0, this, 0 );

		controls_ = newControls;

		if (controls_) {
			connect( controls_, SIGNAL(controlConnectedChanged(bool,AMControl*)), this, SLOT(updateControlsBox()) );
			connect( controls_->signalSource(), SIGNAL(itemAdded(int)), this, SLOT(updateControlsBox()) );
			connect( controls_->signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(updateControlsBox()) );
		}

		emit controlsChanged(controls_);
	}

	updateControlsBox();
}

void BioXASGenericStepScanConfigurationAxisView::updateControlsBox()
{
	// Initially, clear and disable the controls box.

	controlsBox_->blockSignals(false);

	controlsBox_->clear();
	controlsBox_->addItem("None");

	controlsBox_->setEnabled(false);

	if (controls_) {
		controlsBox_->setEnabled(true);

		// Populate with axis control choices.

		for (int i = 0, count = controls_->count(); i < count; i++) {
			AMControl *control = controls_->at(i);

			if (control && control->canMove())
				controlsBox_->addItem(control->description().isEmpty() ? control->name() : control->description(), control->name());
		}

		// Set current index.

		if (configuration_ && validAxis(configuration_, axisNumber_)) {
			int newIndex = controlsBox_->findData(configuration_->axisControlInfoAt(axisNumber_).name());

			if (newIndex >= 0)
				controlsBox_->setCurrentIndex(newIndex);
			else
				controlsBox_->setCurrentIndex(0);
		}
	}

	controlsBox_->blockSignals(false);
}

void BioXASGenericStepScanConfigurationAxisView::updateRegionValueBox()
{
	// Initially, clear and disable the value box.

	regionValueBox_->blockSignals(true);

	regionValueBox_->clear();
	regionValueBox_->setAlignment(Qt::AlignCenter);
	regionValueBox_->setPrefix("Value: ");
	regionValueBox_->setEnabled(false);

	// Update the box's visibility.

	if (viewMode_ == BioXASScanAxisRegionView::Absolute)
		regionValueBox_->hide();
	else if (viewMode_ == BioXASScanAxisRegionView::Relative)
		regionValueBox_->show();

	// Update the box's enabled state.

	if (validAxis(configuration_, axisNumber_) && validAxisRegion(configuration_, axisNumber_, 0)) {

		if (configuration_->axisControlInfoAt(axisNumber_).isValid()) {
			regionValueBox_->setMinimum(BIOXASGENERICSTEPSCANCONFIGURATIONAXISVIEW_REGION_VALUE_MIN);
			regionValueBox_->setMaximum(BIOXASGENERICSTEPSCANCONFIGURATIONAXISVIEW_REGION_VALUE_MAX);
			regionValueBox_->setValue(50);
			regionValueBox_->setEnabled(true);
		}
	}

	regionValueBox_->blockSignals(false);
}

void BioXASGenericStepScanConfigurationAxisView::updateRegionView()
{
	// Update the region view's region.

	if (validAxis(configuration_, axisNumber_) && configuration_->scanAxisAt(axisNumber_)->regionCount() > 0)
		regionView_->setRegion(configuration_->scanAxisAt(axisNumber_)->regionAt(0));
	else
		regionView_->setRegion(0);

	// Update the region view's view mode.

	regionView_->setViewMode(viewMode_);

	// Update the region view's base value.

	regionView_->setBaseValue(regionValueBox_->value());
}

void BioXASGenericStepScanConfigurationAxisView::onControlsBoxCurrentIndexChanged()
{
	qDebug() << "\n\nControls box value changed.";

	AMControl *newControl = 0;

	if (controls_)
		newControl = controls_->controlNamed(controlsBox_->itemData(controlsBox_->currentIndex()).toString());

	if (validAxis(configuration_, axisNumber_)) {
		qDebug() << "\tSetting control for axis number" << axisNumber_;
		if (newControl)
			configuration_->setControl(axisNumber_, newControl->toInfo());
		else
			configuration_->setControl(axisNumber_, AMControlInfo());
	}
}

void BioXASGenericStepScanConfigurationAxisView::onRegionValueBoxValueChanged()
{
	updateRegionView();
}

bool BioXASGenericStepScanConfigurationAxisView::validAxis(AMGenericStepScanConfiguration *configuration, int number) const
{
	return ((number > -1) && configuration && (number < configuration->scanAxes().count()) && (configuration->scanAxisAt(number)));
}

bool BioXASGenericStepScanConfigurationAxisView::validAxisRegion(AMGenericStepScanConfiguration *configuration, int axisNumber, int regionNumber) const
{
	return (validAxis(configuration, axisNumber) && (regionNumber > -1) && (regionNumber < configuration->scanAxisAt(axisNumber)->regionCount()) && configuration->scanAxisAt(axisNumber)->regionAt(regionNumber));
}

bool BioXASGenericStepScanConfigurationAxisView::validAxisControl(AMGenericStepScanConfiguration *configuration, int axisNumber) const
{
	return (validAxis(configuration, axisNumber) && (axisNumber < configuration_->axisControlInfos().count()) && configuration_->axisControlInfoAt(axisNumber).isValid());
}
