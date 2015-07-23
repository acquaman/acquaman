#ifndef BIOXASXIAFILTERSCONTROL_H
#define BIOXASXIAFILTERSCONTROL_H

#include "beamline/CLS/CLSXIAFiltersControl.h"

class BioXASSideXIAFiltersControl : public CLSXIAFiltersControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideXIAFiltersControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideXIAFiltersControl();

	/// Returns true if this control is always measurable, provided the control is connected.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move is always possible, provided control is connected.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a move in progress, provided it is connected.
	virtual bool shouldStop() const { return true; }

	/// Returns true if this control can take a measurement right now.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now.
	virtual bool canStop() const;

	/// Returns true if the given value is a valid value for this control, false otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid setpoint for this control, false otherwise.
	virtual bool validSetpoint(double value) const;

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();

protected:
	/// Returns a new action that moves this control.
	AMAction3* createMoveAction(double setpoint);
};

#endif // BIOXASXIAFILTERSCONTROL_H
