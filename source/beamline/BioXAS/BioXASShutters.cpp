#include "BioXASShutters.h"

BioXASShutters::BioXASShutters(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Set up shutter controls.

	photonShutterUpstream_ = new CLSBiStateControl("PhotonShutterFE1", "BioXAS front end photon shutter 1", "IPSH1407-I00-01:state", "IPSH1407-I00-01:opr:open", "IPSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( photonShutterUpstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	photonShutterDownstream_ = new CLSBiStateControl("PhotonShutterFE2", "BioXAS front end photon shutter 2", "IPSH1407-I00-02:state", "IPSH1407-I00-02:opr:open", "IPSH1407-I00-02:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( photonShutterDownstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	safetyShutter_ = new CLSBiStateControl("SafetyShutterFE", "BioXAS front end safety shutter", "SSH1407-I00-01:state", "SSH1407-I00-01:opr:open", "SSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( safetyShutter_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Set up the shutters control.

	shuttersControl_ = new BioXASShuttersControl("BioXASShuttersControl", this);
	shuttersControl_->setPhotonShutterUpstream(photonShutterUpstream_);
	shuttersControl_->setPhotonShutterDownstream(photonShutterDownstream_);
	shuttersControl_->setSafetyShutter(safetyShutter_);
}

BioXASShutters::~BioXASShutters()
{

}

bool BioXASShutters::isConnected() const
{
	bool connected = (
				photonShutterUpstream_ && photonShutterUpstream_->isConnected() &&
				photonShutterDownstream_ && photonShutterDownstream_->isConnected() &&
				safetyShutter_ && safetyShutter_->isConnected() &&
				shuttersControl_ && shuttersControl_->isConnected()
				);

	return connected;
}
