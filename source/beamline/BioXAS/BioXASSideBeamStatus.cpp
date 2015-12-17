#include "BioXASSideBeamStatus.h"

BioXASSideBeamStatus::BioXASSideBeamStatus(const QString &name, QObject *parent) :
	BioXASSideSOEBeamStatus(name, parent)
{
	// Initialize class variables.

	frontEndStatus_ = 0;
	poeStatus_ = 0;
	soeStatus_ = 0;

	// Current settings.

	setFrontEndStatus(new BioXASFrontEndBeamStatus(name+"FrontEnd", this));
	setPOEStatus(new BioXASSidePOEBeamStatus(name+"POE", this));
	setSOEStatus(new BioXASSideSOEBeamStatus(name+"SOE", this));
}

BioXASSideBeamStatus::~BioXASSideBeamStatus()
{

}

void BioXASSideBeamStatus::setShutters(BioXASFrontEndShutters *newControl)
{
	if (shutters() != newControl) {

		// Update our current shutters.

		BioXASSideSOEBeamStatus::setShutters(newControl);

		// Update the shutters for each of the beam status controls.

		if (frontEndStatus_)
			frontEndStatus_->setShutters(newControl);

		if (poeStatus_)
			poeStatus_->setShutters(newControl);

		if (soeStatus_)
			soeStatus_->setShutters(newControl);
	}
}

void BioXASSideBeamStatus::setValves(BioXASMasterValves *newControl)
{
	if (valves() != newControl) {

		// Update our current valves.

		BioXASSideSOEBeamStatus::setValves(newControl);

		// Update the valves for each of the beam status controls.

		if (frontEndStatus_)
			frontEndStatus_->setValves(newControl);

		if (poeStatus_)
			poeStatus_->setValves(newControl);

		if (soeStatus_)
			soeStatus_->setValves(newControl);
	}
}

void BioXASSideBeamStatus::setMirrorMaskState(BioXASM1MirrorMaskState *newControl)
{
	if (mirrorMaskState() != newControl) {

		// Update our current mask state.

		BioXASSideSOEBeamStatus::setMirrorMaskState(newControl);

		// Update the mask state for each of the beam status controls.

		if (poeStatus_)
			poeStatus_->setMirrorMaskState(newControl);

		if (soeStatus_)
			soeStatus_->setMirrorMaskState(newControl);
	}
}

void BioXASSideBeamStatus::setMonoMaskState(AMControl *newControl)
{
	if (monoMaskState() != newControl) {

		// Update our current mono mask state.

		BioXASSideSOEBeamStatus::setMonoMaskState(newControl);

		// Update the mono mask state for each of the beam status controls.

		if (poeStatus_)
			poeStatus_->setMonoMaskState(newControl);

		if (soeStatus_)
			soeStatus_->setMonoMaskState(newControl);
	}
}

void BioXASSideBeamStatus::setEndstationShutter(CLSBiStateControl *newControl)
{
	if (endstationShutter() != newControl) {

		// Update our current endstation shutter.

		BioXASSideSOEBeamStatus::setEndstationShutter(newControl);

		// Update the endstation shutter for each of the beam status controls.

		if (soeStatus_)
			soeStatus_->setEndstationShutter(newControl);
	}
}

void BioXASSideBeamStatus::setFrontEndStatus(BioXASFrontEndBeamStatus *newStatus)
{
	if (frontEndStatus_ != newStatus) {

		if (frontEndStatus_) {
			disconnect( frontEndStatus_, 0, this, 0 );
			removeBiStateControl(frontEndStatus_);
		}

		frontEndStatus_ = newStatus;

		if (frontEndStatus_) {
			addBiStateControl(frontEndStatus_, BioXASBeamStatus::On, BioXASBeamStatus::Off);

			connect( frontEndStatus_, SIGNAL(shuttersChanged(BioXASFrontEndShutters*)), this, SLOT(setShutters(BioXASFrontEndShutters*)) );
			connect( frontEndStatus_, SIGNAL(valvesChanged(BioXASMasterValves*)), this, SLOT(setValves(BioXASMasterValves*)) );

			// Update the front end status to use our controls.

			frontEndStatus_->setShutters(shutters());
			frontEndStatus_->setValves(valves());
		}

		emit frontEndStatusChanged(frontEndStatus_);
	}
}

