#include "AMToolButton.h"

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
		emit colorStateChanged(colorState_);
	}
}
