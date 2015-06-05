#include "BioXASSSRLMonochromatorStepEnergyControl.h"

BioXASSSRLMonochromatorStepEnergyControl::BioXASSSRLMonochromatorStepEnergyControl(const QString &name, QObject *parent) :
	BioXASSSRLMonochromatorEnergyControl(name, parent)
{

}

BioXASSSRLMonochromatorStepEnergyControl::~BioXASSSRLMonochromatorStepEnergyControl()
{

}

void BioXASSSRLMonochromatorStepEnergyControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateEnergyFromBraggPosition(hc_, crystal2D_, bragg_->stepSetpointControl()->value(), region_->value(), m1MirrorPitch_->value(), thetaBraggOffset_, regionOffset_) );
	}
}
