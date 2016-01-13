#include "BioXASSidePOEBeamStatus.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"

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

bool BioXASSidePOEBeamStatus::isConnected() const
{
	bool connected = (
				BioXASFrontEndBeamStatus::isConnected() &&
				mirrorMaskState_ && mirrorMaskState_->isConnected() &&
				monoMaskState_ && monoMaskState_->isConnected()
				);

	return connected;
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
			addBiStateControl(monoMaskState_, BioXASSSRLMonochromatorMaskState::Open, BioXASSSRLMonochromatorMaskState::Closed);

		emit monoMaskStateChanged(monoMaskState_);
	}
}
