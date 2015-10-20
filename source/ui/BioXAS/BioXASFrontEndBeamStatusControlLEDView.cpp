#include "BioXASFrontEndBeamStatusControlLEDView.h"

BioXASFrontEndBeamStatusControlLEDView::BioXASFrontEndBeamStatusControlLEDView(BioXASFrontEndBeamStatusControl *control, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	photonShutterUpstreamLED_ = new AMControlValueLEDView(0);
	photonShutterUpstreamLED_->setGreenValue(AMNumber(1));
	photonShutterUpstreamLED_->setRedWhenNotGreen(true);
	photonShutterUpstreamLED_->setToolTip("PSH1");

	photonShutterDownstreamLED_ = new AMControlValueLEDView(0);
	photonShutterDownstreamLED_->setGreenValue(AMNumber(CLSBiStateControl::Open));
	photonShutterDownstreamLED_->setRedWhenNotGreen(true);
	photonShutterDownstreamLED_->setToolTip("PSH2");

	safetyShutterLED_ = new AMControlValueLEDView(0);
	safetyShutterLED_->setGreenValue(AMNumber(CLSBiStateControl::Open));
	safetyShutterLED_->setRedWhenNotGreen(true);
	safetyShutterLED_->setToolTip("SSH");

	valvesLED_ = new AMControlValueLEDView(0);
	valvesLED_->setGreenValue(AMNumber(BioXASValvesControl::Open));
	valvesLED_->setRedWhenNotGreen(true);
	valvesLED_->setToolTip("Valves");

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(photonShutterUpstreamLED_);
	layout->addWidget(photonShutterDownstreamLED_);
	layout->addWidget(safetyShutterLED_);
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
	photonShutterUpstreamLED_->setControl(0);
	photonShutterDownstreamLED_->setControl(0);
	safetyShutterLED_->setControl(0);
	valvesLED_->setControl(0);
}

void BioXASFrontEndBeamStatusControlLEDView::update()
{
	photonShutterUpstreamLED_->update();
	photonShutterDownstreamLED_->update();
	safetyShutterLED_->update();
	valvesLED_->update();
}

void BioXASFrontEndBeamStatusControlLEDView::refresh()
{
	// Clear the view.

	clear();

	// Setup.

	if (control_) {

		if (control_->shutters()) {
			photonShutterUpstreamLED_->setControl(control_->shutters()->photonShutterUpstream());
			photonShutterDownstreamLED_->setControl(control_->shutters()->photonShutterDownstream());
			safetyShutterLED_->setControl(control_->shutters()->safetyShutter());
		}

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
			connect( control_, SIGNAL(shuttersUpstreamPhotonShutterChanged(AMControl*)), photonShutterUpstreamLED_, SLOT(setControl(AMControl*)) );
			connect( control_, SIGNAL(shuttersDownstreamPhotonShutterChanged(AMControl*)), photonShutterDownstreamLED_, SLOT(setControl(AMControl*)) );
			connect( control_, SIGNAL(shuttersSafetyShutterChanged(AMControl*)), safetyShutterLED_, SLOT(setControl(AMControl*)) );
			connect( control_, SIGNAL(valvesChanged(AMControl*)), valvesLED_, SLOT(setControl(AMControl*)) );
		}

		refresh();

		emit controlChanged(control_);
	}
}
