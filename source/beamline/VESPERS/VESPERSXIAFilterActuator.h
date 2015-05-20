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
	enum State { Invalid = 0, Out, In };

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
	/// This control handles toggling the actuator.
	AMControl *toggle_;
	/// This control handles reading the actuator state.
	AMControl *state_;
};

#endif // VESPERSXIAFILTERACTUATOR_H
