#ifndef BIOXASCARBONFILTERFARMACTUATORPOSITIONCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORPOSITIONCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"

class BioXASCarbonFilterFarmActuatorPositionControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Enum describing the actuator position status options.
	enum Status { NotInPosition = 0, InPosition };

	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorPositionControl(const QString &name, const QString &units, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorPositionControl();

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

	/// Returns the status value.
	double statusValue() const;

	/// Returns the motor.
	CLSMAXvMotor* motor() const { return motor_; }
	/// Returns the position status control.
	AMControl* status() const { return status_; }

signals:
	/// Notifier that the motor control has changed.
	void motorChanged(CLSMAXvMotor *newControl);
	/// Notifier that the status control has changed.
	void statusChanged(AMControl *newControl);
	/// Notifier that the status control value has changed.
	void statusValueChanged(double newValue);

public slots:
	/// Sets the motor control.
	void setMotor(CLSMAXvMotor *newControl);
	/// Sets the status control.
	void setStatus(AMControl *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the moving state.
	virtual void updateMoving();

protected slots:
	/// Handles emitting the status value changed signal.
	void onStatusControlValueChanged();

protected:
	/// Returns a new action that moves the actuator to the desired position setpoint.
	virtual AMAction3* createMoveAction(double windowSetpoint);

protected:
	/// The motor control.
	CLSMAXvMotor *motor_;
	/// The position status control.
	AMControl *status_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORPOSITIONCONTROL_H
