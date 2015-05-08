#ifndef BIOXASCARBONFILTERFARMCONTROL_H
#define BIOXASCARBONFILTERFARMCONTROL_H

#include "beamline/AMControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h"

// error codes.

#define BIOXAS_FILTER_FARM_NOT_CONNECTED 23048701
#define BIOXAS_FILTER_FARM_ALREADY_MOVING 23048702
#define BIOXAS_FILTER_FARM_INVALID_SETPOINT 23048703
#define BIOXAS_FILTER_FARM_MOVE_FAILED 23048704


class BioXASCarbonFilterFarmControl : public AMPseudoMotorControl
{
	Q_OBJECT

public:
	/// Enum defining all possible filter thickness combinations.
	class Filter { public: enum Thickness { Invalid = 0, None, Fifty, SeventyFive, FiveHundred, FiveHundredSeventyFive, SevenHundred, SevenHundredFifty }; };
	/// Enum defining the different actuators.
	class Actuator { public: enum Position { Upstream = 0, Downstream }; };

	/// Constructor.
	explicit BioXASCarbonFilterFarmControl(BioXASCarbonFilterFarmActuatorControl *upstreamActuatorControl, BioXASCarbonFilterFarmActuatorControl *downstreamActuatorControl, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmControl();

	/// Returns true if the total filter thickness is always measurable, if connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move to a new total filter thickness is always possible, if connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a move in progress, if connected. False otherwise.
	virtual bool shouldStop() const { return true; }

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

	/// Returns the upstream filter arm control.
	AMControl* upstreamActuatorControl() const { return upstreamActuator_; }
	/// Returns the downstream filter arm control.
	AMControl* downstreamActuatorControl() const { return downstreamActuator_; }

	/// Returns true if the given value corresponds to a valid filter, false otherwise. This will be different depending on the beamline.
	virtual bool validValue(double value) const;
	/// Returns true if the given value corresponds to a valid filter setpoint, false otherwise. This will be different depending on the beamline.
	virtual bool validSetpoint(double value) const;

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
	/// Sets the upstream actuator control.
	void setUpstreamActuatorControl(BioXASCarbonFilterFarmActuatorControl *newControl);
	/// Sets the downstream actuator control.
	void setDownstreamActuatorControl(BioXASCarbonFilterFarmActuatorControl *newControl);

protected slots:
	/// Sets a window to filter thickness mapping.
	void setWindowFilter(Actuator::Position actuator, BioXASCarbonFilterFarmActuatorControl::Window::Selection window, Filter::Thickness filterThickness);

	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the 'is moving' state.
	virtual void updateIsMoving();

protected:
	/// Returns a new action that moves the actuators to the desired filter thickness setpoint.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Returns the total filter thickness, according to the given active actuator windows.
	Filter::Thickness calculateTotalFilterFromWindows(BioXASCarbonFilterFarmActuatorControl::Window::Selection upstreamWindow, BioXASCarbonFilterFarmActuatorControl::Window::Selection downstreamWindow);
	/// Returns the total filter thickness, according to the given active upstream and downstream filters.
	static Filter::Thickness calculateTotalFilter(Filter::Thickness upstreamFilter, Filter::Thickness downstreamFilter);

protected:
	/// The mapping between window and filter thickness, for the upstream actuator.
	QMap<BioXASCarbonFilterFarmActuatorControl::Window::Selection, Filter::Thickness> upstreamFilterMap_;
	/// The mapping between window and filter thickness, for the downstream actuator.
	QMap<BioXASCarbonFilterFarmActuatorControl::Window::Selection, Filter::Thickness> downstreamFilterMap_;

	/// The upstream filter actuator.
	BioXASCarbonFilterFarmActuatorControl *upstreamActuator_;
	/// The downstream filter actuator.
	BioXASCarbonFilterFarmActuatorControl *downstreamActuator_;
};

#endif // BIOXASCARBONFILTERFARMCONTROL_H
