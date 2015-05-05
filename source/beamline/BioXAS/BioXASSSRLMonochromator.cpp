#include "BioXASSSRLMonochromator.h"

BioXASSSRLMonochromator::BioXASSSRLMonochromator(QObject *parent) :
	BioXASMonochromator(parent)
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

AMAction3* BioXASSSRLMonochromator::createCalibrateBraggPositionAction(double newPosition)
{
	AMAction3 *action = 0;

	if (braggMotor()->isConnected()) {
		action = braggMotor()->createEGUSetPositionAction(newPosition);
	}

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

void BioXASSSRLMonochromator::calibrateBraggPosition(double newBraggPosition)
{
	if (braggMotor()->isConnected()) {
		braggMotor()->setEGUSetPosition(newBraggPosition);
	}
}
