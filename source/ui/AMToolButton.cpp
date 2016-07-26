#include "AMToolButton.h"
#include <QStyle>

AMToolButton::AMToolButton(QWidget *parent) :
	QToolButton(parent)
{
	colorState_ = None;
}

AMToolButton::~AMToolButton()
{

}

QString AMToolButton::colorStateToString(AMToolButton::ColorState state) const
{
	QString result = "";

	if (state == AMToolButton::None)
		result = "None";
	else if (state == AMToolButton::Good)
		result = "Good";
	else if (state == AMToolButton::Bad)
		result = "Bad";
	else if (state == AMToolButton::Neutral)
		result = "Neutral";

	return result;
}

void AMToolButton::setColorState(AMToolButton::ColorState newState)
{
	if (colorState_ != newState) {
		colorState_ = newState;

		// Sometimes, it seems like the tool buttons don't update
		// their color properly in response to changes in the
		// colorState QProperty. We can force an update here.

		style()->unpolish(this);
		style()->polish(this);

		emit colorStateChanged(colorState_);
	}
}
