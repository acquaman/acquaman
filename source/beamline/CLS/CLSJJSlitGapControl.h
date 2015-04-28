#ifndef CLSJJSLITGAPCONTROL_H
#define CLSJJSLITGAPCONTROL_H

#include "beamline/AMCompositeControl.h"

class CLSMAXvMotor;

class CLSJJSlitGapControl : public AMCompositeControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSJJSlitGapControl(AMControl *firstMotorControl, AMControl *secondMotorControl, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSJJSlitGapControl();

	/// Returns the current gap value.
	virtual double value() const { return value_; }
	/// Returns the current gap setpoint.
	virtual double setpoint() const { return setpoint_; }
	/// Returns true if the gap is moving, as a result of this control's action.
	virtual bool moveInProgress() const { return moveInProgress_; }

	/// Returns true if the region is always measurable (when the control is connected), false otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move is always possible, provided the control is connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a move in progress, provided it is connected. False otherwise.
	virtual bool shouldStop() const { return false; }
	/// Returns true if this control can move right now.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now.
	virtual bool canStop() const;

	/// Returns the first blade motor.
	AMControl* firstMotorControl() const { return firstMotor_; }
	/// Returns the second blade motor.
	AMControl* secondMotorControl() const { return secondMotor_; }

signals:
	/// Notifier that the first motor control has changed.
	void firstMotorChanged(AMControl *newControl);
	/// Notifier that the second motor control has changed.
	void secondMotorChanged(AMControl *newControl);

public slots:
	/// Sets the new gap setpoint and performs a move, if necessary.
	virtual FailureExplanation move(double setpoint);
	/// Stops a move in progress.
	virtual bool stop();

	/// Sets the first motor control.
	void setFirstMotorControl(AMControl *firstControl);
	/// Sets the second motor control.
	void setSecondMotorControl(AMControl *secondControl);

protected slots:
	/// Updates the current gap value and emits the valueChanged() signal.
	void setValue(double newValue);
	/// Updates the current gap setpoint and emits the setpointChanged() signal.
	void setSetpoint(double newValue);
	/// Updates the move in progress status and emits the moveChanged() signal. The moveStarted() signal is emitted if a move is started.
	void setMoveInProgress(bool isMoving);

protected:
	/// The current gap value.
	double value_;
	/// The current gap setpoint.
	double setpoint_;
	/// Indicates whether the control is moving as a result of this control's action.
	bool moveInProgress_;

	/// The first blade motor.
	AMControl *firstMotor_;
	/// The second blade motor.
	AMControl *secondMotor_;

};

#endif // CLSJJSLITGAPCONTROL_H
