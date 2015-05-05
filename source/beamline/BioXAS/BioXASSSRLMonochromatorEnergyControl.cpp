#include "BioXASSSRLMonochromatorEnergyControl.h"

BioXASSSRLMonochromatorEnergyControl::BioXASSSRLMonochromatorEnergyControl(const QString &name, const QString &readPVname, const QString &writePVname, const QString &movingPVname, const QString &stopPVname, QObject *parent, double tolerance, double moveStartTimeoutSeconds, AMAbstractControlStatusChecker *statusChecker, int stopValue, const QString &description) :
    AMPVwStatusControl(name, readPVname, writePVname, movingPVname, stopPVname, parent, tolerance, moveStartTimeoutSeconds, statusChecker, stopValue, description)
{

}

BioXASSSRLMonochromatorEnergyControl::~BioXASSSRLMonochromatorEnergyControl()
{

}

bool BioXASSSRLMonochromatorEnergyControl::isConnected() const
{
	bool connected = (
				AMPVwStatusControl::isConnected() &&
				hc_->isConnected() &&
				crystal2D_->isConnected() &&
				braggAngle_->isConnected() &&
				angleOffset_->isConnected()
				);

	return connected;
}

void BioXASSSRLMonochromatorEnergyControl::setEnergyCalibration(double newEnergy)
{
	if (isConnected()) {
		// Calculate needed offset.
		double newOffset = calibrateEnergy(value(), newEnergy);

		// Set the new angle offset.
		angleOffset_->move(newOffset);
	}
}

AMAction3* BioXASSSRLMonochromatorEnergyControl::createSetEnergyCalibrationAction(double newEnergy)
{
	AMAction3 *action = 0;

	if (isConnected()) {
		// Calculate needed offset.
		double newOffset = calibrateEnergy(value(), newEnergy);

		// Set the new angle offset.
		action = AMActionSupport::buildControlMoveAction(angleOffset_, newOffset, false);
	}

	return action;
}

double BioXASSSRLMonochromatorEnergyControl::calibrateEnergy(double oldEnergy, double newEnergy) const
{
	// Gather pre-calibration information.
	double oldOffset = angleOffset_->value();
	double hc = hc_->value();
	double crystal2D = crystal2D_->value();
	double braggAngle = braggAngle_->value();

	// Calculate changes needed for calibration.
	double deltaEnergy = newEnergy - oldEnergy;
	double deltaOffset = hc / (crystal2D * oldEnergy * oldEnergy * cos(braggAngle * M_PI / 180)) * deltaEnergy * 180 / M_PI;

	// Calibration results.
	double newOffset = oldOffset + deltaOffset;

	return newOffset;
}
