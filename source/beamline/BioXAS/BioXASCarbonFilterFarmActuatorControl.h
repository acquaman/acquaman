#ifndef BIOXASCARBONFILTERFARMACTUATORCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORCONTROL_H

#include <QSignalMapper>

#include "beamline/AMPVControl.h"
#include "beamline/AMCompositeControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

// timeout.

#define TIMEOUT_MOVE 10

// error codes.

#define BIOXAS_FILTER_FARM_ACTUATOR_NOT_CONNECTED 773981
#define BIOXAS_FILTER_FARM_ACTUATOR_ALREADY_MOVING 773982
#define BIOXAS_FILTER_FARM_ACTUATOR_INVALID_SETPOINT 773983
#define BIOXAS_FILTER_FARM_ACTUATOR_MOVE_FAILED 773984

class BioXASCarbonFilterFarmActuatorControl : public AMCompositeControl
{
    Q_OBJECT

public:
	/// Enum defining the active window options.
	class Window { public: enum Selection { None = 0, Removed, First, Second }; };
	/// Enum defining the possible position states.
	class Position { public: enum State { NotSafe = 0, Safe }; };

	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorControl(QString &name, QObject *parent);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorControl();

	/// Returns the current active window.
	virtual double value() const { return value_; }
	/// Returns the desired active window setpoint.
	virtual double setpoint() const { return setpoint_; }
	/// Returns true if the arm is moving, as a result of this control's action.
	virtual bool moveInProgress() const { return moveInProgress_; }
	/// Returns the smallest value this control can take.
	virtual double minimumValue() const { return Window::None; }
	/// Returns the largest value this control can take.
	virtual double maximumValue() const { return Window::Second; }
	/// Returns true if the active window is always measurable, if connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move to a new active window is always possible, if connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a move in progress, if connected. False otherwise.
	virtual bool shouldStop() const { return true; }
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

	/// Returns the position control.
	AMPVControl* positionControl() const { return position_; }
	/// Returns the position status control.
	AMReadOnlyPVControl* statusControl() const { return status_; }

	/// Returns true if the given value corresponds to a valid window, false otherwise.
	static bool validWindow(double value);
	/// Returns true if the given value corresponds to a valid window setpoint, false otherwise.
	static bool validWindowSetpoint(double value);
	/// Returns a string representation of the given window value.
	static QString windowToString(Window::Selection value);
	/// Returns the window corresponding to the given double setpoint.
	static Window::Selection window(double value);

	/// Returns the window corresponding to the given position.
	Window::Selection windowAtPosition(double position) const;
	/// Returns the position of the given window.
	double positionOfWindow(Window::Selection window) const;

public slots:
	/// Sets the new window setpoint and moves the actuator.
	virtual FailureExplanation move(double setpoint);

	/// Sets a window to position mapping.
	void setWindowPosition(Window::Selection window, double position);

protected slots:
	/// Sets the active window.
	void setWindow(double newWindow);
	/// Sets the desired window setpoint.
	void setWindowSetpoint(double newWindow);
	/// Sets the move in progress.
	void setMoveInProgress(bool isMoving);

	/// Handles emitting the appropriate signals and performing action cleanup when the move action is cancelled.
	void onMoveCancelled(QObject *action);
	/// Handles emitting the appropriate signals and performing action cleanup when the move action fails.
	void onMoveFailed(QObject *action);
	/// Handles emitting the appropriate signals and performing action cleanup when the move action succeeds.
	void onMoveSucceeded(QObject *action);

	/// Updates the active window, based in the current position and the available mappings.
	void updateWindow();

protected:
	/// Returns a new action that moves the actuator to the desired window setpoint. Returns 0 if unable to move or if the setpoint is invalid.
	AMAction3* createMoveAction(double setpoint);

protected:
	/// The active window.
	double value_;
	/// The active window setpoint.
	double setpoint_;
	/// The current move state, true if this control has initiated a move.
	bool moveInProgress_;
	/// The mapping between window and position.
	QMap<Window::Selection, double> positionMap_;
	/// The position control.
	AMPVControl *position_;
	/// The position status control.
	AMReadOnlyPVControl *status_;

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

#endif // BIOXASCARBONFILTERFARMACTUATORCONTROL_H
