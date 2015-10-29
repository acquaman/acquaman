#include "BioXASSidePOEBeamStatusControlLEDView.h"

BioXASSidePOEBeamStatusControlLEDView::BioXASSidePOEBeamStatusControlLEDView(BioXASSidePOEBeamStatusControl *control, QWidget *parent) :
	BioXASSidePOEBeamStatusControlView(parent)
{
	// Create UI elements.

	mirrorLED_ = new AMControlValueLEDView(0);
//	mirrorLED_->setRedValue(0);
//	mirrorLED_->setGreenWhenNotRed(true);
	mirrorLED_->setToolTip("M1 Mirror Mask");

	monoLED_ = new AMControlValueLEDView(0);
	monoLED_->setGreenValue(BioXASSSRLMonochromatorMaskControl::Open);
	monoLED_->setToolTip("Mono Mask");

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
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
	mirrorLED_->setControl(0);
	monoLED_->setControl(0);
}

void BioXASSidePOEBeamStatusControlLEDView::update()
{
	if (control_) {
		mirrorLED_->setControl(control_->mirrorMask());
		monoLED_->setControl(control_->monoMask());

	} else {
		mirrorLED_->setControl(0);
		monoLED_->setControl(0);
	}
}
