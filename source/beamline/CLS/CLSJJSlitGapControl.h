#ifndef CLSJJSLITGAPCONTROL_H
#define CLSJJSLITGAPCONTROL_H

#include <QSignalMapper>

#include "beamline/AMCompositeControl.h"

// error codes.
#define CLSJJSLITGAPCONTROL_NOT_CONNECTED 23487200
#define CLSJJSLITGAPCONTROL_ALREADY_MOVING 23487201
#define CLSJJSLITGAPCONTROL_CANNOT_MOVE 23487202
#define CLSJJSLITGAPCONTROL_INVALID_ACTION 23487203
#define CLSJJSLITGAPCONTROL_CANNOT_STOP 23487204

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
	/// Returns the center position.
	double centerPosition() const { return centerPosition_; }
	/// Returns true if the gap is changing, as a result of this control's action.
	virtual bool moveInProgress() const { return moveInProgress_; }

	/// Returns true if the first and second motor controls are valid and connected, false otherwise.
	virtual bool isConnected() const;
	/// Returns true if the gap is always measurable, provided the controls are connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a gap change is always possible, provided the controls are connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a gap change in progress, provided the controls are connected. False otherwise.
	virtual bool shouldStop() const { return false; }
	/// Returns true if this control can move right now.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now.
	virtual bool canStop() const;

	/// Returns the minimum value this control can take.
	virtual double minimumValue() const;
	/// Returns the maximum value this control can take.
	virtual double maximumValue() const;

	/// Returns the upper blade motor.
	AMControl* upperBladeControl() const { return upperBladeControl_; }
	/// Returns the lower blade motor.
	AMControl* lowerBladeControl() const { return lowerBladeControl_; }

signals:
	/// Notifier that the center position has changed.
	void centerPositionChanged(double newValue);
	/// Notifier that the upper blade motor control has changed.
	void upperBladeControlChanged(AMControl *newControl);
	/// Notifier that the lower blade motor control has changed.
	void lowerBladeControlChanged(AMControl *newControl);

public slots:
	/// Sets the new gap setpoint and performs a move, if necessary.
	virtual FailureExplanation move(double setpoint);
	/// Stops a move in progress.
	virtual bool stop();

	/// Sets the center position value.
	void setCenterPosition(double newValue);

	/// Sets the upper blade motor control.
	void setUpperBladeControl(AMControl *newControl);
	/// Sets the lower blade motor control.
	void setLowerBladeControl(AMControl *newControl);

protected slots:
	/// Updates the current gap value and emits the valueChanged() signal.
	void setValue(double newValue);
	/// Updates the current gap setpoint and emits the setpointChanged() signal.
	void setSetpoint(double newValue);
	/// Updates the move in progress status and emits the moveChanged() signal. The moveStarted() signal is emitted if a move is started.
	void setMoveInProgress(bool isMoving);

	/// Handles updating the saved gap value, according to the current motor control values.
	void updateValue();

	/// Handles the situation where the move action is started.
	void onMoveStarted();
	/// Handles the situation where the move action is cancelled.
	void onMoveCancelled(QObject *action);
	/// Handles the situation where the move action failed.
	void onMoveFailed(QObject *action);
	/// Handles the situation where the move action succeeded.
	void onMoveSucceeded(QObject *action);

	/// Handles cleaning up after completed move action.
	void moveCleanup(QObject *action);

protected:
	/// Calculate the basic position offset for a motor from the given gap and center position.
	static double calculatePositionOffset(double gap, double center);
	/// Calculate the gap from the given motor positions.
	static double calculateGap(double upperBladePosition, double lowerBladePosition);

protected:
	/// The current gap value.
	double value_;
	/// The current gap setpoint.
	double setpoint_;
	/// The current center position, about which the gap is centered.
	double centerPosition_;
	/// Indicates whether the control is moving as a result of this control's action.
	bool moveInProgress_;

	/// The upper blade motor control.
	AMControl *upperBladeControl_;
	/// The lower blade motor control.
	AMControl *lowerBladeControl_;

	/// Signal mapper for the move action cancelled.
	QSignalMapper *moveCancelled_;
	/// Signal mapper for the move action failed.
	QSignalMapper *moveFailed_;
	/// Signal mapper for the move action succeeded.
	QSignalMapper *moveSucceeded_;

};

#endif // CLSJJSLITGAPCONTROL_H
