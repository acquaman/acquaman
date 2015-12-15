#include "BioXASSidePOEBeamStatus.h"

BioXASSidePOEBeamStatus::BioXASSidePOEBeamStatus(QObject *parent) :
	BioXASBeamlineBeamStatus("BioXASSidePOEBeamStatus", parent)
{
	// Initialize class variables.

	mirrorMask_ = 0;
	monoMask_ = 0;
}

BioXASSidePOEBeamStatus::~BioXASSidePOEBeamStatus()
{

}

void BioXASSidePOEBeamStatus::setMirrorMask(AMControl *newControl)
{
	if (mirrorMask_ != newControl) {

		if (mirrorMask_)
			removeChildControl(mirrorMask_); // there are no setpoints for these controls yet, so no call to add/removeBiStateControl.

		mirrorMask_ = newControl;

		if (mirrorMask_)
			addChildControl(mirrorMask_);

		emit mirrorMaskChanged(mirrorMask_);
	}
}

void BioXASSidePOEBeamStatus::setMonoMask(BioXASSSRLMonochromatorMaskControl *newControl)
{
	if (monoMask_ != newControl) {

		if (monoMask_)
			removeChildControl(monoMask_);

		monoMask_ = newControl;

		if (monoMask_)
			addChildControl(monoMask_);

		emit monoMaskChanged(monoMask_);
	}
}

void BioXASSidePOEBeamStatus::updateConnected()
{
	bool connected = (
				BioXASBeamlineBeamStatus::isConnected() &&
				mirrorMask_ && mirrorMask_->isConnected() &&
				monoMask_ && monoMask_->isConnected()
				);

	setConnected(connected);
}
