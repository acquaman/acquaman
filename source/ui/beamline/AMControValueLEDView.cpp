#include "AMControlValueLEDView.h"
#include <QDebug>

AMControlValueLEDView::AMControlValueLEDView(AMControl *control, QWidget *parent) :
	AMControlLEDView(control, parent)
{
	// Initialize class variables.

	greenValue_ = AMNumber::InvalidError;
	redValue_ = AMNumber::InvalidError;

	redWhenNotGreen_ = false;

	// Current settings.

	refresh();
}

AMControlValueLEDView::~AMControlValueLEDView()
{

}

void AMControlValueLEDView::update()
{
	if (control_ && control_->isConnected()) {
		if (atGreenValue())
			setAsGreenOn();
		else if (atRedValue())
			setAsRedOn();
		else if (redWhenNotGreen_)
			setAsRedOn();
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
		refresh();

		emit greenValueChanged(greenValue_);
	}
}

void AMControlValueLEDView::setRedValue(const AMNumber &newValue)
{
	if (double(redValue_) != double(newValue)) {
		redValue_ = newValue;
		refresh();

		emit redValueChanged(redValue_);
	}
}

void AMControlValueLEDView::setRedWhenNotGreen(bool setRed)
{
	if (redWhenNotGreen_ != setRed) {
		redWhenNotGreen_ = setRed;

		update();
	}
}

bool AMControlValueLEDView::atGreenValue() const
{
	bool result = false;

	if (control_ && greenValue_.isValid())
		result = qFuzzyCompare(control_->value(), double(greenValue_));

	return result;
}

bool AMControlValueLEDView::atRedValue() const
{
	bool result = false;

	if (control_ && redValue_.isValid())
		result = qFuzzyCompare(control_->value(), double(redValue_));

	return result;
}
