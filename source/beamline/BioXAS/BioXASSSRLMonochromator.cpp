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

AMAction3* BioXASSSRLMonochromator::createCalibrateBraggPositionAction(double newPosition)
{
	AMAction3 *action = 0;

	if (braggMotor()->isConnected()) {
		// calculate the needed offset.
		double newOffset = calculateBraggEncoderOffset(braggMotor()->value(), braggMotor()->encoderCalibrationAbsoluteOffset(), newPosition, braggMotor()->encoderCalibrationSlope());

		// set the new offset.
		action = braggMotor()->createEncoderCalibrationAbsoluteOffsetAction(newOffset);
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
		// calculate the needed encoder calibration offset.
		double newEncoderOffset = calculateBraggEncoderOffset(braggMotor()->value(), braggMotor()->encoderCalibrationAbsoluteOffset(), newBraggPosition, braggMotor()->encoderCalibrationSlope());

		// apply the new encoder offset.
		braggMotor()->setEncoderCalibrationAbsoluteOffset(newEncoderOffset);
	}
}

double BioXASSSRLMonochromator:: calculateBraggEncoderOffset(double oldPosition, double oldEncoderOffset, double newPosition, double encoderSlope)
{
	// Calculate the new offset.
	double newEncoderOffset = oldEncoderOffset + (oldPosition - newPosition) / encoderSlope;

	return newEncoderOffset;
}
