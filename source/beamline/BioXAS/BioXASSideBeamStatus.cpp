#include "BioXASSideBeamStatus.h"
#include "beamline/BioXAS/BioXASSideShutters.h"
#include "beamline/BioXAS/BioXASMasterValves.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"
#include "beamline/CLS/CLSBiStateControl.h"

BioXASSideBeamStatus::BioXASSideBeamStatus(const QString &name, QObject *parent) :
	BioXASBeamStatus(name, parent)
{
	// Initialize class variables.

	shutters_ = 0;
	valves_ = 0;
	mirrorMaskState_ = 0;
	monoMaskState_ = 0;
}

BioXASSideBeamStatus::~BioXASSideBeamStatus()
{

}

bool BioXASSideBeamStatus::isConnected() const
{
	bool connected = (
				shutters_ && shutters_->isConnected() &&
				valves_ && valves_->isConnected() &&
				mirrorMaskState_ && mirrorMaskState_->isConnected() &&
				monoMaskState_ && monoMaskState_->isConnected()
				);

	return connected;
}

void BioXASSideBeamStatus::setShutters(BioXASSideShutters *newShutters)
{
	if (shutters_ != newShutters) {

		if (shutters_)
			removeBiStateControl(shutters_);

		shutters_ = newShutters;

		if (shutters_)
			addBiStateControl(shutters_, BioXASShutters::Open, BioXASShutters::Closed);

		emit shuttersChanged(shutters_);
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
