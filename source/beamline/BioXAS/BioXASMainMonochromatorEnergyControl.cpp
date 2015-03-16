#include "BioXASMainMonochromatorEnergyControl.h"

BioXASMainMonochromatorEnergyControl::BioXASMainMonochromatorEnergyControl(QObject *parent) :
	BioXASSSRLMonochromatorEnergyControl("EnergyEV", "BL1607-5-I21:Energy:EV:fbk", "BL1607-5-I21:Energy:EV", "BL1607-5-I21:Energy:status", QString("BL1607-5-I21:Energy:stop"), parent)
{

}

BioXASMainMonochromatorEnergyControl::~BioXASMainMonochromatorEnergyControl()
{

}

