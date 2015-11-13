#ifndef BIOXASXIAFILTERSACTUATORCONTROL_H
#define BIOXASXIAFILTERSACTUATORCONTROL_H

#include "beamline/AMEnumeratedControl.h"
#include "beamline/AMPVControl.h"

class BioXASXIAFiltersActuatorControl : public AMEnumeratedControl
{
    Q_OBJECT

public:
	/// Enum describing the different actuator states.
	enum State { In = 1, Out = 2 };

	/// Constructor.
	explicit BioXASXIAFiltersActuatorControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASXIAFiltersActuatorControl();

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

	/// Returns the actuator status control.
	AMReadOnlyPVControl* actuatorStatus() const { return status_; }
	/// Returns the actuator 'in' control.
	AMPVControl* actuatorIn() const { return oprIn_; }
	/// Returns the actuator 'out' control.
	AMPVControl* actuatorOut() const { return oprOut_; }

	/// Returns a string representation of the given state value.
	static QString stateToString(double state);

signals:
	/// Notifier that the actuator status control has changed.
	void statusChanged(AMReadOnlyPVControl *newControl);
	/// Notifier that the actuator 'in' control has changed.
	void inChanged(AMPVControl *newControl);
	/// Notifier that the actuator 'out' control has changed.
	void outChanged(AMPVControl *newControl);

public slots:
	/// Sets the actuator status control.
	void setStatus(AMReadOnlyPVControl *newControl);
	/// Sets the actuator 'in' control.
	void setIn(AMPVControl *newControl);
	/// Sets the actuator 'out' control.
	void setOut(AMPVControl *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the moving state.
	virtual void updateMoving();

protected:
	/// Returns a new action that moves the control to the setpoint that corresponds to the given index setpoint.
	virtual AMAction3* createMoveAction(double indexSetpoint);

	/// Returns the current index.
	virtual int currentIndex() const;

protected:
	/// The actuator status feedback control.
	AMReadOnlyPVControl *status_;
	/// The control that moves to the In state.
	AMPVControl *oprIn_;
	/// The control that moves to the Out state.
	AMPVControl *oprOut_;
};

#endif // BIOXASXIAFILTERSACTUATORCONTROL_H
