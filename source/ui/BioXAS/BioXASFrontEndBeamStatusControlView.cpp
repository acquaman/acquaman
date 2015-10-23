#include "BioXASFrontEndBeamStatusControlView.h"

BioXASFrontEndBeamStatusControlView::BioXASFrontEndBeamStatusControlView(QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;
}

BioXASFrontEndBeamStatusControlView::~BioXASFrontEndBeamStatusControlView()
{

}

void BioXASFrontEndBeamStatusControlView::setControl(BioXASFrontEndBeamStatusControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(update()) );
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(update()) );
			connect( control_, SIGNAL(shuttersChanged(AMControl*)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(valvesChanged(AMControl*)), this, SLOT(refresh()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}
