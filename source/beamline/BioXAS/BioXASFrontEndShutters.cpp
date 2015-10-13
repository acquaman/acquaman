#include "BioXASFrontEndShutters.h"

BioXASFrontEndShutters:: BioXASFrontEndShutters(QObject *parent) :
	BioXASBeamlineComponent("BioXASFrontEndShutters", parent)
{
	// Set up shutter controls.

	photonShutterUpstream_ = new CLSBiStateControl(name()+"PhotonShutter1", name()+"PhotonShutter1", "IPSH1407-I00-01:state", "IPSH1407-I00-01:opr:open", "IPSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( photonShutterUpstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	vacuumValve_ = new CLSBiStateControl(name()+"VacuumValve", name()+"VacuumValve", "VVR1407-I00-01:state", "VVR1407-I00-01:opr:open", "VVR1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( vacuumValve_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	fastValve_ = new CLSBiStateControl(name()+"FastValve", name()+"FastValve", "VVF1407-I00-01:state", "VVF1407-I00-01:opr:open", "VVF1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( fastValve_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	photonShutterDownstream_ = new CLSBiStateControl("PhotonShutterFE2", "BioXAS front end photon shutter 2", "IPSH1407-I00-02:state", "IPSH1407-I00-02:opr:open", "IPSH1407-I00-02:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( photonShutterDownstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	safetyShutter_ = new CLSBiStateControl("SafetyShutterFE", "BioXAS front end safety shutter", "SSH1407-I00-01:state", "SSH1407-I00-01:opr:open", "SSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( safetyShutter_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
}

BioXASFrontEndShutters::~BioXASFrontEndShutters()
{

}

bool BioXASFrontEndShutters::isConnected() const
{
	bool connected = (
				photonShutterUpstream_ && photonShutterUpstream_->isConnected() &&
				vacuumValve_ && vacuumValve_->isConnected() &&
				fastValve_ && fastValve_->isConnected() &&
				photonShutterDownstream_ && photonShutterDownstream_->isConnected() &&
				safetyShutter_ && safetyShutter_->isConnected()
				);

	return connected;
}
