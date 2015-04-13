#ifndef BIOXASXIAFILTERS_H
#define BIOXASXIAFILTERS_H

#include <QObject>
#include <QSignalMapper>

#include "beamline/AMCompositeControl.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "actions3/AMAction3.h"

// Setpoints.
#define BIOXAS_XIA_FILTERS_POSITION_OUT -50
#define BIOXAS_XIA_FILTERS_POSITION_IN 50
#define BIOXAS_XIA_FILTERS_POSITION_TOLERANCE 0.1

// Error codes.
#define BIOXAS_XIA_FILTERS_NOT_CONNECTED 29348721
#define BIOXAS_XIA_FILTERS_ALREADY_MOVING 29348722
#define BIOXAS_XIA_FILTERS_INVALID_SETPOINT 29348723
#define BIOXAS_XIA_FILTERS_MOVE_FAILED 29348724

class BioXASXIAFilters : public AMCompositeControl
{
    Q_OBJECT

public:
	/// Enum defining the possible names and positions for each filter, and options for the total active filter thickness.
	class Filter { public:
				   enum Name { First = 0, Second, Third, Fourth };
				   enum Position { Out = 0, In, Other };
				   enum Thickness { Invalid = 0, None, One, Two, Three, Four, Five, Six };
				 };

	/// Constructor.
    explicit BioXASXIAFilters(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASXIAFilters();

	/// Returns the current value.
	virtual double value() const { return (double)value_; }
	/// Returns the current setpoint.
	virtual double setpoint() const { return (double)setpoint_; }
	/// Returns true if there is a filter change in progress, as a result of this control's action.
	virtual bool moveInProgress() const { return moveInProgress_; }
	/// Returns the smallest value this control can assume.
	virtual double minimumValue() const { return Filter::Invalid; }
	/// Returns the largest value this control can assume.
	virtual double maximumValue() const { return Filter::Six; }
	/// Returns true if the region is always measurable (when the control is connected). False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move to a new filter is always possible, provided control is connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a filter change in progress, provided it is connected. False otherwise.
	virtual bool shouldStop() const { return false; }
	/// Returns true if this control can move right now, false otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a change to a new region right now, false otherwise.
	virtual bool canStop() const { return false; }

	/// Returns true if this control is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the first filter control.
	AMControl* filter1() const { return filter1_; }
	/// Returns the second filter control.
	AMControl* filter2() const { return filter2_; }
	/// Returns the third filter control.
	AMControl* filter3() const { return filter3_; }
	/// Returns the fourth filter control.
	AMControl* filter4() const { return filter4_; }

	/// Returns the filter corresponding to the given filter name. Returns 0 if there is no matching control.
	AMControl* filterControl(Filter::Name name) const;

	/// Returns the position of the filter with the given filter name.
	Filter::Position filterPosition(Filter::Name name) const;

	/// Returns the filter thickness corresponding to the given double value. Returns Filter::Invalid if there is no matching thickness.
	static Filter::Thickness filterThickness(double value);

	/// Returns true if the given value corresponds to a valid filter thickness, false otherwise.
	bool validFilter(double value) const;
	/// Returns true if the given value corresponds to a valid filter thickness setpoint, false otherwise.
	bool validFilterSetpoint(double value) const;

public slots:
	/// Sets the new filter setpoint and changes filters.
	virtual FailureExplanation move(double setpoint);

protected slots:
	/// Updates the current value and emits the valueChanged() signal.
	void setValue(Filter::Thickness newValue);
	/// Updates the current value setpoint and emits the setpointChanged() signal.
	void setSetpoint(Filter::Thickness newValue);
	/// Updates the current moveInProgress state and emits the moveChanged() signal. The moveStarted() signal is also emitted if the argument is true.
	void setMoveInProgress(bool isMoving);

	/// Updates the value according to the current filter control positions.
	void updateValue();

	/// Sets the position of the named filter. Returns true if the position change happened successfully, false otherwise.
	bool setFilterPosition(Filter::Name filter, Filter::Position position);
	/// Sets the position of the given filter control. Returns true if the position change happened successfully, false otherwise.
	bool setFilterControlPosition(AMControl *filterControl, Filter::Position position);
	/// Sets the position of the given filter control to Filter::In. Returns true if the position change happened successfully, false otherwise.
	bool setFilterControlIn(AMControl *filterControl);
	/// Sets the position of the given filter control to Filter::Out. Returns true if the position change happened successfully, false otherwise.
	bool setFilterControlOut(AMControl *filterControl);

	/// Handles emitting the appropriate signals when a move has been started.
	void onMoveStarted();
	/// Handles cleaning up the move action and emitting the appropriate signals when a move has been cancelled.
	void onMoveCancelled(QObject *action);
	/// Handles cleaning up the move action and emitting the appropriate signals when a move has failed.
	void onMoveFailed(QObject *action);
	/// Handles cleaning up the move action and emitting the appropriate signals when a move has succeeded.
	void onMoveSucceeded(QObject *action);

protected:
	/// Returns a new action that moves the current filter to the desired setpoint.
	AMAction3* createMoveAction(Filter::Thickness setpoint);

	/// Returns a newly created action that moves the named filter to the given position.
	AMAction3* createSetFilterPositionAction(Filter::Name name, Filter::Position position);
	/// Returns a newly created action that moves the filter control to the position. Returns 0 if either the given control or the desired position are invalid.
	AMAction3* createSetFilterControlPositionAction(AMControl *filterControl, Filter::Position position);
	/// Returns a newly created action that moves the filter control to Filter::In. Returns 0 if the given control is invalid.
	AMAction3* createSetFilterControlIn(AMControl *filterControl);
	/// Returns a newly created action that moves the filter control to Filter::Out. Returns 0 if the given control is invalid.
	AMAction3* createSetFilterControlOut(AMControl *filterControl);

	/// Returns true if the given filter motor control is at the given filter position, false otherwise.
	bool filterControlInPosition(AMControl *filterControl, Filter::Position position) const;
	/// Returns true if the given filter motor control is in the Filter::In position, false otherwise.
	bool filterControlIn(AMControl *filterControl) const;
	/// Returns true if the given filter motor control is in the Filter::Out position, false otherwise.
	bool filterControlOut(AMControl *filterControl) const;
	/// Returns true if the given filter motor control is in the Filter::Other position, false otherwise.
	bool filterControlOther(AMControl *filterControl) const;

protected:
	/// The current value.
	Filter::Thickness value_;
	/// The current value setpoint.
	Filter::Thickness setpoint_;
	/// The current move state, true if this control has initiated a move.
	bool moveInProgress_;

	/// Controls for the first filter.
	CLSBiStateControl *filter1_;
	/// Control for the second filter.
	CLSBiStateControl *filter2_;
	/// Control for the third filter.
	CLSBiStateControl *filter3_;
	/// Control for the fourth filter.
	CLSBiStateControl *filter4_;

	/// Signal mapper for the move action cancelled.
	QSignalMapper *moveCancelledMapper_;
	/// Signal mapper for the move action failed.
	QSignalMapper *moveFailedMapper_;
	/// Signal mapper for the move action succeeded.
	QSignalMapper *moveSucceededMapper_;

private:
	/// Disconnects the given action, removes its mappings, updates the control's 'moveInProgress' state, and frees memory.
	void moveCleanup(QObject *action);

};

#endif // BIOXASXIAFILTERS_H
