#include "BioXASFrontEndBeamStatusView.h"

BioXASFrontEndBeamStatusView::BioXASFrontEndBeamStatusView(BioXASFrontEndBeamStatus *beamStatus, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	beamStatus_ = 0;

	// Create UI elements.

	beamStatusEditor_ = new AMExtendedControlEditor(0);

	photonShutterUpstreamLED_ = new AMControlValueLEDView(0, AMNumber(1), AMNumber(0));
	photonShutterUpstreamLED_->setToolTip("PSH1");

	vacuumValveLED_ = new AMControlValueLEDView(0, AMNumber(1), AMNumber(0));
	vacuumValveLED_->setToolTip("VVR");

	fastValveLED_ = new AMControlValueLEDView(0, AMNumber(1), AMNumber(0));
	fastValveLED_->setToolTip("VVF");

	photonShutterDownstreamLED_ = new AMControlValueLEDView(0, AMNumber(CLSBiStateControl::Open), AMNumber(CLSBiStateControl::Closed));
	photonShutterDownstreamLED_->setToolTip("PSH2");

	safetyShutterLED_ = new AMControlValueLEDView(0, AMNumber(CLSBiStateControl::Open), AMNumber(CLSBiStateControl::Closed));
	safetyShutterLED_->setToolTip("SSH");

	beamlineValvesLED_ = new AMControlValueLEDView(0, AMNumber(BioXASValvesControl::Open), AMNumber(BioXASValvesControl::Closed));
	beamlineValvesLED_->setToolTip("Beamline valves");

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(beamStatusEditor_);
	layout->addWidget(photonShutterUpstreamLED_);
	layout->addWidget(vacuumValveLED_);
	layout->addWidget(fastValveLED_);
	layout->addWidget(photonShutterDownstreamLED_);
	layout->addWidget(safetyShutterLED_);
	layout->addWidget(beamlineValvesLED_);

	setLayout(layout);

	// Current settings.

	setBeamStatus(beamStatus);
	refresh();
}

BioXASFrontEndBeamStatusView::~BioXASFrontEndBeamStatusView()
{

}

void BioXASFrontEndBeamStatusView::clear()
{
	beamStatusEditor_->setControl(0);

	photonShutterUpstreamLED_->setControl(0);
	vacuumValveLED_->setControl(0);
	fastValveLED_->setControl(0);
	photonShutterDownstreamLED_->setControl(0);
	safetyShutterLED_->setControl(0);
	beamlineValvesLED_->setControl(0);
}

void BioXASFrontEndBeamStatusView::update()
{
	photonShutterUpstreamLED_->update();
	vacuumValveLED_->update();
	fastValveLED_->update();
	photonShutterDownstreamLED_->update();
	safetyShutterLED_->update();
	beamlineValvesLED_->update();
}

void BioXASFrontEndBeamStatusView::refresh()
{
	// Clear the view.

	clear();

	// Setup.

	if (beamStatus_) {
		beamStatusEditor_->setControl(beamStatus_->beamStatusControl());

		photonShutterUpstreamLED_->setControl(beamStatus_->beamStatusControl()->photonShutterUpstream());
		vacuumValveLED_->setControl(beamStatus_->beamStatusControl()->vacuumValve());
		fastValveLED_->setControl(beamStatus_->beamStatusControl()->fastValve());
		photonShutterDownstreamLED_->setControl(beamStatus_->beamStatusControl()->photonShutterDownstream());
		safetyShutterLED_->setControl(beamStatus_->beamStatusControl()->safetyShutter());
		beamlineValvesLED_->setControl(beamStatus_->beamStatusControl()->valves());
	}

	// Update the view.

	update();
}

void BioXASFrontEndBeamStatusView::setBeamStatus(BioXASFrontEndBeamStatus *newControl)
{
	if (beamStatus_ != newControl) {

		if (beamStatus_)
			disconnect( beamStatus_, 0, this, 0 );

		beamStatus_ = newControl;

		if (beamStatus_) {
			connect( beamStatus_->beamStatusControl(), SIGNAL(photonShutterUpstreamChanged(AMReadOnlyPVControl*)), photonShutterUpstreamLED_, SLOT(setControl(AMControl*)) );
			connect( beamStatus_->beamStatusControl(), SIGNAL(vacuumValveChanged(AMReadOnlyPVControl*)), vacuumValveLED_, SLOT(setControl(AMControl*)) );
			connect( beamStatus_->beamStatusControl(), SIGNAL(fastValveChanged(AMReadOnlyPVControl*)), fastValveLED_, SLOT(setControl(AMControl*)) );
			connect( beamStatus_->beamStatusControl(), SIGNAL(photonShutterDownstreamChanged(CLSBiStateControl*)), photonShutterDownstreamLED_, SLOT(setControl(AMControl*)) );
			connect( beamStatus_->beamStatusControl(), SIGNAL(safetyShutterChanged(CLSBiStateControl*)), safetyShutterLED_, SLOT(setControl(AMControl*)) );
			connect( beamStatus_->beamStatusControl(), SIGNAL(valvesChanged(BioXASValvesControl*)), beamlineValvesLED_, SLOT(setControl(AMControl*)) );
		}

		refresh();

		emit beamStatusControlChanged(beamStatus_);
	}
}
