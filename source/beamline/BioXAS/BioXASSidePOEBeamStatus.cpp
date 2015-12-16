#include "BioXASSidePOEBeamStatus.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskControl.h"

BioXASSidePOEBeamStatus::BioXASSidePOEBeamStatus(const QString &name, QObject *parent) :
	BioXASFrontEndBeamStatus(name, parent)
{
	// Initialize class variables.

	mirrorMaskState_ = 0;
	monoMask_ = 0;
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

void BioXASSidePOEBeamStatus::setMonoMask(BioXASSSRLMonochromatorMaskControl *newControl)
{
	if (monoMask_ != newControl) {

		if (monoMask_)
			removeBiStateControl(monoMask_);

		monoMask_ = newControl;

		if (monoMask_)
			addBiStateControl(monoMask_, BioXASSSRLMonochromatorMaskControl::Open, BioXASSSRLMonochromatorMaskControl::Closed);

		emit monoMaskChanged(monoMask_);
	}
}
