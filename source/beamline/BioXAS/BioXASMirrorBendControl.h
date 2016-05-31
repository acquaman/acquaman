#ifndef BIOXASMIRRORBENDCONTROL_H
#define BIOXASMIRRORBENDCONTROL_H

#include "beamline/BioXAS/BioXASMirrorControl.h"

class BioXASMirrorBendControl : public BioXASMirrorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorBendControl(const QString &name, const QString &units, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASMirrorBendControl();

	/// Returns true if the control value is always measureable, provided it is connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a control move is always possible, provided it is connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if a control stop is always possible, provided it is connected. False otherwise.
	virtual bool shouldStop() const { return true; }

	/// Returns true if this control's value can be measured right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop right now. False otherwise.
	virtual bool canStop() const;

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the moving state.
	virtual void updateMoving();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Calculates and returns the mirror bend radius, for the given bender values.
	virtual double calculateBendRadius(double upstreamBenderValue, double downstreamBenderValue) const;

	/// Calculates and returns the mirror bend radius for the upstream bender value.
	virtual double calculateUpstreamBendRadius(double upstreamForce) const = 0;
	/// Calculates and returns the mirror bend radius for the downstream bender value.
	virtual double calculateDownstreamBendRadius(double downstreamForce) const = 0;

	/// Calculates and returns the upstream bender value, for the given bend radius.
	virtual double calculateUpstreamBenderValue(double bendRadius) const = 0;
	/// Calculates and returns the downstream bender value, for the given bend radius.
	virtual double calculateDownstreamBenderValue(double bendRadius) const = 0;
};

#endif // BIOXASMIRRORBENDCONTROL_H
