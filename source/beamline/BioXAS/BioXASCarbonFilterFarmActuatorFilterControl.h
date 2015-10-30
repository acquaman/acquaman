#ifndef BIOXASCARBONFILTERFARMACTUATORFILTERCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORFILTERCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorWindowControl.h"

class BioXASCarbonFilterFarmActuatorFilterControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorFilterControl(const QString &name, const QString &units, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorFilterControl();

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

	/// Returns true if the given value corresponds to a valid window, false otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value corresponds to a valid window setpoint, false otherwise.
	virtual bool validSetpoint(double value) const { return validValue(value); }

	/// Returns the window control.
	AMControl* window() { return window_; }

	/// Returns a string representation of the given filter thickness.
	static QString filterThicknessToString(double filterThickness);

	/// Returns the filter thickness at the given window.
	double filterThicknessAtWindow(double window);
	/// Returns a list of windows with the given filter thickness.
	QList<double> windowsWithFilterThickness(double filterThickness);

signals:
	/// Notifier that the window control has changed.
	void windowChanged(AMControl *newControl);

public slots:
	/// Sets the actuator window control.
	void setWindow(BioXASCarbonFilterFarmActuatorWindowControl *newControl);
	/// Sets a window to filter thickness mapping.
	void setWindowFilter(BioXASCarbonFilterFarmActuatorWindowControl::Value window, double filterThickness);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the moving state.
	virtual void updateMoving();

protected:
	/// Returns a new action that moves the actuator to the desired window setpoint.
	virtual AMAction3* createMoveAction(double windowSetpoint);

	/// Sets up the enum states for this control.
	void setupEnumStates();

protected:
	/// The mapping between window enum and filter thickness.
	QMap<double, double> windowFilterMap_;
	/// The window control.
	BioXASCarbonFilterFarmActuatorWindowControl *window_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORFILTERCONTROL_H
