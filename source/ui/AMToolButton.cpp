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
