#include "BioXASSidePOEBeamStatus.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"

BioXASSidePOEBeamStatus::BioXASSidePOEBeamStatus(const QString &name, QObject *parent) :
	BioXASFrontEndBeamStatus(name, parent)
{
	// Initialize class variables.

	mirrorMaskState_ = 0;
	monoMaskState_ = 0;
}

BioXASSidePOEBeamStatus::~BioXASSidePOEBeamStatus()
{

}

void BioXASSidePOEBeamStatus::setMirrorMaskState(BioXASM1MirrorMaskState *newControl)
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

void BioXASSidePOEBeamStatus::setMonoMaskState(AMControl *newControl)
{
	if (monoMaskState_ != newControl) {

		if (monoMaskState_)
			removeBiStateControl(monoMaskState_);

		monoMaskState_ = newControl;

		if (monoMaskState_)
			addBiStateControl(monoMaskState_, 0, 1);

		emit monoMaskStateChanged(monoMaskState_);
	}
}
