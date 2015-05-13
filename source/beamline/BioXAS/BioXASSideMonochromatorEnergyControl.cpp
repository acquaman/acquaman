#include "BioXASSideMonochromatorEnergyControl.h"
#include "beamline/BioXAS/BioXASSideBeamline.h"

BioXASSideMonochromatorEnergyControl::BioXASSideMonochromatorEnergyControl(QObject *parent) :
	BioXASSSRLMonochromatorEnergyControl("SideEnergy", parent)
{

}

BioXASSideMonochromatorEnergyControl::~BioXASSideMonochromatorEnergyControl()
{

}
