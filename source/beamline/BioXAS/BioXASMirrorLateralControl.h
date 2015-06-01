#ifndef BIOXASMIRRORLATERALCONTROL_H
#define BIOXASMIRRORLATERALCONTROL_H

#include "beamline/BioXAS/BioXASMirrorControl.h"

class BioXASMirrorLateralControl : public BioXASMirrorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorLateralControl(const QString &name, const QString &units, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASMirrorLateralControl();

	/// Returns true if this control's value can be measured right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop right now. False otherwise.
	virtual bool canStop() const;

protected slots:
	/// Updates the current connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the current moving state.
	virtual void updateMoving();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Calculates and returns the yaw motor position for the given lateral control value, mirror lengths, and lateral motor position.
	virtual double calculateYawPosition(double lateralDisplacement, double upstreamLength, double downstreamLength, double lateralPosition);

};

#endif // BIOXASMIRRORLATERALCONTROL_H
