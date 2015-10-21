#include "BioXASFrontEndBeamStatusControlLEDView.h"

BioXASFrontEndBeamStatusControlLEDView::BioXASFrontEndBeamStatusControlLEDView(BioXASFrontEndBeamStatusControl *control, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	shuttersLED_ = new AMControlValueLEDView(0);
	shuttersLED_->setGreenValue(AMNumber(BioXASFrontEndShuttersControl::Open));
	shuttersLED_->setRedWhenNotGreen(true);
	shuttersLED_->setToolTip("Shutters");

	valvesLED_ = new AMControlValueLEDView(0);
	valvesLED_->setGreenValue(AMNumber(BioXASValvesControl::Open));
	valvesLED_->setRedWhenNotGreen(true);
	valvesLED_->setToolTip("Valves");

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(shuttersLED_);
	layout->addWidget(valvesLED_);

	setLayout(layout);

	// Current settings.

	setControl(control);
	refresh();
}

BioXASFrontEndBeamStatusControlLEDView::~BioXASFrontEndBeamStatusControlLEDView()
{

}

void BioXASFrontEndBeamStatusControlLEDView::clear()
{
	shuttersLED_->setControl(0);
	valvesLED_->setControl(0);
}

void BioXASFrontEndBeamStatusControlLEDView::update()
{
	shuttersLED_->update();
	valvesLED_->update();
}

void BioXASFrontEndBeamStatusControlLEDView::refresh()
{
	// Clear the view.

	clear();

	// Setup.

	if (control_) {
		shuttersLED_->setControl(control_->shutters());
		valvesLED_->setControl(control_->valves());
	}

	// Update the view.

	update();
}

void BioXASFrontEndBeamStatusControlLEDView::setControl(BioXASFrontEndBeamStatusControl *newControl)
{
	if (control_ != newControl) {

		if (control_) {
			disconnect( control_, 0, this, 0 );
		}

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(update()) );
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(update()) );
			connect( control_, SIGNAL(shuttersChanged(AMControl*)), shuttersLED_, SLOT(setControl(AMControl*)) );
			connect( control_, SIGNAL(valvesChanged(AMControl*)), valvesLED_, SLOT(setControl(AMControl*)) );
		}

		refresh();

		emit controlChanged(control_);
	}
}
