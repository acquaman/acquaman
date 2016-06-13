#include "AMGenericStepScanAxisAbsoluteView.h"

AMGenericStepScanAxisAbsoluteView::AMGenericStepScanAxisAbsoluteView(AMGenericStepScanConfiguration *configuration, AMControlSet *controls, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	configuration_ = 0;
	controls_ = 0;

	// Create UI elements.

	axisControlChoice1_ = new QComboBox;
	connect( axisControlChoice1_, SIGNAL(currentIndexChanged(int)), this, SLOT(onAxisControlChoice1Changed()) );

	axisStart1_ = new QDoubleSpinBox();
	connect( axisStart1_, SIGNAL(valueChanged(double)), this, SLOT(onStart1Changed()) );

	axisStep1_ = new QDoubleSpinBox();
	connect( axisStep1_, SIGNAL(valueChanged(double)), this, SLOT(onStep1Changed()) );

	axisEnd1_ = new QDoubleSpinBox();
	connect( axisEnd1_, SIGNAL(valueChanged(double)), this, SLOT(onEnd1Changed()) );

	axisControlChoice2_ = new QComboBox;
	connect( axisControlChoice2_, SIGNAL(currentIndexChanged(int)), this, SLOT(onAxisControlChoice2Changed()) );

	axisStart2_ = new QDoubleSpinBox();
	connect( axisStart2_, SIGNAL(valueChanged(double)), this, SLOT(onStart2Changed()) );

	axisStep2_ = new QDoubleSpinBox();
	connect( axisStep2_, SIGNAL(valueChanged(double)), this, SLOT(onStep2Changed()) );

	axisEnd2_ = new QDoubleSpinBox();
	connect( axisEnd2_, SIGNAL(valueChanged(double)), this, SLOT(onEnd2Changed()) );

	// Create and set main layouts.

	QHBoxLayout *axis1Layout = new QHBoxLayout;
	axis1Layout->addWidget(axisControlChoice1_);
	axis1Layout->addWidget(axisStart1_);
	axis1Layout->addWidget(axisStep1_);
	axis1Layout->addWidget(axisEnd1_);

	QHBoxLayout *axis2Layout = new QHBoxLayout;
	axis2Layout->addWidget(axisControlChoice2_);
	axis2Layout->addWidget(axisStart2_);
	axis2Layout->addWidget(axisStep2_);
	axis2Layout->addWidget(axisEnd2_);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addLayout(axis1Layout);
	layout->addLayout(axis2Layout);

	setLayout(layout);

	// Current settings.

	setConfiguration(configuration);
	setControls(controls);
}

AMGenericStepScanAxisAbsoluteView::~AMGenericStepScanAxisAbsoluteView()
{

}

void AMGenericStepScanAxisAbsoluteView::setConfiguration(AMGenericStepScanConfiguration *configuration)
{
	if (configuration_ != configuration) {

		if (configuration_)
			disconnect( configuration_, 0, this, 0 );

		configuration_ = configuration;

		if (configuration_) {
			connect( configuration_, SIGNAL(scanAxisAdded(AMScanAxis*)), this, SLOT(updateControl1Box()) );
			connect( configuration_, SIGNAL(scanAxisRemoved(AMScanAxis*)), this, SLOT(updateControl1Box()) );
			connect( configuration_, SIGNAL(axisControlInfosChanged()), this, SLOT(updateControl1Box()) );
		}

		emit configurationChanged(configuration_);
	}

	updateControl1Box();
}

void AMGenericStepScanAxisAbsoluteView::setControls(AMControlSet *newControls)
{
	if (controls_ != newControls) {

		if (controls_)
			disconnect( controls_, 0, this, 0 );

		controls_ = newControls;

		if (controls_) {
			connect( controls_, SIGNAL(controlConnectedChanged(bool,AMControl*)), this, SLOT(updateControl1Box()) );
			connect( controls_->signalSource(), SIGNAL(itemAdded(int)), this, SLOT(updateControl1Box()) );
			connect( controls_->signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(updateControl1Box()) );
		}

		emit controlsChanged(controls_);
	}

	updateControl1Box();
}

void AMGenericStepScanAxisAbsoluteView::onAxisControlChoice1Changed()
{
	if (configuration_ && controls_) {
		AMControl *newControl = controls_->controlNamed(axisControlChoice1_->itemData(axisControlChoice1_->currentIndex()).toString());

		if (newControl)
			configuration_->setControl(0, newControl->toInfo());
		else
			configuration_->setControl(0, AMControlInfo());
	}
}

