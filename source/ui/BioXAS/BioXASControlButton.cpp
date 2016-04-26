#include "BioXASControlButton.h"

BioXASControlButton::BioXASControlButton(AMControl *control, QWidget *parent) :
	AMToolButton(parent)
{
	// Initialize class variables.

	control_ = 0;

	greenValueSet_ = false;
	greenValue_ = 0;

	// Current settings.

	setColorState(Neutral);
	setControl(control);

	refresh();
}

BioXASControlButton::~BioXASControlButton()
{

}

void BioXASControlButton::refresh()
{
	if (control_) {
		if (greenValueSet_ && control_->isConnected() && qFuzzyCompare(control_->value(), greenValue_))
			setColorState(Good);
		else if (greenValueSet_ && control_->isConnected())
			setColorState(Bad);
		else if (greenValueSet_)
			setColorState(Neutral);
	}
}

void BioXASControlButton::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(refresh()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void BioXASControlButton::setGreenValue(double newValue)
{
	if (!greenValueSet_ || (greenValue_ != newValue)) {
		greenValueSet_ = true;
		greenValue_ = newValue;
		emit greenValueChanged(greenValue_);
	}
}

