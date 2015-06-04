#ifndef BIOXASMIRRORYAWCONTROL_H
#define BIOXASMIRRORYAWCONTROL_H

#include "beamline/BioXAS/BioXASMirrorControl.h"

class BioXASMirrorYawControl : public BioXASMirrorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorYawControl(const QString &name, const QString &units, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASMirrorYawControl();

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
};

#endif // BIOXASMIRRORYAWCONTROL_H
