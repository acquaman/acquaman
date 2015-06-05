#include "BioXASSSRLMonochromatorEncoderEnergyControl.h"

BioXASSSRLMonochromatorEncoderEnergyControl::BioXASSSRLMonochromatorEncoderEnergyControl(const QString &name, QObject *parent) :
	BioXASSSRLMonochromatorEnergyControl(name, parent)
{
	// Current settings.

	updateStates();
}

BioXASSSRLMonochromatorEncoderEnergyControl::~BioXASSSRLMonochromatorEncoderEnergyControl()
{

}

void BioXASSSRLMonochromatorEncoderEnergyControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateEnergyFromBraggPosition(hc_, crystal2D_, bragg_->value(), region_->value(), m1MirrorPitch_->value(), thetaBraggOffset_, regionOffset_));
	}
}
