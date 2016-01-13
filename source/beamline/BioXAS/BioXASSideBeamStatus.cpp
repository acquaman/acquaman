#include "BioXASSideBeamStatus.h"
#include "beamline/BioXAS/BioXASFrontEndShutters.h"
#include "beamline/BioXAS/BioXASMasterValves.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"
#include "beamline/CLS/CLSBiStateControl.h"

BioXASSideBeamStatus::BioXASSideBeamStatus(const QString &name, QObject *parent) :
	BioXASBeamStatus(name, parent)
{
	// Initialize class variables.

	frontEndShutters_ = 0;
	valves_ = 0;
	mirrorMaskState_ = 0;
	monoMaskState_ = 0;
	endstationShutter_ = 0;
}

BioXASSideBeamStatus::~BioXASSideBeamStatus()
{

}

bool BioXASSideBeamStatus::isConnected() const
{
	bool connected = (
				frontEndShutters_ && frontEndShutters_->isConnected() &&
				valves_ && valves_->isConnected() &&
				mirrorMaskState_ && mirrorMaskState_->isConnected() &&
				monoMaskState_ && monoMaskState_->isConnected() &&
				endstationShutter_ && endstationShutter_->isConnected()
				);

	return connected;
}

void BioXASSideBeamStatus::setFrontEndShutters(BioXASFrontEndShutters *newShutters)
{
	if (frontEndShutters_ != newShutters) {

		if (frontEndShutters_)
			removeBiStateControl(frontEndShutters_);

		frontEndShutters_ = newShutters;

		if (frontEndShutters_)
			addBiStateControl(frontEndShutters_, BioXASShutters::Open, BioXASShutters::Closed);

		emit frontEndShuttersChanged(frontEndShutters_);
	}
}

void BioXASSideBeamStatus::setValves(BioXASMasterValves *newValves)
{
	if (valves_ != newValves) {

		if (valves_)
			removeBiStateControl(valves_);

		valves_ = newValves;

		if (valves_)
			addBiStateControl(valves_, BioXASValves::Open, BioXASValves::Closed);

		emit valvesChanged(valves_);
	}
}

void BioXASSideBeamStatus::setMirrorMaskState(BioXASM1MirrorMaskState *newControl)
{
	if (mirrorMaskState_ != newControl) {

		if (mirrorMaskState_)
			removeBiStateControl(mirrorMaskState_);

		mirrorMaskState_ = newControl;

		if (mirrorMaskState_)
			addBiStateControl(mirrorMaskState_, BioXASM1MirrorMaskState::Open, BioXASM1MirrorMaskState::Closed);

		emit mirrorMaskStateChanged(mirrorMaskState_);
	}
}

void BioXASSideBeamStatus::setMonoMaskState(BioXASSSRLMonochromatorMaskState *newControl)
{
	if (monoMaskState_ != newControl) {

		if (monoMaskState_)
			removeBiStateControl(monoMaskState_);

		monoMaskState_ = newControl;

		if (monoMaskState_)
			addBiStateControl(monoMaskState_, BioXASSSRLMonochromatorMaskState::Open, BioXASSSRLMonochromatorMaskState::Closed);

		emit monoMaskStateChanged(monoMaskState_);
	}
}

void BioXASSideBeamStatus::setEndstationShutter(CLSBiStateControl *newControl)
{
	if (endstationShutter_ != newControl) {

		if (endstationShutter_)
			removeBiStateControl(endstationShutter_);

		endstationShutter_ = newControl;

		if (endstationShutter_)
			addBiStateControl(endstationShutter_, CLSBiStateControl::Open, CLSBiStateControl::Closed);

		emit endstationShutterChanged(endstationShutter_);
	}
}
