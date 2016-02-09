#include "BioXASSIS3820ScalerModesView.h"
#include "beamline/BioXAS/BioXASSIS3820Scaler.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASSIS3820ScalerModesView::BioXASSIS3820ScalerModesView(BioXASSIS3820Scaler *scaler, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	scaler_ = 0;

	// Create UI elements.

	inputsModeEditor_ = new AMExtendedControlEditor(0);
	inputsModeEditor_->setTitle("Input mode");
	inputsModeEditor_->setNoUnitsBox(true);
	inputsModeEditor_->setMinimumWidth(150);

	triggerSourceModeEditor_ = new AMExtendedControlEditor(0);
	triggerSourceModeEditor_->setTitle("Trigger source");
	triggerSourceModeEditor_->setNoUnitsBox(true);
	triggerSourceModeEditor_->setMinimumWidth(150);

	clockSourceModeEditor_ = new AMExtendedControlEditor(0);
	clockSourceModeEditor_->setTitle("Clock source");
	clockSourceModeEditor_->setNoUnitsBox(true);
	clockSourceModeEditor_->setMinimumWidth(150);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(inputsModeEditor_);
	layout->addWidget(triggerSourceModeEditor_);
	layout->addWidget(clockSourceModeEditor_);

	setLayout(layout);

	// Current settings.

	setScaler(scaler);
}

BioXASSIS3820ScalerModesView::~BioXASSIS3820ScalerModesView()
{

}

void BioXASSIS3820ScalerModesView::refresh()
{
	// Clear the view.

	inputsModeEditor_->setControl(0);
	triggerSourceModeEditor_->setControl(0);
	clockSourceModeEditor_->setControl(0);

	// Update view elements.

	if (scaler_) {
		inputsModeEditor_->setControl(scaler_->inputsModeControl());
		triggerSourceModeEditor_->setControl(scaler_->triggerSourceModeControl());
		clockSourceModeEditor_->setControl(scaler_->clockSourceModeControl());
	}
}

void BioXASSIS3820ScalerModesView::setScaler(BioXASSIS3820Scaler *newScaler)
{
	if (scaler_ != newScaler) {
		scaler_ = newScaler;
		refresh();

		emit scalerChanged(scaler_);
	}
}

