#include "BioXASSidePOEBeamStatusControlView.h"

BioXASSidePOEBeamStatusControlView::BioXASSidePOEBeamStatusControlView(QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;
}

BioXASSidePOEBeamStatusControlView::~BioXASSidePOEBeamStatusControlView()
{

}

void BioXASSidePOEBeamStatusControlView::refresh()
{
	// Clears the view.

	clear();

	// Updates the view.

	update();
}

void BioXASSidePOEBeamStatusControlView::setControl(BioXASSidePOEBeamStatusControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(update()) );
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(update()) );
			connect( control_, SIGNAL(frontEndBeamStatusChanged(BioXASFrontEndBeamStatusControl*)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(mirrorMaskChanged(AMControl*)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(monoMaskChanged(AMControl*)), this, SLOT(refresh()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}
