#ifndef BIOXASBEAMSTATUSCONTROL_H
#define BIOXASBEAMSTATUSCONTROL_H

#include "beamline/AMPseudoMotorControl.h"

class BioXASBeamStatusControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Enum providing the different possible value options.
	enum Value { On = 0, Off = 1, None = 2 };

	/// Constructor.
	explicit BioXASBeamStatusControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatusControl();

	/// Returns true if the given value is a valid value for this control, false otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid setpoint for this control, false otherwise.
	virtual bool validSetpoint(double value) const;

	/// Returns true if beam is on, false otherwise.
	virtual bool isOn() const { return false; }
	/// Returns false if beam is off, false otherwise.
	virtual bool isOff() const { return false; }

protected slots:
	/// Updates the current value.
	virtual void updateValue();

protected:
	/// Returns a new action that changes the beam state.
	virtual AMAction3* createMoveAction(double newState);

	/// Returns a new action that changes the control to 'beam on.'
	virtual AMAction3* createBeamOnAction() { return 0; }
	/// Returns a new action that changes the control to 'beam off.'
	virtual AMAction3* createBeamOffAction() { return 0; }
};

#endif // BIOXASBEAMSTATUSCONTROL_H
