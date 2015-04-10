#ifndef BIOXASCARBONFILTERFARM_H
#define BIOXASCARBONFILTERFARM_H

#include <QObject>

#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h"

// error codes.

#define BIOXAS_FILTER_FARM_NOT_CONNECTED 23048701
#define BIOXAS_FILTER_FARM_ALREADY_MOVING 23048702
#define BIOXAS_FILTER_FARM_INVALID_SETPOINT 23048703
#define BIOXAS_FILTER_FARM_MOVE_FAILED 23048704


class BioXASCarbonFilterFarmControl : public AMCompositeControl
{
    Q_OBJECT

public:
	/// Enum defining all possible filter thickness combinations.
	class Filter { public: enum Thickness { Invalid = 0, None, Fifty, SeventyFive, FiveHundred, FiveHundredSeventyFive, SevenHundred, SevenHundredFifty }; };
	/// Enum defining the different actuators.
	class Actuator { public: enum Position { Upstream = 0, Downstream }; };

	/// Constructor.
	explicit BioXASCarbonFilterFarmControl(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmControl();

	/// Returns the current total filter thickness.
	virtual double value() const { return value_; }
	/// Returns the desired total filter thickness setpoint.
	virtual double setpoint() const { return setpoint_; }
	/// Returns true if the filter farm is moving, as a result of this control's action.
	virtual bool moveInProgress() const { return moveInProgress_; }
	/// Returns the smallest value this control can take.
	virtual double minimumValue() const { return Filter::Invalid; }
	/// Returns the largest value this control can take.
	virtual double maximumValue() const { return Filter::SevenHundredFifty; }
	/// Returns true if the total filter thickness is always measurable, if connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move to a new total filter thickness is always possible, if connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a move in progress, if connected. False otherwise.
	virtual bool shouldStop() const { return true; }
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const { return isConnected(); }
	/// Returns true if this control is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the upstream filter arm control.
	BioXASCarbonFilterFarmActuatorControl* upstreamActuatorControl() const { return upstreamActuator_; }
	/// Returns the downstream filter arm control.
	BioXASCarbonFilterFarmActuatorControl* downstreamActuatorControl() const { return downstreamActuator_; }

	/// Returns true if the given value corresponds to a valid filter, false otherwise. This will be different depending on the beamline.
	virtual bool validFilter(double value) = 0;
	/// Returns true if the given value corresponds to a valid filter setpoint, false otherwise. This will be different depending on the beamline.
	virtual bool validFilterSetpoint(double value) = 0;

	/// Returns a string representation of the given filter thickness.
	static QString filterToString(Filter::Thickness value);
	/// Returns a filter corresponding to the given string representation. Returns Filter::Invalid if no filter match found.
	static Filter::Thickness stringToFilter(const QString &string);
	/// Returns a double representation of the given filter thickness.
	static double filterToDouble(Filter::Thickness value);
	/// Returns the filter corresponding to the given double respresentation.
	static Filter::Thickness doubleToFilter(double filterThickness);

	/// Returns the filter corresponding to the given enum index.
	static Filter::Thickness filter(double index);

	/// Returns the filter corresponding to the given actuator and window. Returns Filter::Invalid if filter not found.
	Filter::Thickness filterAtWindow(Actuator::Position actuator, BioXASCarbonFilterFarmActuatorControl::Window::Selection window);

public slots:
	/// Sets the new filter thickness setpoint and moves the actuators.
	virtual FailureExplanation move(double setpoint);

protected slots:
	/// Sets the filter thickness.
	void setFilter(double newValue);
	/// Sets the filter thickness setpoint.
	void setFilterSetpoint(double newSetpoint);
	/// Sets a window to filter thickness mapping.
	void setWindowFilter(Actuator::Position actuator, BioXASCarbonFilterFarmActuatorControl::Window::Selection window, Filter::Thickness filterThickness);
	/// Sets the move in progress.
	void setMoveInProgress(bool isMoving);

	/// Handles emitting the appropriate signals when the move action has started.
	void onMoveStarted();
	/// Handles emitting the appropriate signals and performing action cleanup when the move action is cancelled.
	void onMoveCancelled(QObject *action);
	/// Handles emitting the appropriate signals and performing action cleanup when the move action fails.
	void onMoveFailed(QObject *action);
	/// Handles emitting the appropriate signals and performing action cleanup when the move action succeeds.
	void onMoveSucceeded(QObject *action);

	/// Updates the total filter thickness, according to the active windows of each actuator.
	void updateFilter();

protected:
	/// Returns a new action that moves the actuators to the desired filter thickness setpoint.
	virtual AMAction3* createMoveAction(double setpoint) = 0;

	/// Returns the total filter thickness, according to the given active actuator windows.
	Filter::Thickness calculateTotalFilterFromWindows(BioXASCarbonFilterFarmActuatorControl::Window::Selection upstreamWindow, BioXASCarbonFilterFarmActuatorControl::Window::Selection downstreamWindow);
	/// Returns the total filter thickness, according to the given active upstream and downstream filters.
	static Filter::Thickness calculateTotalFilter(Filter::Thickness upstreamFilter, Filter::Thickness downstreamFilter);

protected:
	/// The current total filter thickness.
	double value_;
	/// The current total filter thickness setpoint.
	double setpoint_;
	/// The current move state, true if this control has initiated a move.
	bool moveInProgress_;
	/// The mapping between window and filter thickness, for the upstream actuator.
	QMap<BioXASCarbonFilterFarmActuatorControl::Window::Selection, Filter::Thickness> upstreamFilterMap_;
	/// The mapping between window and filter thickness, for the downstream actuator.
	QMap<BioXASCarbonFilterFarmActuatorControl::Window::Selection, Filter::Thickness> downstreamFilterMap_;
	/// The upstream filter actuator.
	BioXASCarbonFilterFarmActuatorControl *upstreamActuator_;
	/// The downstream filter actuator.
	BioXASCarbonFilterFarmActuatorControl *downstreamActuator_;

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

#endif // BIOXASCARBONFILTERFARM_H
