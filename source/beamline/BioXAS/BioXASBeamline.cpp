#include "BioXASBeamline.h"

#include "actions3/AMActionSupport.h"
#include "beamline/CLS/CLSStorageRing.h"
#include "util/AMErrorMonitor.h"

BioXASBeamline::~BioXASBeamline()
{

}

bool BioXASBeamline::isConnected() const
{
	bool connected = (
				beamStatus_ && beamStatus_->isConnected() &&
				utilities_ && utilities_->isConnected()
				);

	return connected;
}

BioXASShutters* BioXASBeamline::shutters() const
{
	BioXASShutters *result = 0;

	if (utilities_)
		result = utilities_->shutters();

	return result;
}

BioXASValves* BioXASBeamline::beampathValves() const
{
	BioXASValves *result = 0;

	if (utilities_)
		result = utilities_->beampathValves();

	return result;
}

BioXASValves* BioXASBeamline::valves() const
{
	BioXASValves *result = 0;

	if (utilities_)
		result = utilities_->valves();

	return result;
}

BioXASIonPumps* BioXASBeamline::ionPumps() const
{
	BioXASIonPumps *result = 0;

	if (utilities_)
		result = utilities_->ionPumps();

	return result;
}

AMBasicControlDetectorEmulator* BioXASBeamline::detectorForControl(AMControl *control) const
{
	return controlDetectorMap_.value(control, 0);
}

void BioXASBeamline::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connectedChanged(connected_);
	}
}

void BioXASBeamline::updateConnected()
{
	setConnected( isConnected() );
}

void BioXASBeamline::addShutter(AMControl *newControl, double openValue, double closedValue)
{
	if (utilities_)
		utilities_->addShutter(newControl, openValue, closedValue);

	if (beamStatus_)
		beamStatus_->addShutter(newControl, openValue, closedValue);
}

void BioXASBeamline::removeShutter(AMControl *control)
{
	if (utilities_)
		utilities_->removeShutter(control);

	if (beamStatus_)
		beamStatus_->removeShutter(control);
}

void BioXASBeamline::clearShutters()
{
	if (utilities_)
		utilities_->clearShutters();

	if (beamStatus_)
		beamStatus_->clearShutters();
}

void BioXASBeamline::addBeampathValve(AMControl *newControl, double openValue, double closedValue)
{
	if (utilities_)
		utilities_->addBeampathValve(newControl, openValue, closedValue);

	if (beamStatus_)
		beamStatus_->addValve(newControl, openValue, closedValue);
}

void BioXASBeamline::removeBeampathValve(AMControl *control)
{
	if (utilities_)
		utilities_->removeBeampathValve(control);

	if (beamStatus_)
		beamStatus_->removeValve(control);
}

void BioXASBeamline::clearBeampathValves()
{
	if (utilities_)
		utilities_->clearBeampathValves();

	if (beamStatus_)
		beamStatus_->clearValves();
}

void BioXASBeamline::addValve(AMControl *newControl, double openValue, double closedValue)
{
	if (utilities_)
		utilities_->addValve(newControl, openValue, closedValue);
}

void BioXASBeamline::removeValve(AMControl *control)
{
	if (utilities_)
		utilities_->removeValve(control);
}

void BioXASBeamline::clearValves()
{
	if (utilities_)
		utilities_->clearValves();
}

void BioXASBeamline::addIonPump(AMControl *newControl)
{
	if (utilities_)
		utilities_->addIonPump(newControl);
}

void BioXASBeamline::removeIonPump(AMControl *control)
{
	if (utilities_)
		utilities_->removeIonPump(control);
}

void BioXASBeamline::clearIonPumps()
{
	if (utilities_)
		utilities_->clearIonPumps();
}

