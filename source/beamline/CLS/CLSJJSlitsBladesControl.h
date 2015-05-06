#ifndef CLSJJSLITSBLADESCONTROL_H
#define CLSJJSLITSBLADESCONTROL_H

#include <QSignalMapper>

#include "beamline/AMCompositeControl.h"

// Error codes.
#define CLSJJSLITSCONTROL_NOT_CONNECTED 23487300
#define CLSJJSLITSCONTROL_ALREADY_MOVING 23487301
#define CLSJJSLITSCONTROL_CANNOT_MOVE 23487302
#define CLSJJSLITSCONTROL_INVALID_ACTION 23487303
#define CLSJJSLITSCONTROL_CANNOT_STOP 23487304

class CLSJJSlitsBladesControl : public AMCompositeControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSJJSlitsBladesControl(const QString &name, AMControl *firstMotorControl, AMControl *secondMotorControl, QObject *parent = 0, const QString &units = "", const QString &description = "", double tolerance = 0.005);
	/// Destructor.
	virtual ~CLSJJSlitsBladesControl();

	/// Returns the current value.
	virtual double value() const { return value_; }
	/// Returns the current setpoint.
	virtual double setpoint() const { return setpoint_; }
	/// Returns the gap.
	double gap() const { return gap_; }
	/// Returns the center position.
	double centerPosition() const { return centerPosition_; }
	/// Returns true if the gap is changing, as a result of this control's action.
	virtual bool moveInProgress() const { return moveInProgress_; }

	/// Returns true if the child controls are valid and connected, false otherwise.
	virtual bool isConnected() const { return connected_; }
	/// Returns true if the gap is always measurable, provided the controls are connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a gap change is always possible, provided the controls are connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a gap change in progress, provided the controls are connected. False otherwise.
	virtual bool shouldStop() const { return true; }
	/// Returns true if this control can move right now.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now.
	virtual bool canStop() const;

	/// Returns the minimum value this control can take.
	virtual double minimumValue() const { return -30; }
	/// Returns the maximum value this control can take.
	virtual double maximumValue() const { return 30; }

	/// Returns the upper blade motor.
	AMControl* upperBladeControl() const { return upperBladeControl_; }
	/// Returns the lower blade motor.
	AMControl* lowerBladeControl() const { return lowerBladeControl_; }

signals:
	/// Notifier that the gap has changed.
	void gapChanged(double newValue);
	/// Notifier that the center position has changed.
	void centerPositionChanged(double newValue);
	/// Notifier that the upper blade motor control has changed.
	void upperBladeControlChanged(AMControl *newControl);
	/// Notifier that the lower blade motor control has changed.
	void lowerBladeControlChanged(AMControl *newControl);

public slots:
	/// Sets the new setpoint and performs a move, if necessary.
	virtual FailureExplanation move(double setpoint);
	/// Stops a move in progress.
	virtual bool stop();

	/// Sets the upper blade motor control.
	void setUpperBladeControl(AMControl *newControl);
	/// Sets the lower blade motor control.
	void setLowerBladeControl(AMControl *newControl);

protected slots:
	/// Sets the current connected state and emits the connectedChanged() signal.
	void setConnected(bool isConnected);
	/// Sets the current gap value and emits the valueChanged() signal.
	void setValue(double newValue);
	/// Sets the current gap setpoint and emits the setpointChanged() signal.
	void setSetpoint(double newValue);
	/// Sets the move in progress status and emits the moveChanged() signal. The moveStarted() signal is emitted if a move is started.
	void setMoveInProgress(bool isMoving);

	/// Sets the gap value.
	void setGap(double newValue);
	/// Sets the center position value.
	void setCenterPosition(double newValue);

	/// Handles updating the control's states: connected, value, gap, center position.
	void updateControlStates();

	/// Handles updating the control's connected state, called when a child control's connected state changes.
	void updateConnected();
	/// Handles updating the control's value, called when a child control's value changes. Subclasses must implement this.
	virtual void updateValue() { return; }
	/// Handles updating the saved gap value, according to the current motor control values.
	void updateGap();
	/// Handles updating the saved center position value, according to the current motor control values.
	void updateCenterPosition();

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
	/// Creates and returns a move action. Returns 0 if not connected. Subclasses must implement this.
	virtual AMAction3* createMoveAction(double setpoint) = 0;

	/// Calculate the upper blade motor control's position from the given gap and center position.
	static double calculateUpperPosition(double gap, double centerPosition);
	/// Calculate the lower blade motor control's position from the given gap and center position.
	static double calculateLowerPosition(double gap, double centerPosition);
	/// Calculate the gap from the given motor positions.
	static double calculateGap(double upperBladePosition, double lowerBladePosition);
	/// Calculate the center position from the given motor positions.
	static double calculateCenterPosition(double upperBladePosition, double lowerBladePosition);

protected:
	/// The current connected state.
	bool connected_;
	/// The current value.
	double value_;
	/// The current setpoint.
	double setpoint_;
	/// The current gap value, centered about the center position.
	double gap_;
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

#endif // CLSJJSLITSBLADESCONTROL_H
