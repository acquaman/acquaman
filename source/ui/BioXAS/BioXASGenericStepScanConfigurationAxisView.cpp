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
	connect( regionValueBox_, SIGNAL(valueChanged(double)), this, SLOT(updateRegionView()) );

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
			connect( configuration_, SIGNAL(axisControlInfosChanged()), this, SLOT(updateAxisView()) );
			connect( configuration_, SIGNAL(axisControlInfoAdded()), this, SLOT(updateAxisView()) );
			connect( configuration_, SIGNAL(axisControlInfoRemoved()), this, SLOT(updateAxisView()) );
			connect( configuration_, SIGNAL(axisControlInfoChanged()), this, SLOT(updateAxisView()) );
		}

		emit configurationChanged(configuration_);
	}

	updateControlsBox();
	updateRegionValueBox();
	updateRegionView();
}

void BioXASGenericStepScanConfigurationAxisView::setAxisNumber(int newNumber)
{
	if (axisNumber_ != newNumber) {
		axisNumber_ = newNumber;
		emit axisNumberChanged(axisNumber_);
	}

	updateControlsBox();
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

void BioXASGenericStepScanConfigurationAxisView::updateAxisView()
{
	qDebug() << "\n\nUpdating axis view.";
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
	controlsBox_->setCurrentIndex(0);

	controlsBox_->setEnabled(false);

	if (controls_ && validAxis(configuration_, axisNumber_)) {

		controlsBox_->setEnabled(true);

		// Populate with axis control choices. Disable choices that shouldn't be used.

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

	regionValueBox_->setAlignment(Qt::AlignCenter);
	regionValueBox_->setPrefix("Value: ");

	// Update the box's visibility.

	if (viewMode_ == BioXASScanAxisRegionView::Absolute)
		regionValueBox_->hide();
	else if (viewMode_ == BioXASScanAxisRegionView::Relative)
		regionValueBox_->show();

	// Update the box's enabled state.

	if (validAxisRegion(configuration_, axisNumber_, 0) && validAxisControl(configuration_, axisNumber_)) {
		regionValueBox_->setMinimum(BIOXASGENERICSTEPSCANCONFIGURATIONAXISVIEW_REGION_VALUE_MIN);
		regionValueBox_->setMaximum(BIOXASGENERICSTEPSCANCONFIGURATIONAXISVIEW_REGION_VALUE_MAX);
		regionValueBox_->setValue(0);
		regionValueBox_->setEnabled(true);
	}

	regionValueBox_->blockSignals(false);
}

void BioXASGenericStepScanConfigurationAxisView::updateRegionView()
{
	// Initially clear the region view.

	regionView_->setRegion(0);
	regionView_->setEnabled(false);

	// Update the view's region.

	if (validAxisRegion(configuration_, axisNumber_, 0) && validAxisControl(configuration_, axisNumber_)) {
		regionView_->setRegion(configuration_->scanAxisAt(axisNumber_)->regionAt(0));
		regionView_->setEnabled(true);
	}

	// Update the region view's base value.

	regionView_->setBaseValue(regionValueBox_->value());

	// Update the region view's view mode.

	regionView_->setViewMode(viewMode_);
}

void BioXASGenericStepScanConfigurationAxisView::onControlsBoxCurrentIndexChanged()
{
	qDebug() << "\n\nControls box value changed.";

	AMControl *newControl = 0;

	if (controls_)
		newControl = controls_->controlNamed(controlsBox_->itemData(controlsBox_->currentIndex()).toString());

	if (validAxis(configuration_, axisNumber_)) {
		qDebug() << "\tSetting control for axis number" << axisNumber_;
		if (newControl) {
			configuration_->setControl(axisNumber_, newControl->toInfo());

			if (validAxisRegion(configuration_, axisNumber_, 0)) {
				AMScanAxisRegion *region = configuration_->scanAxisAt(axisNumber_)->regionAt(0);

				region->setRegionStart(newControl->value());
				region->setRegionStep(1.0);
				region->setRegionEnd(newControl->value() + 10);
				region->setRegionTime(1.0);
			}

		} else {
			configuration_->setControl(axisNumber_, AMControlInfo());

			if (validAxisRegion(configuration_, axisNumber_, 0)) {
				AMScanAxisRegion *region = configuration_->scanAxisAt(axisNumber_)->regionAt(0);

				region->setRegionStart(-1);
				region->setRegionStep(-1);
				region->setRegionEnd(-1);
				region->setRegionTime(-1);
			}
		}
	}
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
