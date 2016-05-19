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

bool BioXASMAXvMotor::hasLimitSetpoint(CLSMAXvMotor::Limit limit) const
{
	return limitSetpointMap_.keys().contains(limit);
}

double BioXASMAXvMotor::limitSetpoint(CLSMAXvMotor::Limit limit) const
{
	return limitSetpointMap_.value(limit, 0);
}

AMAction3* BioXASMAXvMotor::createMoveToLimitAction(CLSMAXvMotor::Limit setpoint)
{
	return new BioXASMAXvMotorMoveToLimitAction(new BioXASMAXvMotorMoveToLimitActionInfo(toInfo(), setpoint), this);
}

void BioXASMAXvMotor::setLimitSetpoint(CLSMAXvMotor::Limit limit, double setpoint)
{
	limitSetpointMap_.insert(limit, setpoint);
	emit limitSetpointMappingChanged();
}

void BioXASMAXvMotor::removeLimitSetpoint(CLSMAXvMotor::Limit limit)
{
	limitSetpointMap_.remove(limit);
	emit limitSetpointMappingChanged();
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

	if (!hasLimitSetpoint(limit)) {
		AMErrorMon::alert(this, CLSMAXVMOTOR_INVALID_LIMIT_SETPOINT, QString("Failed to move '%1' to the '%2' limit. There is no setpoint for the given limit.").arg(name()).arg(limitToString(limit)));
		return AMControl::OtherFailure;
	}

	return move(limitSetpointMap_.value(limit));
}
