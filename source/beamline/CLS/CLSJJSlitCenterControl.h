#ifndef CLSJJSLITCENTERCONTROL_H
#define CLSJJSLITCENTERCONTROL_H

#include <QSignalMapper>

#include "beamline/AMCompositeControl.h"

// error codes.
#define CLSJJSLITCENTERCONTROL_NOT_CONNECTED 23487300
#define CLSJJSLITCENTERCONTROL_ALREADY_MOVING 23487301
#define CLSJJSLITCENTERCONTROL_CANNOT_MOVE 23487302
#define CLSJJSLITCENTERCONTROL_INVALID_ACTION 23487303
#define CLSJJSLITCENTERCONTROL_CANNOT_STOP 23487304

class CLSJJSlitCenterControl : public AMCompositeControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit CLSJJSlitCenterControl(AMControl *upperBladeControl, AMControl *lowerBladeControl, QObject *parent = 0, double tolerance = 0.005);
	/// Destructor.
	virtual ~CLSJJSlitCenterControl();

	/// Returns the current center position value.
	virtual double value() const { return value_; }
	/// Returns the current center position setpoint.
	virtual double setpoint() const { return setpoint_; }
	/// Returns the current gap value.
	double gap() const { return gap_; }
	/// Returns true if the center position is changing, as a result of this control's action.
	virtual bool moveInProgress() const { return moveInProgress_; }

	/// Returns true if the first and second motor controls are valid and connected, false otherwise.
	virtual bool isConnected() const;
	/// Returns true if the center position is always measurable, provided the controls are connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a center position change is always possible, provided the controls are connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a center position change in progress, provided the controls are connected. False otherwise.
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
	/// Notifier that the gap value has changed.
	void gapChanged(double newValue);
	/// Notifier that the upper blade motor control has changed.
	void upperBladeControlChanged(AMControl *newControl);
	/// Notifier that the lower blade motor control has changed.
	void lowerBladeControlChanged(AMControl *newControl);

public slots:
	/// Sets the new center position setpoint and performs a move, if necessary.
	virtual FailureExplanation move(double setpoint);
	/// Stops a move in progress.
	virtual bool stop();

	/// Sets the gap value.
	void setGap(double newValue);

	/// Sets the upper blade motor control.
	void setUpperBladeControl(AMControl *newControl);
	/// Sets the lower blade motor control.
	void setLowerBladeControl(AMControl *newControl);

protected slots:
	/// Updates the current center position value and emits the valueChanged() signal.
	void setValue(double newValue);
	/// Updates the current center position setpoint and emits the setpointChanged() signal.
	void setSetpoint(double newValue);
	/// Updates the move in progress status and emits the moveChanged() signal. The moveStarted() signal is emitted if a move is started.
	void setMoveInProgress(bool isMoving);

	/// Handles updating the saved center position value, according to the current motor control values.
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
	/// Calculates the basic position offset for a motor from the given gap and center position.
	static double calculatePositionOffset(double gap, double center);
	/// Calculates the center position, from the given motor positions.
	static double calculateCenterPosition(double upperBladePosition, double lowerBladePosition);

protected:
	/// The current center position value.
	double value_;
	/// The current center position setpoint.
	double setpoint_;
	/// The current gap value.
	double gap_;
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

#endif // CLSJJSLITCENTERCONTROL_H
