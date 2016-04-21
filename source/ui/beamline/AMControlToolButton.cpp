#include "AMControlToolButton.h"
#include "beamline/AMControl.h"

AMControlToolButton::AMControlToolButton(AMControl *control, QWidget *parent) :
	AMToolButton(parent)
{
	// Initialize inherited variables.

	setCheckable(true);
	setAutoRaise(false);

	// Initialize class variables.

	control_ = 0;

	// Current settings.

	setControl(control);
	refresh();
}

AMControlToolButton::~AMControlToolButton()
{

}

void AMControlToolButton::refresh()
{
	updateColor();
}

void AMControlToolButton::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(refresh()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void AMControlToolButton::updateColor()
{
	//setColor( currentColor() );
}
