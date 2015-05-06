#ifndef AMPSEUDOMOTORCONTROL_H
#define AMPSEUDOMOTORCONTROL_H

#include <QSignalMapper>

#include "beamline/AMControl.h"
#include "actions3/AMAction3.h"

// Error codes.

#define AMPSEUDOMOTORCONTROL_NOT_CONNECTED 9823700
#define AMPSEUDOMOTORCONTROL_ALREADY_MOVING 9823701
#define AMPSEUDOMOTORCONTROL_INVALID_SETPOINT 9823702
#define AMPSEUDOMOTORCONTROL_INVALID_MOVE_ACTION 9823703
#define AMPSEUDOMOTORCONTROL_MOVE_FAILED 9823704

class AMPseudoMotorControl : public AMControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMPseudoMotorControl(const QString &name, const QString &units, QObject *parent = 0, const QString &description = "");
	/// Destructor.
	virtual ~AMPseudoMotorControl();

	/// Returns true if this control is connected, false otherwise.
	virtual bool isConnected() const { return connected_; }
	/// Returns the current value.
	virtual double value() const { return value_; }
	/// Returns the current setpoint.
	virtual double setpoint() const { return setpoint_; }
	/// Returns true if the control is moving, as a result of this control's action.
	virtual bool moveInProgress() const { return moveInProgress_; }
	/// Returns true if the control is moving.
	virtual bool isMoving() const { return isMoving_; }
//	/// Returns the smallest value this control can take.
//	virtual double minimumValue() const { return minimumValue_; }
//	/// Returns the largest value this control can take.
//	virtual double maximumValue() const { return maximumValue_; }

//	/// Returns true if the control value is always measureable, provided it is connected. False otherwise.
//	virtual bool shouldMeasure() { return false; }
//	/// Returns true if a control move is always possible, provided it is connected. False otherwise.
//	virtual bool shouldMove() const { return false; }
//	/// Returns true if a control stop is always possible, provided it is connected. False otherwise.
//	virtual bool shouldStop() const { return false; }

//	/// Returns true if this control's value can be measured right now. False otherwise.
//	virtual bool canMeasure() const { return false; }
//	/// Returns true if this control can move right now. False otherwise.
//	virtual bool canMove() const { return false; }
//	/// Returns true if this control can stop right now. False otherwise.
//	virtual bool canStop() const { return false; }

	/// Returns true if the given value is a valid value for this control. False otherwise.
	virtual bool validValue(double value) = 0;
	/// Returns true if the given value is a valid setpoint for this control. False otherwise.
	virtual bool validSetpoint(double value) = 0;

signals:
	/// Notifier that the minimum value has changed.
	void minimumValueChanged(double newValue);
	/// Notifier that the maximum value has changed.
	void maximumValueChanged(double newValue);

public slots:
	/// Sets the setpoint and moves the control, if necessary.
	virtual FailureExplanation move(double setpoint);

protected slots:
	/// Sets the connected state.
	void setConnected(bool isConnected);
	/// Sets the value.
	void setValue(double newValue);
	/// Sets the setpoint.
	void setSetpoint(double newValue);
	/// Sets the 'move in progress' state.
	void setMoveInProgress(bool isMoving);
	/// Sets the 'is moving' state.
	void setIsMoving(bool isMoving);

	/// Updates the connected state.
	virtual void updateConnected() = 0;
	/// Updates the current value.
	virtual void updateValue() = 0;
	/// Updates the 'is moving' state.
	virtual void updateIsMoving() = 0;

	/// Adds a given control to the list of child controls.
	virtual void addChildControl(AMControl *control);
	/// Removes a given control from the list of child controls.
	virtual void removeChildControl(AMControl *control);

	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint) = 0;

	/// Handles emitting the appropriate signals when a move action has started.
	void onMoveStarted();
	/// Handles emitting the appropriate signals and performing action cleanup when a move action is cancelled.
	void onMoveCancelled(QObject *action);
	/// Handles emitting the appropriate signals and performing action cleanup when a move action fails.
	void onMoveFailed(QObject *action);
	/// Handles emitting the appropriate signals and performing action cleanup when a move action succeeds.
	void onMoveSucceeded(QObject *action);

protected:
	/// The flag indicating whether this control is connected.
	bool connected_;
	/// The current value.
	double value_;
	/// The current setpoint.
	double setpoint_;
	/// The flag indicating whether the control is moving as a result of the move() slot.
	bool moveInProgress_;
	/// The flag indicating whether the control is moving.
	bool isMoving_;

	/// The signal mapper for move cancelled.
	QSignalMapper *cancelledMapper_;
	/// The signal mapper for move failed.
	QSignalMapper *failedMapper_;
	/// The signal mapper for move succeeded.
	QSignalMapper *succeededMapper_;

private:
	/// Handles disconnecting from a move action and removing the signal mappings when the action is complete.
	void moveCleanup(QObject *action);

};

#endif // AMPSEUDOMOTORCONTROL_H
