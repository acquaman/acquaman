#include "BioXASFrontEndBeamStatusView.h"

BioXASFrontEndBeamStatusView::BioXASFrontEndBeamStatusView(BioXASFrontEndBeamStatusControl *beamStatus, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	beamStatus_ = 0;

	// Create UI elements.

	beamStatusEditor_ = new AMExtendedControlEditor(0);
	photonShutterUpstreamLED_ = new QLabel();
	vacuumValveLED_ = new QLabel();
	fastValveLED_ = new QLabel();
	photonShutterDownstreamLED_ = new QLabel();
	safetyShutterLED_ = new QLabel();
	beamlineValvesLED_ = new QLabel();

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

	setBeamStatusControl(beamStatus);
	refresh();
}

BioXASFrontEndBeamStatusView::~BioXASFrontEndBeamStatusView()
{

}

void BioXASFrontEndBeamStatusView::clear()
{
	beamStatusEditor_->setControl(0);
}

void BioXASFrontEndBeamStatusView::update()
{
	updatePhotonShutterUpstreamLED();
	updateVacuumValveLED();
	updateFastValveLED();
	updatePhotonShutterDownstreamLED();
	updateSafetyShutterLED();
	updateBeamlineValvesLED();
}

void BioXASFrontEndBeamStatusView::refresh()
{
	// Clear the view.

	clear();

	// Setup.

	beamStatusEditor_->setControl(beamStatus_);

	photonShutterUpstreamLED_->setToolTip("PSH1");
	vacuumValveLED_->setToolTip("VVR");
	fastValveLED_->setToolTip("VVF");
	photonShutterDownstreamLED_->setToolTip("PSH2");
	safetyShutterLED_->setToolTip("SSH");
	beamlineValvesLED_->setToolTip("Beamline valves");

	// Update the view.

	update();
}

void BioXASFrontEndBeamStatusView::setBeamStatusControl(BioXASFrontEndBeamStatusControl *newControl)
{
	if (beamStatus_ != newControl) {

		beamStatus_ = newControl;

		refresh();

		emit beamStatusControlChanged(beamStatus_);
	}
}

void BioXASFrontEndBeamStatusView::setBeamStatusEditorMinimumWidth(int newWidth)
{
	beamStatusEditor_->setMinimumWidth(newWidth);
}

void BioXASFrontEndBeamStatusView::setBeamStatusEditorTitle(const QString &newTitle)
{
	beamStatusEditor_->setTitle(newTitle);
}

void BioXASFrontEndBeamStatusView::updatePhotonShutterUpstreamLED()
{
	if (beamStatus_)
		updateLED(photonShutterUpstreamLED_, beamStatus_->photonShutterUpstream());
	else
		updateLED(photonShutterUpstreamLED_, 0);
}

void BioXASFrontEndBeamStatusView::updateVacuumValveLED()
{
	if (beamStatus_)
		updateLED(vacuumValveLED_, beamStatus_->vacuumValve());
	else
		updateLED(vacuumValveLED_, 0);
}

void BioXASFrontEndBeamStatusView::updateFastValveLED()
{
	if (beamStatus_)
		updateLED(fastValveLED_, beamStatus_->fastValve());
	else
		updateLED(fastValveLED_, 0);
}

void BioXASFrontEndBeamStatusView::updatePhotonShutterDownstreamLED()
{
	if (beamStatus_)
		updateLED(photonShutterDownstreamLED_, beamStatus_->photonShutterDownstream());
	else
		updateLED(photonShutterDownstreamLED_, 0);
}

void BioXASFrontEndBeamStatusView::updateSafetyShutterLED()
{
	if (beamStatus_)
		updateLED(safetyShutterLED_, beamStatus_->safetyShutter());
	else
		updateLED(safetyShutterLED_, 0);
}

void BioXASFrontEndBeamStatusView::updateBeamlineValvesLED()
{
	if (beamStatus_)
		updateLED(beamlineValvesLED_, beamStatus_->valves());
	else
		updateLED(beamlineValvesLED_, 0);
}

void BioXASFrontEndBeamStatusView::updateLED(QLabel *led, AMControl *control)
{
	if (led) {
		bool controlFound = false;

		if (!controlFound) {
			CLSBiStateControl *biStateControl = qobject_cast<CLSBiStateControl*>(control);
			if (biStateControl) {
				controlFound = true;
				updateBiStateControlLED(led, biStateControl);
			}
		}

		if (!controlFound) {
			BioXASValvesControl *valvesControl = qobject_cast<BioXASValvesControl*>(control);
			if (valvesControl) {
				controlFound = true;
				updateBioXASBeamlineValvesLED(led, valvesControl);
			}
		}
	}
}

void BioXASFrontEndBeamStatusView::updateBiStateControlLED(QLabel *led, CLSBiStateControl *control)
{
	if (led) {
		if (control && control->isOpen())
			led->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
		else if (control && !control->isOpen())
			led->setPixmap(QPixmap(":/22x22/redLEDOn.png"));
		else
			led->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
	}
}

void BioXASFrontEndBeamStatusView::updateBioXASBeamlineValvesLED(QLabel *led, BioXASValvesControl *control)
{
	if (led) {
		if (control && control->isOpen())
			led->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
		else if (control && !control->isOpen())
			led->setPixmap(QPixmap(":/22x22/redLEDOn.png"));
		else
			led->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
	}
}


