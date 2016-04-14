#ifndef BIOXASBRAGGMAXVMOTORCONTROL_H
#define BIOXASBRAGGMAXVMOTORCONTROL_H

#include "beamline/CLS/CLSMAXvMotor.h"

class BioXASBraggMaxVMotorControl : public CLSMAXvMotor
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASBraggMaxVMotorControl(const QString &name, const QString &baseName, const QString &description, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0, QString pvUnitFieldName=":mm");
	/// Destructor.
	virtual ~BioXASBraggMaxVMotorControl();

//	virtual bool isMoving() const { return CLSMAXvMotor::isMoving() && !moveValueUpdateReceived_; }

public slots:
	/// Initiates a motor move to the given setpoint. Reimplemented to initialize whether we have received a value update after the move has started.
	virtual AMControl::FailureExplanation move(double setpoint);

protected slots:
	/// Handles checking whether the new feedback value is within tolerance for moves. Reimplemeted to additionally update moveValueUpdateReceived_.
	virtual void onValueChanged(double value);
	/// Handles updating the move failed/succeeded states. Reimplemented to include additional check that the motor has received at least one value update.
	virtual void onMovingChanged(int isMovingValue);

protected:
	/// Flag for whether a value update has been received since a move began.
	bool moveValueUpdateReceived_;
};

#endif // BIOXASBRAGGMAXVMOTORCONTROL_H
