#ifndef BIOXASMIRRORROLLCONTROL_H
#define BIOXASMIRRORROLLCONTROL_H

#include "beamline/BioXAS/BioXASMirrorControl.h"

class BioXASMirrorRollControl : public BioXASMirrorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorRollControl(const QString &name, const QString &units, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASMirrorRollControl();

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
	/// Updates the current connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the current moving state.
	virtual void updateMoving();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);
};

#endif // BIOXASMIRRORROLLCONTROL_H
