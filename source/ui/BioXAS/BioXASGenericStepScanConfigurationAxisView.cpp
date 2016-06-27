#include "BioXASGenericStepScanConfigurationAxisView.h"

BioXASGenericStepScanConfigurationAxisView::BioXASGenericStepScanConfigurationAxisView(AMGenericStepScanConfiguration *configuration, int axisNumber, BioXASScanAxisRegionView::ViewMode viewMode, AMControlSet *controls, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	viewMode_ = BioXASScanAxisRegionView::Absolute;
	configuration_ = 0;
	axisNumber_ = -1;
	controls_ = 0;
	regionValue_ = AMNumber(AMNumber::InvalidError);

	// Create UI elements.

	controlsBox_ = new QComboBox();
	connect( controlsBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onControlsBoxCurrentIndexChanged()) );

	regionValueBox_ = new QDoubleSpinBox();
	regionValueBox_->setAlignment(Qt::AlignCenter);
	regionValueBox_->setPrefix("Value: ");
	regionValueBox_->setMinimum(BIOXASGENERICSTEPSCANCONFIGURATIONAXISVIEW_REGION_VALUE_MIN);
	regionValueBox_->setMaximum(BIOXASGENERICSTEPSCANCONFIGURATIONAXISVIEW_REGION_VALUE_MAX);

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
			connect( configuration_, SIGNAL(scanAxisAdded(AMScanAxis*)), this, SLOT(updateAxisView()) );
			connect( configuration_, SIGNAL(scanAxisRemoved(AMScanAxis*)), this, SLOT(updateAxisView()) );

			connect( configuration_, SIGNAL(axisControlInfosChanged()), this, SLOT(updateRegionValue()) );
			connect( configuration_, SIGNAL(axisControlInfoAdded()), this, SLOT(updateRegionValue()) );
			connect( configuration_, SIGNAL(axisControlInfoRemoved()), this, SLOT(updateRegionValue()) );
			connect( configuration_, SIGNAL(axisControlInfoChanged()), this, SLOT(updateRegionValue()) );

			connect( configuration_, SIGNAL(axisControlInfosChanged()), this, SLOT(updateAxisView()) );
			connect( configuration_, SIGNAL(axisControlInfoAdded()), this, SLOT(updateAxisView()) );
			connect( configuration_, SIGNAL(axisControlInfoRemoved()), this, SLOT(updateAxisView()) );
			connect( configuration_, SIGNAL(axisControlInfoChanged()), this, SLOT(updateAxisView()) );
		}

		emit configurationChanged(configuration_);
	}

	updateRegionValue();
	updateAxisView();
}

void BioXASGenericStepScanConfigurationAxisView::setAxisNumber(int newNumber)
{
	if (axisNumber_ != newNumber) {
		axisNumber_ = newNumber;
		emit axisNumberChanged(axisNumber_);
	}

	updateRegionValue();
	updateAxisView();
}

void BioXASGenericStepScanConfigurationAxisView::setControls(AMControlSet *newControls)
{
	if (controls_ != newControls) {

		if (controls_)
			disconnect( controls_, 0, this, 0 );

		controls_ = newControls;

		if (controls_) {
			connect( controls_, SIGNAL(controlConnectedChanged(bool,AMControl*)), this, SLOT(updateControlsBox()) );

			connect( controls_->signalSource(), SIGNAL(itemAdded(int)), this, SLOT(updateRegionValue()) );
			connect( controls_->signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(updateRegionValue()) );

			connect( controls_->signalSource(), SIGNAL(itemAdded(int)), this, SLOT(updateControlsBox()) );
			connect( controls_->signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(updateControlsBox()) );
		}

		emit controlsChanged(controls_);
	}

	updateRegionValue();
	updateControlsBox();
}

void BioXASGenericStepScanConfigurationAxisView::setRegionValue(const AMNumber &newValue)
{
	if (regionValue_ != newValue) {
		regionValue_ = newValue;
		emit regionValueChanged(regionValue_);
	}

	updateRegionValueBox();
	updateRegionView();
}

void BioXASGenericStepScanConfigurationAxisView::updateRegionValue()
{
	AMNumber newValue = AMNumber(AMNumber::InvalidError);

	if (controls_ && validAxisControl(configuration_, axisNumber_)) {
		AMControl *axisControl = controls_->controlNamed(configuration_->axisControlInfoAt(axisNumber_).name());

		if (axisControl)
			newValue = AMNumber(axisControl->value());
	}

	setRegionValue(newValue);
}

