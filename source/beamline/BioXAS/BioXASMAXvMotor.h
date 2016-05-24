#ifndef BIOXASMAXVMOTOR_H
#define BIOXASMAXVMOTOR_H

#include "beamline/CLS/CLSMAXvMotor.h"

#define BIOXASMAXVMOTOR_INVALID_LIMIT_SETPOINT 3230238


class BioXASMAXvMotor : public CLSMAXvMotor
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASMAXvMotor(const QString &name, const QString &baseName, const QString &description, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0, QString pvUnitFieldName = ":mm");
	/// Destructor.
	virtual ~BioXASMAXvMotor();

	/// Returns true if the given limit has a setpoint.
	bool hasLimitSetpoint(CLSMAXvMotor::Limit limit) const;
	/// Returns the setpoint for the given limit, returns 0 if no setpoint found.
	double limitSetpoint(CLSMAXvMotor::Limit limit) const;

	/// Returns a new action that moves the motor to the given limit.
	virtual AMAction3* createMoveToLimitAction(CLSMAXvMotor::Limit setpoint);

signals:
	/// Notifier that a limit setpoint mapping has changed.
	void limitSetpointMappingChanged();

public slots:
	/// Sets the setpoint for a given limit.
	void setLimitSetpoint(CLSMAXvMotor::Limit limit, double setpoint);
	/// Removes the setpoint for the given limit.
	void removeLimitSetpoint(CLSMAXvMotor::Limit limit);

	/// Moves the control to the given limit.
	virtual AMControl::FailureExplanation moveToLimit(CLSMAXvMotor::Limit limit);

protected:
	/// The limit setpoint mapping.
	QMap<CLSMAXvMotor::Limit, double> limitSetpointMap_;
};

#endif // BIOXASMAXVMOTOR_H
