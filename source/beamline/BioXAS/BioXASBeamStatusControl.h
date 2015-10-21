#ifndef BIOXASBEAMSTATUSCONTROL_H
#define BIOXASBEAMSTATUSCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
#include "application/BioXAS/BioXAS.h"

class BioXASBeamStatusControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamStatusControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatusControl();

	/// Returns true if this control is always measurable, provided it is connected.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move is always possible, provided this control is connected.
	virtual bool shouldMove() const { return true; }

	/// Returns true if the given value is a valid value for this control, false otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid setpoint for this control, false otherwise.
	virtual bool validSetpoint(double value) const;

	/// Returns true if the front end beam is on, false otherwise.
	virtual bool isOn() const { return false; }
	/// Returns false if the front and beam is off, false otherwise.
	virtual bool isOff() const { return false; }

	/// Returns a string representation of the given value.
	QString valueToString(BioXAS::Beam::Status value) const;

protected:
	/// Returns a move action that moves this control to the given setpoint.
	AMAction3* createMoveAction(double setpoint);

	/// Returns a new action that changes the control to 'beam on.'
	virtual AMAction3* createBeamOnAction() = 0;
	/// Returns a new action that changes the control to 'beam off.'
	virtual AMAction3* createBeamOffAction() = 0;
};

#endif // BIOXASBEAMSTATUSCONTROL_H
