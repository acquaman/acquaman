#include "AMControlConnectedLEDView.h"

AMControlConnectedLEDView::AMControlConnectedLEDView(AMControl *control, QWidget *parent) :
	AMControlLEDView(control, parent)
{

}

AMControlConnectedLEDView::~AMControlConnectedLEDView()
{

}

void AMControlConnectedLEDView::update()
{
	if (control_) {
		if (control_->isConnected())
			setAsGreenOn();
		else
			setAsRedOn();

	} else {
		setAsGreenOff();
	}
}
