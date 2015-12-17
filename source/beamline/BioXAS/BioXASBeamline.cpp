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
				frontEndShutters_ && frontEndShutters_->isConnected() &&
				valves_ && valves_->isConnected() &&
				frontEndBeamStatus_ && frontEndBeamStatus_->isConnected()
				);

	return connected;
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

void BioXASBeamline::setupComponents()
{
	// Front end shutters.

	frontEndShutters_ = new BioXASFrontEndShutters(this);
	connect( frontEndShutters_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	frontEndShutters_->setUpstreamPhotonShutter(new AMReadOnlyPVControl("IPSH1407-I00-01", "IPSH1407-I00-01:state", this));
	frontEndShutters_->setDownstreamPhotonShutter(new CLSBiStateControl("IPSH1407-I00-02", "IPSH1407-I00-02", "IPSH1407-I00-02:state", "IPSH1407-I00-02:opr:open", "IPSH1407-I00-02:opr:close", new AMControlStatusCheckerDefault(2), this));
	frontEndShutters_->setSafetyShutter(new CLSBiStateControl("SSH1407-I00-01", "SSH1407-I00-01", "SSH1407-I00-01:state", "SSH1407-I00-01:opr:open", "SSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this));

	// Valves.

	valves_ = new BioXASMasterValves(this);
	connect( valves_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	valves_->setFrontEndValves(new BioXASFrontEndValves(this));
	valves_->setSideValves(new BioXASSideValves(this));
	valves_->setMainValves(new BioXASMainValves(this));
	valves_->setImagingValves(new BioXASImagingValves(this));

	// Beam status.

	frontEndBeamStatus_ = new BioXASFrontEndBeamStatus("BioXASBeamStatusFrontEnd", this);
	connect( frontEndBeamStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	frontEndBeamStatus_->setShutters(frontEndShutters_);
	frontEndBeamStatus_->setValves(valves_);
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

	frontEndShutters_ = 0;

	valves_ = 0;

	frontEndBeamStatus_ = 0;

	// Setup procedures.

	setupComponents();
}
