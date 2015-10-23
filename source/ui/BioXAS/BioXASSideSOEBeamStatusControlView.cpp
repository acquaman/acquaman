#include "BioXASSideSOEBeamStatusControlView.h"

BioXASSideSOEBeamStatusControlView::BioXASSideSOEBeamStatusControlView(QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;
}

BioXASSideSOEBeamStatusControlView::~BioXASSideSOEBeamStatusControlView()
{

}

void BioXASSideSOEBeamStatusControlView::setControl(BioXASSideSOEBeamStatusControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(update()) );
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(update()) );
			connect( control_, SIGNAL(poeBeamStatusChanged(BioXASSidePOEBeamStatusControl*)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(endstationShutterChanged(AMControl*)), this, SLOT(refresh()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

