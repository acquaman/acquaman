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
				photonShutterFEUpstream_ && photonShutterFEUpstream_->isConnected() &&
				photonShutterFEDownstream_ && photonShutterFEDownstream_->isConnected() &&
				safetyShutterFE_ && safetyShutterFE_->isConnected()
				);

	return connected;
}

bool BioXASBeamline::beamOff() const
{
	bool result = false;

	if (isConnected())
		result = (photonShutterFEUpstream_->isClosed() && photonShutterFEDownstream_->isClosed() && safetyShutterFE_->isClosed());

	return result;
}

bool BioXASBeamline::beamOn() const
{
	bool result = false;

	if (isConnected())
		result = (photonShutterFEUpstream_->isOpen() && photonShutterFEDownstream_->isOpen() && safetyShutterFE_->isOpen());

	return result;
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
	// The front end upstream photon shutter.
	photonShutterFEUpstream_ = new CLSBiStateControl("PhotonShutterFE1", "BioXAS front end photon shutter 1", "IPSH1407-I00-01:state", "IPSH1407-I00-01:opr:open", "IPSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( photonShutterFEUpstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// The front end downstream photon shutter.
	photonShutterFEDownstream_ = new CLSBiStateControl("PhotonShutterFE2", "BioXAS front end photon shutter 2", "IPSH1407-I00-02:state", "IPSH1407-I00-02:opr:open", "IPSH1407-I00-02:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( photonShutterFEDownstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// The front end safety shutter.
	safetyShutterFE_ = new CLSBiStateControl("SafetyShutterFE", "BioXAS front end safety shutter", "SSH1407-I00-01:state", "SSH1407-I00-01:opr:open", "SSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( safetyShutterFE_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
}

BioXASBeamline::BioXASBeamline(const QString &controlName) :
	CLSBeamline(controlName)
{
	// Initialize member variables.

	connected_ = false;

	photonShutterFEUpstream_ = 0;
	photonShutterFEDownstream_ = 0;
	safetyShutterFE_ = 0;

	// Setup procedures.

	setupComponents();
}
