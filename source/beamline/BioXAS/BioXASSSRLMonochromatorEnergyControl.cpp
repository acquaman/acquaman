#include "BioXASSSRLMonochromatorEnergyControl.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include <math.h>

BioXASSSRLMonochromatorEnergyControl::BioXASSSRLMonochromatorEnergyControl(const QString &name, QObject *parent) :
	AMPseudoMotorControl(name, "eV", parent)
{
	// Initialize member variables.

	hc_ = 12398.42;
	crystal2D_ = 3.8403117;
	energyOffsetDeg_ = 0;
	m1AngleOffset_ = -0.4;
	thetaBraggOffset_ = 180.0;

	bragg_ = 0;
	region_ = 0;
}

BioXASSSRLMonochromatorEnergyControl::~BioXASSSRLMonochromatorEnergyControl()
{

}

bool BioXASSSRLMonochromatorEnergyControl::canMeasure()
{
	bool result = false;

	if (isConnected())
		result = ( bragg_->canMeasure() && region_->canMeasure() );

	return result;
}

bool BioXASSSRLMonochromatorEnergyControl::canMove()
{
	bool result = false;

	if (isConnected())
		result = ( bragg_->canMove() );

	return result;
}

bool BioXASSSRLMonochromatorEnergyControl::canStop()
{
	bool result = false;

	if (isConnected())
		result = ( bragg_->canStop() );

	return false;
}

void BioXASSSRLMonochromatorEnergyControl::setBraggControl(AMControl *newControl)
{
	if (bragg_ != newControl) {

		if (bragg_)
			removeChildControl(bragg_);

		bragg_ = newControl;

		if (bragg_)
			addChildControl(bragg_);
	}
}

void BioXASSSRLMonochromatorEnergyControl::setRegionControl(AMControl *newControl)
{
	if (region_ != newControl) {

		if (region_)
			removeChildControl(region_);

		region_ = newControl;

		if (region_)
			addChildControl(region_);
	}
}

void BioXASSSRLMonochromatorEnergyControl::setEnergyCalibration(double newEnergy)
{
	if (isConnected()) {
		// Calculate needed offset.
		double newOffset = calibrateEnergy(value(), newEnergy);

		// Set the new angle offset.
		setDeltaTheta(newOffset);

		// Update the control value.
		updateValue();
	}
}

void BioXASSSRLMonochromatorEnergyControl::setDeltaTheta(double newAngle)
{
	if (deltaTheta_ != newAngle) {
		deltaTheta_ = newAngle;
		emit deltaThetaChanged(deltaTheta_);
	}
}

void BioXASSSRLMonochromatorEnergyControl::updateConnected()
{
	bool isConnected = (
				bragg_ && bragg_->isConnected() &&
				region_ && region_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASSSRLMonochromatorEnergyControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateEnergy(hc_, crystal2D_, deltaTheta_, bragg_->value(), region_->value(), m1AngleOffset_, thetaBraggOffset_));
	}
}

void BioXASSSRLMonochromatorEnergyControl::updateIsMoving()
{
	if (isConnected()) {
		setIsMoving( bragg_->isMoving() || region_->isMoving() );
	}
}

AMAction3* BioXASSSRLMonochromatorEnergyControl::createMoveAction(double setpoint)
{
	return 0;
}

double BioXASSSRLMonochromatorEnergyControl::calibrateEnergy(double oldEnergy, double newEnergy) const
{
	// Gather pre-calibration information.
	double oldOffset = deltaTheta_;
	double hc = hc_;
	double crystal2D = crystal2D_;
	double braggPosition = bragg_->value();

	// Calculate changes needed for calibration.
	double deltaEnergy = newEnergy - oldEnergy;
	double deltaOffset = hc / (crystal2D * oldEnergy * oldEnergy * cos(braggPosition * M_PI / 180)) * deltaEnergy * 180 / M_PI;

	// Calibration results.
	double newOffset = oldOffset + deltaOffset;

	return newOffset;
}

double BioXASSSRLMonochromatorEnergyControl::calculateBraggAngleRegionA(double deltaTheta, double braggPosition, double m1AngleOffset, double thetaBraggOffset)
{
	return (calculateBraggAngleRegionB(deltaTheta, braggPosition, m1AngleOffset, thetaBraggOffset) - 180);
}

double BioXASSSRLMonochromatorEnergyControl::calculateBraggAngleRegionB(double deltaTheta, double braggPosition, double m1AngleOffset, double thetaBraggOffset)
{
	return (thetaBraggOffset - braggPosition + m1AngleOffset + deltaTheta);
}

double BioXASSSRLMonochromatorEnergyControl::calculateEnergy(double hc, double crystal2D, double deltaTheta, double braggPosition, double region, double m1AngleOffset, double thetaBraggOffset)
{
	double result = 0;
	double braggAngle = 0;
	bool validRegion = false;

	if ((int)region == BioXASSSRLMonochromator::Region::A) {
		braggAngle = calculateBraggAngleRegionA(deltaTheta, braggPosition, m1AngleOffset, thetaBraggOffset);
		validRegion = true;

	} else if ((int)region == BioXASSSRLMonochromator::Region::B) {
		braggAngle = calculateBraggAngleRegionB(deltaTheta, braggPosition, m1AngleOffset, thetaBraggOffset);
		validRegion = true;
	}

	if (validRegion)
		result = ( hc / (crystal2D + sin(braggAngle * M_PI / 180)) );

	return result;
}
