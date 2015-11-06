#ifndef BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H

#include "beamline/AMSingleEnumeratedControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorPositionControl.h"

/*
  This is a control that's meant to control a carbon filter farm actuator by window,
  through mapping the actuator's positions to enumerated window values.
  */
class BioXASCarbonFilterFarmActuatorWindowControl : public AMSingleEnumeratedControl
{
	Q_OBJECT

public:
	/// Enumerates the different actuator window options.
	enum Option { None = 0, Bottom = 1, Top = 2 };

	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorWindowControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorWindowControl();

	/// Returns a string representation of the given window option.
	static QString windowToString(double window);

	/// Returns the position control.
	BioXASCarbonFilterFarmActuatorPositionControl* positionControl() const { return qobject_cast<BioXASCarbonFilterFarmActuatorPositionControl*>(control_); }

public slots:
	/// Sets the position control.
	void setPositionControl(BioXASCarbonFilterFarmActuatorPositionControl *newControl);

	/// Adds a window option.
	void addWindow(Option window, double windowSetpoint, double windowMin, double windowMax);
};

#endif // BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H