void BioXASGenericStepScanConfigurationAxisView::updateAxisView()
{
	updateControlsBox();
	updateRegionValueBox();
	updateRegionView();
}

void BioXASGenericStepScanConfigurationAxisView::updateControlsBox()
{
	controlsBox_->blockSignals(true);

	// Initially, clear and disable the controls box.

	controlsBox_->clear();

	controlsBox_->addItem("None");

	controlsBox_->setEnabled(false);

	if (controls_ && validAxis(configuration_, axisNumber_)) {

		controlsBox_->setEnabled(true);

		// Populate with axis control choices. Ignore controls that can't move, and disable choices that are in use on another scan axis.

		for (int i = 0, count = controls_->count(); i < count; i++) {
			AMControl *control = controls_->at(i);

			if (control && control->canMove()) {
				controlsBox_->addItem(control->description().isEmpty() ? control->name() : control->description(), control->name());

				if (configuration_->axisControlInfos().hasControl(control->name()) && configuration_->indexOfAxisControlInfo(control->name()) != axisNumber_) {
					QStandardItemModel *comboBoxModel = qobject_cast<QStandardItemModel*>(controlsBox_->model());
					QStandardItem *comboBoxModelItem = comboBoxModel->item(comboBoxModel->rowCount() - 1);

					if (comboBoxModelItem)
						comboBoxModelItem->setFlags(Qt::NoItemFlags);
				}
			}
		}

		// Set current index.

		if (validAxisControl(configuration_, axisNumber_))
			controlsBox_->setCurrentIndex(controlsBox_->findData(configuration_->axisControlInfoAt(axisNumber_).name()));
	}

	controlsBox_->blockSignals(false);
}

void BioXASGenericStepScanConfigurationAxisView::updateRegionValueBox()
{
	regionValueBox_->blockSignals(true);

	// Initially, clear and disable the value box.

	regionValueBox_->clear();
	regionValueBox_->setEnabled(false);

	// Update the box's value and enabled state.

	if (regionValue_.isValid() && validAxisControl(configuration_, axisNumber_)) {
		regionValueBox_->setValue(double(regionValue_));
		regionValueBox_->setEnabled(true);
	}

	// Update the box's visibility.

	if (viewMode_ == BioXASScanAxisRegionView::Absolute)
		regionValueBox_->hide();
	else if (viewMode_ == BioXASScanAxisRegionView::Relative)
		regionValueBox_->show();

	regionValueBox_->blockSignals(false);
}

void BioXASGenericStepScanConfigurationAxisView::updateRegionView()
{
	regionView_->blockSignals(true);

	// Initially, clear and disable the region view.

	regionView_->clear();
	regionView_->setEnabled(false);

	// Update the view's region, base value, and enabled state.

	if (regionValue_.isValid() && validAxisControl(configuration_, axisNumber_)) {
		regionView_->setBaseValue(regionValue_);
		regionView_->setRegion(configuration_->scanAxisAt(axisNumber_)->regionAt(0));
		regionView_->setEnabled(true);
	}

	// Update the region view's view mode.

	regionView_->setViewMode(viewMode_);

	regionView_->blockSignals(false);
}

void BioXASGenericStepScanConfigurationAxisView::onControlsBoxCurrentIndexChanged()
{
	AMControl *newControl = 0;

	if (controls_)
		newControl = controls_->controlNamed(controlsBox_->itemData(controlsBox_->currentIndex()).toString());

	if (validAxis(configuration_, axisNumber_)) {
		if (newControl) {
			configuration_->setControl(axisNumber_, newControl->toInfo());

			if (validAxisRegion(configuration_, axisNumber_, 0)) {
				AMScanAxisRegion *region =  configuration_->scanAxisAt(axisNumber_)->regionAt(0);

				region->setRegionStart(newControl->value() - 5);
				region->setRegionStep(1.0);
				region->setRegionEnd(newControl->value() + 5);
				region->setRegionTime(1.0);
			}

		} else {

			configuration_->setControl(axisNumber_, AMControlInfo());
		}
	}

	updateRegionValue();
}

void BioXASGenericStepScanConfigurationAxisView::onRegionValueBoxValueChanged()
{
	setRegionValue(regionValueBox_->value());
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
	return (validAxis(configuration, axisNumber) && (axisNumber < configuration->axisControlInfos().count()) && configuration->axisControlInfoAt(axisNumber).isValid());
}
