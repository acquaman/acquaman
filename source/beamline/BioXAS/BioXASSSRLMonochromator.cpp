#include "BioXASSSRLMonochromator.h"

BioXASSSRLMonochromator::BioXASSSRLMonochromator(QObject *parent) :
	QObject(parent)
{

}

BioXASSSRLMonochromator::~BioXASSSRLMonochromator()
{

}

AMAction3* BioXASSSRLMonochromator::createSetEnergyAction(double newEnergy)
{
	AMAction3 *action = 0;

	if (energyControl()->isConnected())
		action = AMActionSupport::buildControlMoveAction(energyControl(), newEnergy);

	return action;
}

AMAction3* BioXASSSRLMonochromator::createSetRegionAction(double newRegion)
{
	AMAction3 *action = 0;

	if (regionControl()->isConnected())
		action = AMActionSupport::buildControlMoveAction(regionControl(), newRegion);

	return action;
}

void BioXASSSRLMonochromator::setEnergy(double newEnergy)
{
	if (energyControl()->isConnected())
		energyControl()->move(newEnergy);
}

void BioXASSSRLMonochromator::setRegion(double newRegion)
{
	if (regionControl()->isConnected())
		regionControl()->move(newRegion);
}
