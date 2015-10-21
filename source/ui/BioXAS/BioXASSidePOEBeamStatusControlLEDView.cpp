#include "BioXASSidePOEBeamStatusControlLEDView.h"

BioXASSidePOEBeamStatusControlLEDView::BioXASSidePOEBeamStatusControlLEDView(BioXASSidePOEBeamStatusControl *control, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	frontEndLED_ = new AMControlValueLEDView(0);

	mirrorLED_ = new AMControlValueLEDView(0);

	monoLED_ = new AMControlValueLEDView(0);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(frontEndLED_);
	layout->addWidget(mirrorLED_);
	layout->addWidget(monoLED_);

	setLayout(layout);

	// Current settings.

	setControl(control);
	refresh();
}

BioXASSidePOEBeamStatusControlLEDView::~BioXASSidePOEBeamStatusControlLEDView()
{

}

void BioXASSidePOEBeamStatusControlLEDView::clear()
{
	frontEndLED_->setControl(0);
	mirrorLED_->setControl(0);
	monoLED_->setControl(0);
}

void BioXASSidePOEBeamStatusControlLEDView::update()
{
	frontEndLED_->update();
	mirrorLED_->update();
	monoLED_->update();
}

void BioXASSidePOEBeamStatusControlLEDView::refresh()
{
	// Clear the view.

	clear();

	// Apply current settings.

	if (control_) {
		frontEndLED_->setControl(control_->frontEndBeamStatus());
		mirrorLED_->setControl(control_->mirrorMask());
		monoLED_->setControl(control_->monoMask());
	}

	// Update the view.

	update();
}

void BioXASSidePOEBeamStatusControlLEDView::setControl(BioXASSidePOEBeamStatusControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(update()) );
			connect( control_, SIGNAL(frontEndBeamStatusChanged(AMControl*)), frontEndLED_, SLOT(setControl(AMControl*)) );
			connect( control_, SIGNAL(mirrorMaskChanged(AMControl*)), mirrorLED_, SLOT(setControl(AMControl*)) );
			connect( control_, SIGNAL(monoMaskChanged(AMControl*)), monoLED_, SLOT(setControl(AMControl*)) );
		}
	}
}
