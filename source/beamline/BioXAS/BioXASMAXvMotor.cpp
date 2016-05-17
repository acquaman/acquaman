#include "BioXASMAXvMotor.h"
#include "actions3/BioXAS/BioXASMAXvMotorMoveToLimitAction.h"
#include "util/AMErrorMonitor.h"

BioXASMAXvMotor::BioXASMAXvMotor(const QString &name, const QString &baseName, const QString &description, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds, QObject *parent, QString pvUnitFieldName) :
	CLSMAXvMotor(name, baseName, description, hasEncoder, tolerance, moveStartTimeoutSeconds, parent, pvUnitFieldName)
{

}

BioXASMAXvMotor::~BioXASMAXvMotor()
{

}

AMAction3* BioXASMAXvMotor::createMoveToLimitAction(CLSMAXvMotor::Limit setpoint)
{
	return new BioXASMAXvMotorMoveToLimitAction(new BioXASMAXvMotorMoveToLimitActionInfo(toInfo(), setpoint), this);
}

AMControl::FailureExplanation BioXASMAXvMotor::move(double setpoint)
{
	// Update setpoint.

	setpoint_ = setpoint;

	// Check whether the motor is at the setpoint, within tolerance.
	// If so, report as succeeded. If not, start regular move.

	if (!attemptMoveWhenWithinTolerance_ && inPosition()) {
		emit moveSucceeded();
		return AMControl::NoFailure;

	} else {
		return CLSMAXvMotor::move(setpoint);
	}
}

AMControl::FailureExplanation BioXASMAXvMotor::moveToLimit(CLSMAXvMotor::Limit limit)
{
	// Check that the limit input is valid.

	if (limit != CLSMAXvMotor::LimitCW || limit != CLSMAXvMotor::LimitCCW) {
		AMErrorMon::alert(this, CLSMAXVMOTOR_INVALID_LIMIT_SETPOINT, QString("Failed to move '%1' to the '%2' limit. The provided limit is invalid.").arg(name()).arg(limitToString(limit)));
		return AMControl::OtherFailure;
	}

	// Identify the motor's step slope sign.

	bool positiveSlope = false;
	if(stepCalibrationSlope_->value() > 0)
		positiveSlope = true;

	// Identify whether the move to the limit would be a positive movement.

	bool positiveMovement = false;
	if ((limit == CLSMAXvMotor::LimitCW && !positiveSlope) || (limit == CLSMAXvMotor::LimitCCW && positiveSlope))
		positiveMovement = true;

	// Identify the corresponding setpoint.

	double setpoint = -DBL_MAX;
	if (positiveMovement)
		setpoint = DBL_MAX;

	// Initiate movement.

	return move(setpoint);
}
