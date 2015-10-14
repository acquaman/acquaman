#include "BioXASFrontEndShutters.h"

BioXASFrontEndShutters:: BioXASFrontEndShutters(QObject *parent) :
	BioXASBeamlineComponent("BioXASFrontEndShutters", parent)
{
	// Set up shutter controls.

	photonShutterUpstream_ = new AMReadOnlyPVControl(name()+"PhotonShutter1", "IPSH1407-I00-01:state", this);
	connect( photonShutterUpstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	photonShutterDownstream_ = new CLSBiStateControl(name()+"PhotonShutter2", name()+"PhotonShutter2", "IPSH1407-I00-02:state", "IPSH1407-I00-02:opr:open", "IPSH1407-I00-02:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( photonShutterDownstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	safetyShutter_ = new CLSBiStateControl(name()+"SafetyShutter", name()+"SafetyShutter", "SSH1407-I00-01:state", "SSH1407-I00-01:opr:open", "SSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( safetyShutter_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
}

BioXASFrontEndShutters::~BioXASFrontEndShutters()
{

}

bool BioXASFrontEndShutters::isConnected() const
{
	bool connected = (
				photonShutterUpstream_ && photonShutterUpstream_->isConnected() &&
				photonShutterDownstream_ && photonShutterDownstream_->isConnected() &&
				safetyShutter_ && safetyShutter_->isConnected()
				);

	return connected;
}
