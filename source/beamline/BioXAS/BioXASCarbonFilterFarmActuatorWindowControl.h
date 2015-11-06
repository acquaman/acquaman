#ifndef BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H

#include "beamline/AMEnumeratedControl.h"

/*
  This is a control that's meant to control a carbon filter farm actuator by window,
  through mapping the actuator's positions to enumerated window values.
  */
class BioXASCarbonFilterFarmActuatorWindowControl : public AMEnumeratedControl
{
	Q_OBJECT

public:
	/// Enumerates the different actuator windows.
	enum Windows { None = 0, Bottom = 1, Top = 2 };

	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorWindowControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorWindowControl();
};

#endif // BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H
