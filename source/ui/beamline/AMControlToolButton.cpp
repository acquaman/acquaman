#include "AMControlToolButton.h"

AMControlToolButton::AMControlToolButton(AMControl *control, QWidget *parent) :
    AMToolButton(parent)
{
	control_ = 0;

	// Current settings.

	setControl(control);
}

AMControlToolButton::~AMControlToolButton()
{

}

void AMControlToolButton::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_)
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(updateColorState()) );

		emit controlChanged(control_);
	}

	updateColorState();
}

void AMControlToolButton::addColorState(AMToolButton::ColorState state, double minValue, double maxValue)
{
	colorStates_.append(state);
	colorStateMinValues_.append(minValue);
	colorStateMaxValues_.append(maxValue);
}

void AMControlToolButton::clearColorStates()
{
	colorStates_.clear();
	colorStateMinValues_.clear();
	colorStateMaxValues_.clear();
}

void AMControlToolButton::updateColorState()
{
	setColorState( getColorState() );
}

AMToolButton::ColorState AMControlToolButton::getColorState() const
{
	AMToolButton::ColorState result = AMToolButton::None;

	if (control_ && control_->canMeasure()) {
		double controlValue = control_->value();

		QList<AMToolButton::ColorState> colorStateMatches;

		// Iterate through the list of color states, identifying
		// states where the control value falls between the state min
		// and max values.

		for (int i = 0, count = colorStates_.count(); i < count; i++) {
			if (colorStateMinValues_.at(i) <= controlValue && controlValue <= colorStateMaxValues_.at(i))
				colorStateMatches << colorStates_.at(i);
		}

		// If there is more than one potential color state match,
		// take the result to be the first one.

		if (colorStateMatches.count() >= 1)
			result = colorStateMatches.first();
	}

	return result;
}
