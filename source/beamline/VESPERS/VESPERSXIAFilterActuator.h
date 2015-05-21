#ifndef VESPERSXIAFILTERACTUATOR_H
#define VESPERSXIAFILTERACTUATOR_H

#include "beamline/AMPseudoMotorControl.h"
#include "beamline/AMPVControl.h"

/// This is an actuator that handles changing the state of a particular filter actuator.
class VESPERSXIAFilterActuator : public AMPseudoMotorControl
{
	Q_OBJECT

public:
	/// State enum.
	enum State { Out = 0, In, Invalid };

	/// Constructor.  Builds an XIA filter actuator from valid toggle and state controls.
	VESPERSXIAFilterActuator(const QString &name, AMControl *toggleControl, AMControl *stateControl, QObject *parent = 0);
	/// Destructor.
	virtual ~VESPERSXIAFilterActuator();

	/// Returns true if the control value is always measureable, provided it is connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a control move is always possible, provided it is connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if a control stop is always possible, provided it is connected. False otherwise.
	virtual bool shouldStop() const { return false; }

	/// Returns true if this control's value can be measured right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop right now. False otherwise.
	virtual bool canStop() const;

	/// Returns the toggle control.
	AMControl *toggleControl() const { return toggle_; }
	/// Returns the state control.
	AMControl *stateControl() const { return state_; }

	/// Returns true if the given value corresponds to a valid filter state.
	virtual bool validValue(double value) const;
	/// Returns true if the given value corresponds to a valid filter setpoint.
	virtual bool validSetpoint(double value) const;

public slots:
	/// Sets the toggle control.
	void setToggleControl(AMControl *newControl);
	/// Sets the status control.
	void setStatusControl(AMControl *newControl);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the 'is moving' state.
	virtual void updateIsMoving();

protected:
	/// Returns a new action that will move the actuator to the desired state.
	virtual AMAction3 *createMoveAction(double setpoint);

	/// This control handles toggling the actuator.
	AMControl *toggle_;
	/// This control handles reading the actuator state.
	AMControl *state_;
};

#endif // VESPERSXIAFILTERACTUATOR_H
