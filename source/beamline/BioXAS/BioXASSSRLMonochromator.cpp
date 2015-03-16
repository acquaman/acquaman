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

AMAction3* BioXASSSRLMonochromator::createSetEnergyCalibrationAction(double newEnergy)
{
	AMAction3 *action = 0;

	if (isConnected()) {
		// Calculate needed offset.
		double newOffset = calibrateEnergy(energyControl()->value(), newEnergy);

		// Set the new angle offset.
		action = braggMotor()->createEGUOffsetAction(newOffset);
	}

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

void BioXASSSRLMonochromator::setEnergyCalibration(double newEnergy)
{
	if (isConnected()) {
		// Calculate needed offset.
		double newOffset = calibrateEnergy(energyControl()->value(), newEnergy);

		// Set the new angle offset.
		braggMotor()->setEGUOffset(newOffset);
	}
}

void BioXASSSRLMonochromator::setRegion(double newRegion)
{
	if (regionControl()->isConnected())
		regionControl()->move(newRegion);
}

double BioXASSSRLMonochromator::calibrateEnergy(double oldEnergy, double newEnergy) const
{
	// Gather pre-calibration information.
	double oldOffset = braggMotor()->EGUOffset();

	// Calculate changes needed for calibration.
	double deltaEnergy = newEnergy - oldEnergy;
	double deltaOffset = hc() / (crystal2D() * oldEnergy * oldEnergy * cos(braggAngle() * M_PI / 180)) * deltaEnergy * 180 / M_PI;

	// Calibration results.
	double newOffset = oldOffset + deltaOffset;

	return newOffset;
}
