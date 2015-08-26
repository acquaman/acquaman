#ifndef BIOXASCARBONFILTERFARMACTUATORCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORCONTROL_H

#include "beamline/BioXAS/BioXASCarbonFilterFarmControl.h"

#define TIMEOUT_MOVE 10

class BioXASCarbonFilterFarmActuatorControl : public BioXASCarbonFilterFarmControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorControl(const QString &name, const QString &units, AMControl *positionControl, AMControl *statusControl, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorControl();

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

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

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the moving state.
	virtual void updateMoving();

protected:
	/// Returns a new action that moves the actuator to the desired window setpoint. Returns 0 if unable to move or if the setpoint is invalid.
	virtual AMAction3* createMoveAction(double windowSetpoint);

protected:
	/// The position control.
	AMControl *position_;
	/// The position status control.
	AMControl *status_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORCONTROL_H
