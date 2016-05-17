#ifndef BIOXASMAXVMOTOR_H
#define BIOXASMAXVMOTOR_H

#include "beamline/CLS/CLSMAXvMotor.h"

class BioXASMAXvMotor : public CLSMAXvMotor
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASMAXvMotor(const QString &name, const QString &baseName, const QString &description, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0, QString pvUnitFieldName = ":mm");
	/// Destructor.
	virtual ~BioXASMAXvMotor();

	/// Returns a new action that moves the motor to the given limit.
	virtual AMAction3* createMoveToLimitAction(CLSMAXvMotor::Limit setpoint);

public slots:
	/// Moves the control to the given setpoint. Reimplemented to check whether the control is at the setpoint before checking the limits status.
	virtual AMControl::FailureExplanation move(double setpoint);
	/// Moves the control to the given limit.
	virtual AMControl::FailureExplanation moveToLimit(CLSMAXvMotor::Limit limit);
};

#endif // BIOXASMAXVMOTOR_H
