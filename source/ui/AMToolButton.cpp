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

		style()->unpolish(this);
		style()->polish(this);

		emit colorStateChanged(colorState_);
	}
}
