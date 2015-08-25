#ifndef BIOXASCARBONFILTERFARMACTUATORFILTERTHICKNESSCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORFILTERTHICKNESSCONTROL_H

#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorWindowControl.h"

class BioXASCarbonFilterFarmActuatorFilterThicknessControl : public BioXASCarbonFilterFarmActuatorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorFilterThicknessControl(const QString &name, const QString &units, AMControl *positionControl, AMControl *statusControl, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorFilterThicknessControl();

	/// Returns true if the given value corresponds to a valid window, false otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value corresponds to a valid window setpoint, false otherwise.
	virtual bool validSetpoint(double value) const { return validValue(value); }

	/// Returns a string representation of the given filter thickness.
	static QString filterThicknessToString(double filterThickness);

	/// Returns the filter thickness at the given window.
	double filterThicknessAtWindow(BioXASCarbonFilterFarm::Actuator::Window window);
	/// Returns a list of windows with the given filter thickness.
	QList<BioXASCarbonFilterFarm::Actuator::Window> windowsWithFilterThickness(double filterThickness);

public slots:
	/// Sets the position control. Reimplemented so that the local windowControl is updated too.
	virtual void setPositionControl(AMControl *newControl);
	/// Sets the position status control. Reimplemented so that the local windowControl is updated too.
	virtual void setStatusControl(AMControl *newControl);
	/// Sets a window to filter thickness mapping.
	void setWindowFilterThickness(BioXASCarbonFilterFarm::Actuator::Window window, double filterThickness);

protected slots:
	/// Updates the 'is moving' state.
	virtual void updateMoving();

protected:
	/// Returns a new action that moves the actuator to the desired window setpoint.
	virtual AMAction3* createMoveAction(double windowSetpoint);

protected:
	/// The mapping between window enum and filter thickness.
	QMap<BioXASCarbonFilterFarm::Actuator::Window, double> windowFilterMap_;
	/// The window control.
	BioXASCarbonFilterFarmActuatorWindowControl *windowControl_;

};

#endif // BIOXASCARBONFILTERFARMACTUATORFILTERTHICKNESSCONTROL_H
