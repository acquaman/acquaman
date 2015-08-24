#ifndef BIOXASCARBONFILTERFARMACTUATORCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORCONTROL_H

#include <QSignalMapper>

#include "beamline/AMPVControl.h"
#include "beamline/AMPseudoMotorControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

class BioXASCarbonFilterFarmActuatorControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorControl(const QString &name, const QString &units, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorControl();

	/// Returns the position control.
	AMControl* positionControl() const { return position_; }
	/// Returns the position status control.
	AMControl* statusControl() const { return status_; }

signals:
	/// Notifier that the position control has changed.
	void positionControlChanged(AMControl *newControl);
	/// Notifier that the status control has changed.
	void statusControlChanged(AMControl *newControl);

public slots:
	/// Sets the position control.
	void setPositionControl(AMControl *newControl);
	/// Sets the status control.
	void setStatusControl(AMControl *newControl);

protected:
	/// The position control.
	AMControl *position_;
	/// The position status control.
	AMControl *status_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORCONTROL_H
