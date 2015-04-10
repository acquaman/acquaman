#ifndef BIOXASXIAFILTERS_H
#define BIOXASXIAFILTERS_H

#include <QObject>
#include <QSignalMapper>

#include "beamline/AMCompositeControl.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "actions3/AMAction3.h"

class BioXASXIAFilters : public AMCompositeControl
{
    Q_OBJECT

public:
	/// Enum defining the possible positions for each filter.
	class Filter { public:
				   enum Name { First = 0, Second, Third, Fourth };
				   enum Position { Out = 0, In };
				 };

	/// Constructor.
    explicit BioXASXIAFilters(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASXIAFilters();

	/// Returns the current value.
	virtual double value() const { return value_; }
	/// Returns the current setpoint.
	virtual double setpoint() const { return setpoint_; }
	/// Returns true if there is a filter change in progress, as a result of this control's action.
	virtual bool moveInProgress() const { return moveInProgress_; }
	/// Returns the smallest value this control can assume.
	virtual double minimumValue() const { return 0; }
	/// Returns the largest value this control can assume.
	virtual double maximumValue() const { return 0; }
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

signals:

public slots:
	/// Sets the new filter setpoint and changes filters.
	virtual FailureExplanation move(double setpoint);

protected slots:
	/// Updates the current value and emits the valueChanged() signal.
	void setValue(double newValue);
	/// Updates the current value setpoint and emits the setpointChanged() signal.
	void setSetpoint(double newValue);
	/// Updates the current moveInProgress state and emits the moveChanged() signal. The moveStarted() signal is also emitted if the argument is true.
	void setMoveInProgress(bool isMoving);

	/// Sets the position of the given filter control.
	void setFilterPosition(Filter::Name filter, Filter::Position position);

protected:
	/// Returns a new action that moves the current filter to the desired setpoint.
	AMAction3* createMoveAction();

protected:
	/// The current value.
	double value_;
	/// The current value setpoint.
	double setpoint_;
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

};

#endif // BIOXASXIAFILTERS_H