void AMGenericStepScanAxisAbsoluteView::updateControl1Box()
{
	axisControlChoice1_->blockSignals(true);

	axisControlChoice1_->clear();
	axisControlChoice1_->addItem("None");

	axisControlChoice1_->setEnabled(false);

	if (configuration_ && configuration_->scanAxes().size() > 0) {
		axisControlChoice1_->setEnabled(true);

		// Populate with axis control choices.

		for (int i = 0, count = controls_->count(); i < count; i++) {
			AMControl *control = controls_->at(i);

			if (control && control->isConnected())
				axisControlChoice1_->addItem(control->description().isEmpty() ? control->name() : control->description(), control->name());
		}

		// Set current index.

		if (configuration_->axisControlInfos().count() > 0)
			axisControlChoice1_->setCurrentIndex(axisControlChoice1_->findData(configuration_->axisControlInfoAt(0).name()));
	}

	axisControlChoice1_->blockSignals(false);

	// Update the start, step, and end boxes.

	updateStart1Box();
	updateStep1Box();
	updateEnd1Box();

	// Update the second axis control box.

	updateControl2Box();
}

void AMGenericStepScanAxisAbsoluteView::onStart1Changed()
{
	if (configuration_ && configuration_->scanAxes().size() > 0 && configuration_->scanAxisAt(0)->regionCount() > 0)
		configuration_->scanAxisAt(0)->regionAt(0)->setRegionStart(axisStart1_->value());
}

void AMGenericStepScanAxisAbsoluteView::onStep1Changed()
{
	if (configuration_ && configuration_->scanAxes().size() > 0 && configuration_->scanAxisAt(0)->regionCount() > 0)
		configuration_->scanAxisAt(0)->regionAt(0)->setRegionStep(axisStep1_->value());
}

void AMGenericStepScanAxisAbsoluteView::onEnd1Changed()
{
	if (configuration_ && configuration_->scanAxes().size() > 0 && configuration_->scanAxisAt(0)->regionCount() > 0)
		configuration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(axisEnd1_->value());
}

void AMGenericStepScanAxisAbsoluteView::updateStart1Box()
{
	axisStart1_->blockSignals(true);

	axisStart1_->clear();
	axisStart1_->setAlignment(Qt::AlignCenter);
	axisStart1_->setPrefix("Start: ");

	axisStart1_->setEnabled(false);

	if (configuration_ && configuration_->scanAxes().size() > 0 && configuration_->axisControlInfoAt(0).isValid()) {
		axisStart1_->setEnabled(true);
		axisStart1_->setRange(AMGENERICSTEPSCANAXISABSOLUTEVIEW_POSITION_MIN, AMGENERICSTEPSCANAXISABSOLUTEVIEW_POSITION_MAX);

		if (configuration_->scanAxisAt(0)->regionCount() > 0)
			axisStart1_->setValue(double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()));
	}

	axisStart1_->blockSignals(false);
}

void AMGenericStepScanAxisAbsoluteView::updateStep1Box()
{
	axisStep1_->blockSignals(true);

	axisStep1_->clear();
	axisStep1_->setAlignment(Qt::AlignCenter);
	axisStep1_->setPrefix("Step: ");

	axisStep1_->setEnabled(false);

	if (configuration_ && configuration_->scanAxes().size() > 0 && configuration_->axisControlInfoAt(0).isValid()) {
		axisStep1_->setEnabled(true);
		axisStep1_->setRange(AMGENERICSTEPSCANAXISABSOLUTEVIEW_POSITION_MIN, AMGENERICSTEPSCANAXISABSOLUTEVIEW_POSITION_MAX);

		if (configuration_->scanAxisAt(0)->regionCount() > 0)
			axisStep1_->setValue(double(configuration_->scanAxisAt(0)->regionAt(0)->regionStep()));
	}

	axisStep1_->blockSignals(false);
}

void AMGenericStepScanAxisAbsoluteView::updateEnd1Box()
{
	axisEnd1_->blockSignals(true);

	axisEnd1_->clear();
	axisEnd1_->setAlignment(Qt::AlignCenter);
	axisEnd1_->setPrefix("End: ");

	axisEnd1_->setEnabled(false);

	if (configuration_ && configuration_->scanAxes().size() > 0 && configuration_->axisControlInfoAt(0).isValid()) {
		axisEnd1_->setEnabled(true);
		axisEnd1_->setRange(AMGENERICSTEPSCANAXISABSOLUTEVIEW_POSITION_MIN, AMGENERICSTEPSCANAXISABSOLUTEVIEW_POSITION_MAX);

		if (configuration_->scanAxisAt(0)->regionCount() > 0)
			axisEnd1_->setValue(double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd()));
	}

	axisEnd1_->blockSignals(false);
}

void AMGenericStepScanAxisAbsoluteView::onAxisControlChoice2Changed()
{
	if (configuration_ && controls_) {
		AMControl *newControl = controls_->controlNamed(axisControlChoice2_->itemData(axisControlChoice2_->currentIndex()).toString());

		if (newControl)
			configuration_->setControl(1, newControl->toInfo());
		else
			configuration_->setControl(1, AMControlInfo());
	}
}

