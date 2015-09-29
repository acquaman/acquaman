#include "BioXASShutters.h"

BioXASShutters::BioXASShutters(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Set up shutter controls.

	photonShutterFEUpstream_ = new CLSBiStateControl("PhotonShutterFE1", "BioXAS front end photon shutter 1", "IPSH1407-I00-01:state", "IPSH1407-I00-01:opr:open", "IPSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( photonShutterFEUpstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	photonShutterFEDownstream_ = new CLSBiStateControl("PhotonShutterFE2", "BioXAS front end photon shutter 2", "IPSH1407-I00-02:state", "IPSH1407-I00-02:opr:open", "IPSH1407-I00-02:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( photonShutterFEDownstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	safetyShutterFE_ = new CLSBiStateControl("SafetyShutterFE", "BioXAS front end safety shutter", "SSH1407-I00-01:state", "SSH1407-I00-01:opr:open", "SSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( safetyShutterFE_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Set up the shutters control.

	shuttersControl_ = new BioXASShuttersControl("BioXASShuttersControl", this);
	shuttersControl_->setPhotonShutterUpstreamFE(photonShutterFEUpstream_);
	shuttersControl_->setPhotonShutterDownstreamFE(photonShutterFEDownstream_);
	shuttersControl_->setSafetyShutterFE(safetyShutterFE_);
}

BioXASShutters::~BioXASShutters()
{

}

bool BioXASShutters::isConnected() const
{
	bool connected = (
				photonShutterFEUpstream_ && photonShutterFEUpstream_->isConnected() &&
				photonShutterFEDownstream_ && photonShutterFEDownstream_->isConnected() &&
				safetyShutterFE_ && safetyShutterFE_->isConnected() &&
				shuttersControl_ && shuttersControl_->isConnected()
				);

	return connected;
}
