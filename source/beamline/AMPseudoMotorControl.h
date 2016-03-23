#ifndef AMPSEUDOMOTORCONTROL_H
#define AMPSEUDOMOTORCONTROL_H

#include <QSignalMapper>

#include "source/beamline/AMConnectedControl.h"
#include "source/actions3/AMAction3.h"

#define AMPSEUDOMOTORCONTROL_INVALID_VALUE 89327420
#define AMPSEUDOMOTORCONTROL_INVALID_SETPOINT 89327421
#define AMPSEUDOMOTORCONTROL_NOT_CONNECTED 89327422
#define AMPSEUDOMOTORCONTROL_CANNOT_MOVE 89327423
#define AMPSEUDOMOTORCONTROL_ALREADY_MOVING 89327424
#define AMPSEUDOMOTORCONTROL_INVALID_MOVE_ACTION 89327425
#define AMPSEUDOMOTORCONTROL_MOVE_FAILED 89327426
#define AMPSEUDOMOTORCONTROL_CANNOT_CALIBRATE 89327427
#define AMPSEUDOMOTORCONTROL_ALREADY_CALIBRATING 89327428
#define AMPSEUDOMOTORCONTROL_INVALID_CALIBRATION_ACTION 89327429
#define AMPSEUDOMOTORCONTROL_CALIBRATION_FAILED 89327430

class AMPseudoMotorControl : public AMConnectedControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMPseudoMotorControl(const QString &name, const QString &units, QObject *parent = 0, const QString &description = "");
	/// Destructor.
	virtual ~AMPseudoMotorControl();

	/// Returns true if this control should be able to be stopped (if connected), false otherwise.
	virtual bool shouldStop() const { return true; }
	/// Returns true if this control can be stopped right now, false otherwise. Finds this out be examining all child controls. Subclasses can reimplement to achieve their particular behavior.
	virtual bool canStop() const;

	/// Returns the current value.
	virtual double value() const { return value_; }
	/// Returns the current setpoint.
	virtual double setpoint() const { return setpoint_; }
	/// Returns the smallest value this control can take.
	virtual double minimumValue() const { return minimumValue_; }
	/// Returns the largest value this control can take.
	virtual double maximumValue() const { return maximumValue_; }

	/// Returns true if the control is moving.
	virtual bool isMoving() const { return isMoving_; }
	/// Returns true if the control is moving, as a result of this control's action.
	virtual bool moveInProgress() const { return moveInProgress_; }
	/// Returns true if this control is calibrating, as a result of this control's action.
	virtual bool calibrationInProgress() const { return calibrationInProgress_; }

	/// Returns true if the given value is a valid value for this control. False otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid setpoint for this control. False otherwise.
	virtual bool validSetpoint(double value) const;

	/// Adds a given control to the list of child controls.
	virtual void addChildControl(AMControl *control);
	/// Removes a given control from the list of child controls.
	virtual void removeChildControl(AMControl *control);

	/// Returns a string representation of this control.
	virtual QString toString() const;

signals:

public slots:
	/// Sets the setpoint and moves the control, if necessary.
	virtual FailureExplanation move(double setpoint);
	/// Stops the control, by stopping all children.
	virtual bool stop();
	/// Calibrates the control such that the old value becomes the new value. Fails if calibration has not been implemented for this control.
	virtual FailureExplanation calibrate(double oldValue, double newValue);

protected slots:
	/// Sets the enum states.
	void setEnumStates(const QStringList &enumStateNames);
	/// Sets the value.
	void setValue(double newValue);
	/// Sets the setpoint.
	void setSetpoint(double newValue);
	/// Sets the 'move in progress' state.
	void setMoveInProgress(bool isMoving);
	/// Sets the 'is moving' state.
	void setIsMoving(bool isMoving);
	/// Sets the minimum value.
	void setMinimumValue(double newValue);
	/// Sets the maximum value.
	void setMaximumValue(double newValue);
	/// Sets the 'calibration in progress' state.
	void setCalibrationInProgress(bool isCalibrating);

	/// Updates states.
	virtual void updateStates();
	/// Updates the current value.
	virtual void updateValue() = 0;
	/// Updates the moving state.
	virtual void updateMoving() { return; }
	/// Updates the minimum value.
	virtual void updateMinimumValue() { return; }
	/// Updates the maximum value.
	virtual void updateMaximumValue() { return; }
	/// Updates the units.
	virtual void updateUnits() { return; }
	/// Updates the tolerance.
	virtual void updateTolerance() { return; }

	/// Handles emitting the appropriate signals when a move action has started.
	virtual void onMoveStarted(QObject *action);
	/// Handles emitting the appropriate signals and performing action cleanup when a move action is cancelled.
	virtual void onMoveCancelled(QObject *action);
	/// Handles emitting the appropriate signals and performing action cleanup when a move action fails.
	virtual void onMoveFailed(QObject *action);
	/// Handles emitting the appropriate signals and performing action cleanup when a move action succeeds.
	virtual void onMoveSucceeded(QObject *action);

	/// Handles situation where the calibration has started.
	virtual void onCalibrationStarted();
	/// Handles situation where the calibration is cancelled.
	virtual void onCalibrationCancelled(QObject *action);
	virtual void onCalibrationCancelled();
	/// Handles situation where the calibration fails.
	virtual void onCalibrationFailed(QObject *action);
	virtual void onCalibrationFailed();
	/// Handles situation where the calibration succeeds.
	virtual void onCalibrationSucceeded(QObject *action);
	virtual void onCalibrationSucceeded();

protected:
	/// Creates and returns a move action. Subclasses are required to reimplement.
	virtual AMAction3* createMoveAction(double setpoint) = 0;

	/// Creates and returns a calibration action. Subclasses can optionally reimplement.
	virtual AMAction3* createCalibrateAction(double oldValue, double newValue);

	/// Handles disconnecting from a move action and removing the signal mappings when the action is complete.
	void moveActionCleanup(QObject *action);
	/// Handles cleaning up a calibration action, once the action is no longer needed.
	void calibrationActionCleanup(QObject *action);
	/// Handles disconnecting from and deleting an action after it has been executed.
	void actionCleanup(QObject *action);

protected:
	/// The current value.
	double value_;
	/// The current setpoint.
	double setpoint_;
	/// The flag indicating whether the control is moving as a result of the move() slot.
	bool moveInProgress_;
	/// The flag indicating whether the control is moving.
	bool isMoving_;
	/// The minimum value this control can take.
	double minimumValue_;
	/// The maximum value this control can take.
	double maximumValue_;
	/// The flag indicating whether the control is calibrating as a result of the calibrate() slot.
	bool calibrationInProgress_;

	/// The signal mapper for move started.
	QSignalMapper *startedMapper_;
	/// The signal mapper for move cancelled.
	QSignalMapper *cancelledMapper_;
	/// The signal mapper for move failed.
	QSignalMapper *failedMapper_;
	/// The signal mapper for move succeeded.
	QSignalMapper *succeededMapper_;

	/// The calibration started signal mapper.
	QSignalMapper *calibrationStartedMapper_;
	/// The calibration cancelled signal mapper.
	QSignalMapper *calibrationCancelledMapper_;
	/// The calibration failed signal mapper.
	QSignalMapper *calibrationFailedMapper_;
	/// The calibration succeeded signal mapper.
	QSignalMapper *calibrationSucceededMapper_;
};

#endif // AMPSEUDOMOTORCONTROL_H
