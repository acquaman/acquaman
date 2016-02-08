#include "AMControlConnectedLEDView.h"

AMControlConnectedLEDView::AMControlConnectedLEDView(AMControl *control, QWidget *parent) :
	AMControlLEDView(control, parent)
{

}

AMControlConnectedLEDView::~AMControlConnectedLEDView()
{

}

bool AMControlConnectedLEDView::shouldBeGreenOn() const
{
	bool result = false;

	if (controlConnected())
		result = true;

	return result;
}

bool AMControlConnectedLEDView::shouldBeGreenOff() const
{
	bool result = false;

	if (!controlConnected())
		result = true;

	return result;
}

bool AMControlConnectedLEDView::controlConnected() const
{
	bool result = false;

	if (control_ && control_->isConnected())
		result = true;

	return result;
}
