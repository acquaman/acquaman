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
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorWindowControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorWindowControl();

	/// Returns the position control.
	BioXASCarbonFilterFarmActuatorPositionControl* positionControl() const { return qobject_cast<BioXASCarbonFilterFarmActuatorPositionControl*>(control_); }

signals:
	/// Notifier that the position control has changed.
	void positionChanged(BioXASCarbonFilterFarmActuatorPositionControl *newControl);
	/// Notifier that the window options have changed.
	void windowsChanged();

public slots:
	/// Sets the position control.
	void setPositionControl(BioXASCarbonFilterFarmActuatorPositionControl *newControl);

	/// Adds a window option.
	void addWindow(int windowIndex, const QString &windowString, double positionSetpoint, double positionMin, double positionMax);
	/// Removes a window option.
	void removeWindow(int windowIndex);
	/// Clears all window options.
	void clearWindows();
};

#endif // BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H
