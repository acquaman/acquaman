#include "BioXASFrontEndBeamStatus.h"

BioXASFrontEndBeamStatus::BioXASFrontEndBeamStatus(BioXASFrontEndShutters *shutters, BioXASValves *valves, QObject *parent) :
	BioXASBeamStatus("BioXASFrontEndBeamStatus", parent)
{
	// Initialize class variables.

	frontEndShutters_ = 0;
	beamlineValves_ = 0;

	beamStatus_ = new BioXASFrontEndBeamStatusControl(this);

	// Current settings.

	setFrontEndShutters(shutters);
	setBeamlineValves(valves);
}

BioXASFrontEndBeamStatus::~BioXASFrontEndBeamStatus()
{

}

bool BioXASFrontEndBeamStatus::isConnected() const
{
	bool connected = (
				frontEndShutters_ && frontEndShutters_->isConnected() &&
				beamlineValves_ && beamlineValves_->isConnected() &&
				beamStatus_ && beamStatus_->isConnected()
				);

	return connected;
}

void BioXASFrontEndBeamStatus::setFrontEndShutters(BioXASFrontEndShutters *newShutters)
{
	if (frontEndShutters_ != newShutters) {

		if (frontEndShutters_) {
			beamStatus_->setPhotonShutterUpstream(0);
			beamStatus_->setVacuumValve(0);
			beamStatus_->setFastValve(0);
			beamStatus_->setPhotonShutterDownstream(0);
			beamStatus_->setSafetyShutter(0);
		}

		frontEndShutters_ = newShutters;

		if (frontEndShutters_) {
			beamStatus_->setPhotonShutterUpstream(frontEndShutters_->photonShutterUpstream());
			beamStatus_->setVacuumValve(frontEndShutters_->vacuumValve());
			beamStatus_->setFastValve(frontEndShutters_->fastValve());
			beamStatus_->setPhotonShutterDownstream(frontEndShutters_->photonShutterDownstream());
			beamStatus_->setSafetyShutter(frontEndShutters_->safetyShutter());
		}

		emit frontEndShuttersChanged(frontEndShutters_);
	}
}

void BioXASFrontEndBeamStatus::setBeamlineValves(BioXASValves *newValves)
{
	if (beamlineValves_ != newValves) {

		if (beamlineValves_) {
			beamStatus_->setValves(0);
		}

		beamlineValves_ = newValves;

		if (beamlineValves_) {
			beamStatus_->setValves(beamlineValves_->valvesControl());
		}

		emit beamlineValvesChanged(beamlineValves_);
	}
}
