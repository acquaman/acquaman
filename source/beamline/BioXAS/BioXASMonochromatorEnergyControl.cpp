#include "BioXASMonochromatorEnergyControl.h"

BioXASMonochromatorEnergyControl::BioXASMonochromatorEnergyControl(const QString &name, QObject *parent) :
	AMPseudoMotorControl(name, "eV", parent)
{

}

BioXASMonochromatorEnergyControl::~BioXASMonochromatorEnergyControl()
{

}
