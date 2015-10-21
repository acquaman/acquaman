#ifndef BIOXASVALVESCONTROL_H
#define BIOXASVALVESCONTROL_H

#include "application/BioXAS/BioXAS.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMPseudoMotorControl.h"
#include "beamline/CLS/CLSBiStateControl.h"

class BioXASValvesControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Enum providing the values this control can have.
	enum Value { Open = 0, Closed = 1, None = 2 };

	/// Constructor.
	explicit BioXASValvesControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASValvesControl();

	/// Returns true if the value is always measurable, provided the control is connected.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move is always possible, provided control is connected.
	virtual bool shouldMove() const { return true; }

	/// Returns true if the given value is a valid value for this control. False otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid setpoint for this control. False otherwise.
	virtual bool validSetpoint(double value) const;

	/// Returns true if this control's value is Open, false otherwise.
	virtual bool isOpen() const { return false; }
	/// Returns true if this control's value is Closed, false otherwise.
	virtual bool isClosed() const { return false; }

	/// Returns a string representation of the given value.
	static QString valueToString(BioXASValvesControl::Value value);

protected slots:
	/// Updates the current value.
	virtual void updateValue();

protected:
	/// Creates and returns a move action to the given setpoint.
	virtual AMAction3* createMoveAction(double setpoint);
	/// Creates and returns an action that opens the valves.
	virtual AMAction3* createOpenValvesAction() = 0;
};

#endif // BIOXASVALVESCONTROL_H
