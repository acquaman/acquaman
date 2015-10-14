#include "AMControlValueLEDView.h"

AMControlValueLEDView::AMControlValueLEDView(AMControl *control, const AMNumber &greenValue, const AMNumber &redValue, QWidget *parent) :
	AMControlLEDView(control, parent)
{
	// Initialize class variables.

	greenValue_ = AMNumber::InvalidError;
	redValue_ = AMNumber::InvalidError;

	// Current settings.

	setGreenValue(greenValue);
	setRedValue(redValue);

	refresh();
}

AMControlValueLEDView::~AMControlValueLEDView()
{

}

void AMControlValueLEDView::update()
{
	if (control_) {
		if (control_->isConnected() && greenValue_.isValid() && qFuzzyCompare(control_->value(), double(greenValue_)))
			setAsGreenOn();
		else if (control_->isConnected() && redValue_.isValid() && qFuzzyCompare(control_->value(), double(redValue_)))
			setAsRedOn();
		else if (control_->isConnected())
			setAsYellowOn();
		else
			setAsGreenOff();

	} else {
		setAsGreenOff();
	}
}

void AMControlValueLEDView::setGreenValue(const AMNumber &newValue)
{
	if (double(greenValue_) != double(newValue)) {
		greenValue_ = newValue;
		emit greenValueChanged(greenValue_);
	}
}

void AMControlValueLEDView::setRedValue(const AMNumber &newValue)
{
	if (double(redValue_) != double(newValue)) {
		redValue_ = newValue;
		emit redValueChanged(redValue_);
	}
}