void BioXASBeamline::setupComponents()
{
	// Beam status.

	beamStatus_ = new BioXASBeamStatus("BioXASBeamStatus", this);
	connect( beamStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Utilities.

	utilities_ = new BioXASUtilities("BioXASUtilities", this);
	connect( utilities_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Utilities - front-end shutters.

	addShutter(new BioXASFrontEndShutters("BioXASFrontEndShutters", this), BioXASFrontEndShutters::Open, BioXASFrontEndShutters::Closed);

	// Utilities - front-end beampath valves.

	addBeampathValve(new AMReadOnlyPVControl("VVR1407-I00-01", "VVR1407-I00-01:state", this), BIOXASBEAMLINE_VALVE_OPEN, BIOXASBEAMLINE_VALVE_CLOSED);
	addBeampathValve(new AMReadOnlyPVControl("VVF1407-I00-01", "VVF1407-I00-01:state", this), BIOXASBEAMLINE_FASTVALVE_OPEN, BIOXASBEAMLINE_FASTVALVE_CLOSED);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I00-01", "VVR1607-5-I00-01:state", "VVR1607-5-I00-01:opr:open", "VVR1607-5-I00-01:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-01", "VVR1607-5-I22-01:state", "VVR1607-5-I22-01:opr:open", "VVR1607-5-I22-01:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-01", "VVR1607-5-I21-01:state", "VVR1607-5-I21-01:opr:open", "VVR1607-5-I21-01:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-01", "VVR1607-5-I10-01:state", "VVR1607-5-I10-01:opr:open", "VVR1607-5-I10-01:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);

	// Utilities - Side beampath valves.

	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-02", "VVR1607-5-I22-02:state", "VVR1607-5-I22-02:opr:open", "VVR1607-5-I22-02:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-03", "VVR1607-5-I22-03:state", "VVR1607-5-I22-03:opr:open", "VVR1607-5-I22-03:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-04", "VVR1607-5-I22-04:state", "VVR1607-5-I22-04:opr:open", "VVR1607-5-I22-04:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-06", "VVR1607-5-I22-06:state", "VVR1607-5-I22-06:opr:open", "VVR1607-5-I22-06:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-07", "VVR1607-5-I22-07:state", "VVR1607-5-I22-07:opr:open", "VVR1607-5-I22-07:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);

	// Utilities - Main beampath valves.

	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-02", "VVR1607-5-I21-02:state", "VVR1607-5-I21-02:opr:open", "VVR1607-5-I21-02:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-03", "VVR1607-5-I21-03:state", "VVR1607-5-I21-03:opr:open", "VVR1607-5-I21-03:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-04", "VVR1607-5-I21-04:state", "VVR1607-5-I21-04:opr:open", "VVR1607-5-I21-04:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-06", "VVR1607-5-I21-06:state", "VVR1607-5-I21-06:opr:open", "VVR1607-5-I21-06:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-07", "VVR1607-5-I21-07:state", "VVR1607-5-I21-07:opr:open", "VVR1607-5-I21-07:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);

	// Utilities - Imaging beampath valves.

	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-02", "VVR1607-5-I10-02:state", "VVR1607-5-I10-02:opr:open", "VVR1607-5-I10-02:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-03", "VVR1607-5-I10-03:state", "VVR1607-5-I10-03:opr:open", "VVR1607-5-I10-03:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-04", "VVR1607-5-I10-04:state", "VVR1607-5-I10-04:opr:open", "VVR1607-5-I10-04:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-05", "VVR1607-5-I10-05:state", "VVR1607-5-I10-05:opr:open", "VVR1607-5-I10-05:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-07", "VVR1607-5-I10-07:state", "VVR1607-5-I10-07:opr:open", "VVR1607-5-I10-07:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-08", "VVR1607-5-I10-08:state", "VVR1607-5-I10-08:opr:open", "VVR1607-5-I10-08:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-10", "VVR1607-5-I10-10:state", "VVR1607-5-I10-10:opr:open", "VVR1607-5-I10-10:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);

	// Utilities - front-end ion pumps.

	addIonPump(new AMReadOnlyPVControl("IOP1407-I00-01", "IOP1407-I00-01", this));
	addIonPump(new AMReadOnlyPVControl("IOP1407-I00-02", "IOP1407-I00-02", this));
	addIonPump(new AMReadOnlyPVControl("IOP1407-I00-03", "IOP1407-I00-03", this));
	addIonPump(new AMReadOnlyPVControl("IOP1607-5-I00-01", "IOP1607-5-I00-01", this));
	addIonPump(new AMReadOnlyPVControl("IOP1607-5-I00-02", "IOP1607-5-I00-02", this));
}

AMBasicControlDetectorEmulator* BioXASBeamline::createDetectorEmulator(const QString &name, const QString &description, AMControl *control, bool hiddenFromUsers, bool isVisible)
{
	AMBasicControlDetectorEmulator *detector = 0;

	if (control) {
		detector = new AMBasicControlDetectorEmulator(name, description, control, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
		detector->setHiddenFromUsers(hiddenFromUsers);
		detector->setIsVisible(isVisible);
	}

	return detector;
}

void BioXASBeamline::addControlAsDetector(const QString &name, const QString &description, AMControl *control, bool hiddenFromUsers, bool isVisible)
{
	if (control && !controlDetectorMap_.contains(control)) {
		AMBasicControlDetectorEmulator *detector = createDetectorEmulator(name, description, control, hiddenFromUsers, isVisible);
		controlDetectorMap_.insert(control, detector);
	}
}

BioXASBeamline::BioXASBeamline(const QString &controlName) :
	CLSBeamline(controlName)
{
	// Initialize member variables.

	connected_ = false;

	beamStatus_ = 0;
	utilities_ = 0;

	// Setup procedures.

	setupComponents();
}