void AMGenericStepScanAxisAbsoluteView::updateControl2Box()
{
	axisControlChoice2_->blockSignals(true);

	axisControlChoice2_->clear();
	axisControlChoice2_->addItem("None");

	axisControlChoice2_->setEnabled(false);

	if (configuration_ && configuration_->scanAxes().size() > 1) {
		axisControlChoice2_->setEnabled(true);

		// Populate with axis control choices, leaving out the axis 1 control selection.

		for (int i = 0, count = controls_->count(); i < count; i++) {
			AMControl *control = controls_->at(i);

			if (control && control->isConnected() && axisControlChoice1_->findData(control->name()) == -1)
				axisControlChoice2_->addItem(control->description().isEmpty() ? control->name() : control->description(), control->name());
		}

		// Set current index.

		if (configuration_->axisControlInfos().count() > 1)
			axisControlChoice2_->setCurrentIndex(axisControlChoice1_->findData(configuration_->axisControlInfoAt(0).name()));
	}

	axisControlChoice2_->blockSignals(false);

	// Update the start, step, and end boxes.

	updateStart2Box();
	updateStep2Box();
	updateEnd2Box();
}

void AMGenericStepScanAxisAbsoluteView::onStart2Changed()
{
	if (configuration_ && configuration_->scanAxes().size() > 1 && configuration_->scanAxisAt(1)->regionCount() > 0)
		configuration_->scanAxisAt(1)->regionAt(0)->setRegionStart(axisStart2_->value());
}

void AMGenericStepScanAxisAbsoluteView::onStep2Changed()
{
	if (configuration_ && configuration_->scanAxes().size() > 1 && configuration_->scanAxisAt(1)->regionCount() > 0)
		configuration_->scanAxisAt(1)->regionAt(0)->setRegionStep(axisStep2_->value());
}

void AMGenericStepScanAxisAbsoluteView::onEnd2Changed()
{
	if (configuration_ && configuration_->scanAxes().size() > 1 && configuration_->scanAxisAt(1)->regionCount() > 0)
		configuration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(axisEnd2_->value());
}

void AMGenericStepScanAxisAbsoluteView::updateStart2Box()
{
	axisStart2_->blockSignals(true);

	axisStart2_->clear();
	axisStart2_->setAlignment(Qt::AlignCenter);
	axisStart2_->setPrefix("Start: ");

	axisStart2_->setEnabled(false);

	if (configuration_ && configuration_->scanAxes().size() > 1 && configuration_->axisControlInfoAt(1).isValid()) {
		axisStart2_->setEnabled(true);
		axisStart2_->setRange(AMGENERICSTEPSCANAXISABSOLUTEVIEW_POSITION_MIN, AMGENERICSTEPSCANAXISABSOLUTEVIEW_POSITION_MAX);

		if (configuration_->scanAxisAt(1)->regionCount() > 0)
			axisStart2_->setValue(double(configuration_->scanAxisAt(1)->regionAt(0)->regionStart()));
	}

	axisStart2_->blockSignals(false);
}

void AMGenericStepScanAxisAbsoluteView::updateStep2Box()
{
	axisStep2_->blockSignals(true);

	axisStep2_->clear();
	axisStep2_->setAlignment(Qt::AlignCenter);
	axisStep2_->setPrefix("Step: ");

	axisStep2_->setEnabled(false);

	if (configuration_ && configuration_->scanAxes().size() > 1 && configuration_->axisControlInfoAt(1).isValid()) {
		axisStep2_->setEnabled(true);
		axisStep2_->setRange(AMGENERICSTEPSCANAXISABSOLUTEVIEW_POSITION_MIN, AMGENERICSTEPSCANAXISABSOLUTEVIEW_POSITION_MAX);

		if (configuration_->scanAxisAt(1)->regionCount() > 0)
			axisStep2_->setValue(double(configuration_->scanAxisAt(1)->regionAt(0)->regionStep()));
	}

	axisStep2_->blockSignals(false);
}

void AMGenericStepScanAxisAbsoluteView::updateEnd2Box()
{
	axisEnd2_->blockSignals(true);

	axisEnd2_->clear();
	axisEnd2_->setAlignment(Qt::AlignCenter);
	axisEnd2_->setPrefix("End: ");

	axisEnd2_->setEnabled(false);

	if (configuration_ && configuration_->scanAxes().size() > 1 && configuration_->axisControlInfoAt(1).isValid()) {
		axisEnd2_->setEnabled(true);
		axisEnd2_->setRange(AMGENERICSTEPSCANAXISABSOLUTEVIEW_POSITION_MIN, AMGENERICSTEPSCANAXISABSOLUTEVIEW_POSITION_MAX);

		if (configuration_->scanAxisAt(1)->regionCount() > 0)
			axisEnd2_->setValue(double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd()));
	}

	axisEnd2_->blockSignals(false);
}
