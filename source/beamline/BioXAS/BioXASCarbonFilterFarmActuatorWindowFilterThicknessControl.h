#ifndef BIOXASCARBONFILTERFARMACTUATORWINDOWFILTERTHICKNESSCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORWINDOWFILTERTHICKNESSCONTROL_H

#include "beamline/BioXAS/BioXASCarbonFilterFarmControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorWindowControl.h"

class BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl : public BioXASCarbonFilterFarmControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl(const QString &name, const QString &units, BioXASCarbonFilterFarmActuatorWindowControl *windowControl,  QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl();

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
	AMControl* windowControl() { return window_; }

	/// Returns a string representation of the given filter thickness.
	static QString filterThicknessToString(double filterThickness);

	/// Returns the filter thickness at the given window.
	double filterThicknessAtWindow(BioXASCarbonFilterFarm::Actuator::Window window);
	/// Returns a list of windows with the given filter thickness.
	QList<BioXASCarbonFilterFarm::Actuator::Window> windowsWithFilterThickness(double filterThickness);

signals:
	/// Notifier that the window control has changed.
	void windowControlChanged(AMControl *newControl);

public slots:
	/// Sets the actuator window control.
	void setWindowControl(BioXASCarbonFilterFarmActuatorWindowControl *newControl);
	/// Sets a window to filter thickness mapping.
	void setWindowFilterThickness(BioXASCarbonFilterFarm::Actuator::Window window, double filterThickness);

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

	/// Generates this control's enum states from the filter thickness map.
	void setupEnumStates();

protected:
	/// The mapping between window enum and filter thickness.
	QMap<BioXASCarbonFilterFarm::Actuator::Window, double> windowFilterMap_;
	/// The window control.
	BioXASCarbonFilterFarmActuatorWindowControl *window_;

};

#endif // BIOXASCARBONFILTERFARMACTUATORWINDOWFILTERTHICKNESSCONTROL_H
