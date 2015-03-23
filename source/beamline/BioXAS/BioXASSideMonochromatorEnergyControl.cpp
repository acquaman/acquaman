#include "BioXASSideMonochromatorEnergyControl.h"

BioXASSideMonochromatorEnergyControl::BioXASSideMonochromatorEnergyControl(QObject *parent) :
    BioXASSSRLMonochromatorEnergyControl("EnergyEV", "BL1607-5-I22:Energy:EV:fbk", "BL1607-5-I22:Energy:EV", "BL1607-5-I22:Energy:status", "BL1607-5-I22:Energy:stop", parent)
{

}

BioXASSideMonochromatorEnergyControl::~BioXASSideMonochromatorEnergyControl()
{

}
