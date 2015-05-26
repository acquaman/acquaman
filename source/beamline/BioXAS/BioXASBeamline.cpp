#include "BioXASBeamline.h"
#include "beamline/BioXAS/BioXASMonochromator.h"
#include "beamline/BioXAS/BioXASM2Mirror.h"
#include "actions3/AMActionSupport.h"
#include "beamline/CLS/CLSStorageRing.h"

BioXASBeamline::~BioXASBeamline()
{

}

bool BioXASBeamline::isConnected() const
{
	bool shuttersOK = (
				photonShutterUpstream_ && photonShutterUpstream_->isConnected() &&
				photonShutterDownstream_ && photonShutterDownstream_->isConnected() &&
				safetyShutterUpstream_ && safetyShutterUpstream_->isConnected() &&
				safetyShutterDownstream_ && safetyShutterDownstream_->isConnected()
				);

	return shuttersOK;
}

bool BioXASBeamline::beamOff() const
{
	bool result = false;

	if (safetyShutterDownstream_ && safetyShutterDownstream_->isConnected()) {
		// The beam is considered off if the endstation safety shutter is closed (ignore other photon and safety shutters).
		result = (safetyShutterDownstream_->value() == Shutters::Closed);
	}

	return result;
}

bool BioXASBeamline::beamOn() const
{
	bool result = false;

	if (safetyShutterDownstream_ && safetyShutterDownstream_->isConnected()) {
		// The beam is considered on if the endstation safety shutter is open (ignore other photon and safety shutters).
		result = (safetyShutterDownstream_->value() == Shutters::Open);
	}

	return result;
}

bool BioXASBeamline::beamAvailable() const
{
	bool result = false;

	if (connected()) {
		// The beam is considered available if all shutters are open.
		result = (
					photonShutterUpstream_->value() == Shutters::Open &&
					photonShutterDownstream_->value() == Shutters::Open &&
					safetyShutterUpstream_->value() == Shutters::Open &&
					safetyShutterDownstream_->value() == Shutters::Open
					);
	}

	return result;
}

AMAction3* BioXASBeamline::createTurnOffBeamActions()
{
	AMAction3 *action = 0;

	if (safetyShutterDownstream_ && safetyShutterDownstream_->isConnected()) {

		// For bi-state controls, the move setpoint for 'close' is 0 and the desired state is 4.

		AMListAction3 *beamOff = new AMListAction3(new AMListActionInfo3("Turn off beam", "Turn off beam"), AMListAction3::Sequential);
		beamOff->addSubAction(AMActionSupport::buildControlMoveAction(safetyShutterDownstream_, 0));
		beamOff->addSubAction(AMActionSupport::buildControlWaitAction(safetyShutterDownstream_, 4, 5));

		action = beamOff;
	}

	return action;
}

AMAction3* BioXASBeamline::createTurnOnBeamActions()
{
	AMAction3 *action = 0;

	if (safetyShutterDownstream_ && safetyShutterDownstream_->isConnected()) {

		// For bi-state controls, the move setpoint for 'open' is 1 and the desired state is 1.

		AMListAction3 *beamOn = new AMListAction3(new AMListActionInfo3("Turn off beam", "Turn off beam"), AMListAction3::Sequential);
		beamOn->addSubAction(AMActionSupport::buildControlMoveAction(safetyShutterDownstream_, 1));
		beamOn->addSubAction(AMActionSupport::buildControlWaitAction(safetyShutterDownstream_, 1, 5));

		action = beamOn;
	}

	return action;
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

void BioXASBeamline::setupStorageRing()
{
	CLSStorageRing::sr1();
}

void BioXASBeamline::setupComponents()
{
	// The upstream photon shutter.
	photonShutterUpstream_ = new CLSBiStateControl("PhotonShutter", "BioXAS photon shutter", "IPSH1407-I00-01:state", "IPSH1407-I00-01:opr:open", "IPSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( photonShutterUpstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// The downstream photon shutter.
	photonShutterDownstream_ = new CLSBiStateControl("PhotonShutter", "BioXAS photon shutter", "IPSH1407-I00-02:state", "IPSH1407-I00-02:opr:open", "IPSH1407-I00-02:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( photonShutterDownstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// The upstream safety shutter.
	safetyShutterUpstream_ = new CLSBiStateControl("SafetyShutter", "BioXAS safety Shutter", "SSH1407-I00-01:state", "SSH1407-I00-01:opr:open", "SSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( safetyShutterUpstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
}

BioXASBeamline::BioXASBeamline(const QString &controlName) :
	CLSBeamline(controlName)
{
	// Initialize member variables.

	connected_ = false;

	photonShutterUpstream_ = 0;
	photonShutterDownstream_ = 0;
	safetyShutterUpstream_ = 0;
	safetyShutterDownstream_ = 0;

	m2Mirror_ = 0;

	// Setup procedures.

	setupStorageRing();
	setupComponents();
}
