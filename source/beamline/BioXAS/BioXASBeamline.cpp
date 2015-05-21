#include "BioXASBeamline.h"
#include "beamline/BioXAS/BioXASMonochromator.h"
#include "beamline/BioXAS/BioXASM1Mirror.h"
#include "beamline/BioXAS/BioXASM2Mirror.h"
#include "actions3/AMActionSupport.h"

BioXASBeamline::~BioXASBeamline()
{

}

bool BioXASBeamline::isConnected() const
{
	bool shuttersOK = (
				photonShutter_ && photonShutter_->isConnected() &&
				safetyShutter_ && safetyShutter_->isConnected()
				);

	return shuttersOK;
}

bool BioXASBeamline::beamOff() const
{
	bool result = false;

	if (isConnected()) {
		bool photonShutterClosed = (photonShutter_->value() == Shutters::Closed);
		bool safetyShutterClosed = (safetyShutter_->value() == Shutters::Closed);

		result = (photonShutterClosed && safetyShutterClosed);
	}

	return result;
}

bool BioXASBeamline::beamOn() const
{
	bool result = false;

	if (isConnected()) {
		bool photonShutterOpen = (photonShutter_->value() == Shutters::Open);
		bool safetyShutterOpen = (safetyShutter_->value() == Shutters::Open);

		result = (photonShutterOpen && safetyShutterOpen);
	}

	return result;
}

AMAction3* BioXASBeamline::createTurnOffBeamActions()
{
	AMAction3 *action = 0;

	if (photonShutter() && photonShutter()->isConnected() && safetyShutter() && safetyShutter()->isConnected()) {

		// For bi-state controls, the move setpoint for 'close' is 0 and the desired state is 4.

		AMListAction3 *beamOff = new AMListAction3(new AMListActionInfo3("Turn off beam", "Turn off beam"), AMListAction3::Sequential);
		beamOff->addSubAction(AMActionSupport::buildControlMoveAction(photonShutter(), 0));
		beamOff->addSubAction(AMActionSupport::buildControlWaitAction(photonShutter(), 4, 5));
		beamOff->addSubAction(AMActionSupport::buildControlMoveAction(safetyShutter(), 0));
		beamOff->addSubAction(AMActionSupport::buildControlWaitAction(safetyShutter(), 4, 5));

		action = beamOff;
	}

	return action;
}

AMAction3* BioXASBeamline::createTurnOnBeamActions()
{
	AMAction3 *action = 0;

	if (photonShutter() && photonShutter()->isConnected() && safetyShutter() && safetyShutter()->isConnected()) {

		// For bi-state controls, the move setpoint for 'open' is 1 and the desired state is 1.

		AMListAction3 *beamOn = new AMListAction3(new AMListActionInfo3("Turn off beam", "Turn off beam"), AMListAction3::Sequential);
		beamOn->addSubAction(AMActionSupport::buildControlMoveAction(photonShutter(), 1));
		beamOn->addSubAction(AMActionSupport::buildControlWaitAction(photonShutter(), 1, 5));
		beamOn->addSubAction(AMActionSupport::buildControlMoveAction(safetyShutter(), 1));
		beamOn->addSubAction(AMActionSupport::buildControlWaitAction(safetyShutter(), 1, 5));

		action = beamOn;
	}

	return action;
}

void BioXASBeamline::setupComponents()
{
	// Shutters

	photonShutter_ = new CLSBiStateControl("PhotonShutter", "BioXAS photon shutter", "IPSH1407-I00-02:state", "IPSH1407-I00-02:opr:open", "IPSH1407-I00-02:opr:close", new AMControlStatusCheckerDefault(2), this);
	safetyShutter_ = new CLSBiStateControl("SafetyShutter", "BioXAS safety Shutter", "SSH1407-I00-01:state", "SSH1407-I00-01:opr:open", "SSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
}

BioXASBeamline::BioXASBeamline(const QString &controlName) :
	CLSBeamline(controlName)
{
	// Initialize member variables.

	photonShutter_ = 0;
	safetyShutter_ = 0;

	// Setup procedures.

	setupComponents();
}
