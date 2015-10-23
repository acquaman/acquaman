 #include "AMControlValueRedLEDView.h"

AMControlValueRedLEDView::AMControlValueRedLEDView(AMControl *control, QWidget *parent) :
	AMControlLEDView(control, parent)
{
	// Initialize class variables.

	redValue_ = AMNumber::InvalidError;

	// Current settings.

	refresh();
}

AMControlValueRedLEDView::~AMControlValueRedLEDView()
{

}

void AMControlValueRedLEDView::setRedValue(const AMNumber &newValue)
{
	if (double(redValue_) != double(newValue)) {
		redValue_ = newValue;
		refresh();

		emit redValueChanged(redValue_);
	}
}

bool AMControlValueRedLEDView::shouldBeRedOn() const
{
	return atRedValue();
}

bool AMControlValueRedLEDView::shouldBeRedOff() const
{
	return (!shouldBeRedOn());
}

bool AMControlValueRedLEDView::atRedValue() const
{
	bool result = false;

	if (control_ && control_->canMeasure() && redValue_.isValid())
		result = qFuzzyCompare(control_->value(), double(redValue_));

	return result;
}
