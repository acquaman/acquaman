#include "AMControlValueGreenLEDView.h"

AMControlValueGreenLEDView::AMControlValueGreenLEDView(AMControl *control, QWidget *parent) :
	AMControlLEDView(control, parent)
{
	// Initialize class variables.

	greenValue_ = AMNumber::InvalidError;

	// Current settings.

	refresh();
}

AMControlValueGreenLEDView::~AMControlValueGreenLEDView()
{

}

void AMControlValueGreenLEDView::setGreenValue(const AMNumber &newValue)
{
	if (double(greenValue_) != double(newValue)) {
		greenValue_ = newValue;
		refresh();

		emit greenValueChanged(greenValue_);
	}
}

bool AMControlValueGreenLEDView::shouldBeGreenOff() const
{
	return !atGreenValue();
}

bool AMControlValueGreenLEDView::shouldBeGreenOn() const
{
	return atGreenValue();
}

bool AMControlValueGreenLEDView::atGreenValue() const
{
	bool result = false;

	if (control_ && control_->canMeasure() && greenValue_.isValid() && control_->value() == double(greenValue_))
		result = true;

	return result;
}
