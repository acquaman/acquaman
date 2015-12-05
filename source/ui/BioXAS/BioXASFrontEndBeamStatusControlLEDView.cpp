#include "BioXASFrontEndBeamStatusControlLEDView.h"
#include "beamline/BioXAS/BioXASFrontEndValves.h"

BioXASFrontEndBeamStatusControlLEDView::BioXASFrontEndBeamStatusControlLEDView(BioXASFrontEndBeamStatusControl *control, QWidget *parent) :
	BioXASFrontEndBeamStatusControlView(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	shuttersLED_ = new AMControlValueLEDView(0);
	shuttersLED_->setGreenValue(AMNumber(BioXASFrontEndShuttersControl::Open));
	shuttersLED_->setToolTip("Shutters");

	valvesLED_ = new AMControlValueLEDView(0);
	valvesLED_->setGreenValue(AMNumber(BioXASValves::Open));
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
	if (control_) {
		shuttersLED_->setControl(control_->shutters());
		valvesLED_->setControl(control_->valves());

	} else {
		shuttersLED_->setControl(0);
		valvesLED_->setControl(0);
	}
}