void BioXASSideBeamStatus::setPOEStatus(BioXASSidePOEBeamStatus *newStatus)
{
	if (poeStatus_ != newStatus) {

		if (poeStatus_) {
			disconnect( poeStatus_, 0, this, 0 );
			removeBiStateControl(poeStatus_);
		}

		poeStatus_ = newStatus;

		if (poeStatus_) {
			addBiStateControl(poeStatus_, BioXASBeamStatus::On, BioXASBeamStatus::Off);

			connect( poeStatus_, SIGNAL(shuttersChanged(BioXASFrontEndShutters*)), this, SLOT(setShutters(BioXASFrontEndShutters*)) );
			connect( poeStatus_, SIGNAL(valvesChanged(BioXASMasterValves*)), this, SLOT(setValves(BioXASMasterValves*)) );
			connect( poeStatus_, SIGNAL(mirrorMaskStateChanged(BioXASM1MirrorMaskState*)), this, SLOT(setMirrorMaskState(BioXASM1MirrorMaskState*)) );
			connect( poeStatus_, SIGNAL(monoMaskStateChanged(AMControl*)), this, SLOT(setMonoMaskState(AMControl*)) );

			// Update the POE status to use our controls.

			poeStatus_->setShutters(shutters());
			poeStatus_->setValves(valves());
			poeStatus_->setMirrorMaskState(mirrorMaskState());
			poeStatus_->setMonoMaskState(monoMaskState());
		}

		emit poeStatusChanged(poeStatus_);
	}
}

void BioXASSideBeamStatus::setSOEStatus(BioXASSideSOEBeamStatus *newStatus)
{
	if (soeStatus_ != newStatus) {

		if (soeStatus_) {
			disconnect( soeStatus_, 0, this, 0 );
			removeBiStateControl(soeStatus_);
		}

		soeStatus_ = newStatus;

		if (soeStatus_) {
			addBiStateControl(soeStatus_, BioXASBeamStatus::On, BioXASBeamStatus::Off);

			connect( soeStatus_, SIGNAL(shuttersChanged(BioXASFrontEndShutters*)), this, SLOT(setShutters(BioXASFrontEndShutters*)) );
			connect( soeStatus_, SIGNAL(valvesChanged(BioXASMasterValves*)), this, SLOT(setValves(BioXASMasterValves*)) );
			connect( soeStatus_, SIGNAL(mirrorMaskStateChanged(BioXASM1MirrorMaskState*)), this, SLOT(setMirrorMaskState(BioXASM1MirrorMaskState*)) );
			connect( soeStatus_, SIGNAL(monoMaskStateChanged(AMControl*)), this, SLOT(setMonoMaskState(AMControl*)) );
			connect( soeStatus_, SIGNAL(endstationShutterChanged(CLSBiStateControl*)), this, SLOT(setEndstationShutter(CLSBiStateControl*)) );

			// Update the SOE status to use our controls.

			soeStatus_->setShutters(shutters());
			soeStatus_->setValves(valves());
			soeStatus_->setMirrorMaskState(mirrorMaskState());
			soeStatus_->setMonoMaskState(monoMaskState());
			soeStatus_->setEndstationShutter(endstationShutter());
		}

		emit soeStatusChanged(soeStatus_);
	}
}

void BioXASSideBeamStatus::updateConnected()
{
	bool connected = (
				frontEndStatus_ && frontEndStatus_->isConnected() &&
				poeStatus_ && poeStatus_->isConnected() &&
				soeStatus_ && soeStatus_->isConnected()
				);

	setConnected(connected);
}
