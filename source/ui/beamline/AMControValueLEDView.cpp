#include "AMControlValueLEDView.h"
#include <QDebug>

AMControlValueLEDView::AMControlValueLEDView(AMControl *control, QWidget *parent) :
	AMControlLEDView(control, parent)
{
	// Initialize class variables.

	greenValue_ = AMNumber::InvalidError;

	// Current settings.

	refresh();
}

AMControlValueLEDView::~AMControlValueLEDView()
{

}

void AMControlValueLEDView::setGreenValue(const AMNumber &newValue)
{
	if (double(greenValue_) != double(newValue)) {
		greenValue_ = newValue;
		refresh();

		emit greenValueChanged(greenValue_);
	}
}

bool AMControlValueLEDView::shouldBeGreenOn() const
{
	bool result = false;

	if (control_ && control_->canMeasure())
		result = atGreenValue();

	return result;
}

bool AMControlValueLEDView::shouldBeGreenOff() const
{
	bool result = (!control_ || !control_->canMeasure());

	return result;
}

bool AMControlValueLEDView::shouldBeRedOn() const
{
	bool result = false;

	if (control_ && control_->canMeasure())
		result = (!atGreenValue());

	return result;
}

bool AMControlValueLEDView::atGreenValue() const
{
	bool result = false;

	if (control_ && control_->canMeasure() && greenValue_.isValid())
		result = qFuzzyCompare(control_->value(), double(greenValue_));

	return result;
}
