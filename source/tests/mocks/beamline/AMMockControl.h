#ifndef AMMOCKCONTROL_H
#define AMMOCKCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
#include "actions3/AMAction3.h"

class AMMockControl : public AMPseudoMotorControl
{
    Q_OBJECT
public:
    explicit AMMockControl(const QString& name,
	                       const QString& units,
	                       QObject *parent = 0);

	/// Returns true if this control's value can be measured right now. False otherwise.
	virtual bool canMeasure() const { return true; }
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const { return true; }
	/// Returns true if this control can stop right now. False otherwise.
	virtual bool canStop() const { return false; }
signals:

public slots:

protected slots:
	/// Updates the connected state.
	virtual void updateConnected() {}
	/// Updates the current value.
	virtual void updateValue() {}

	void onMoveActionStateChanged(int newActionState, int oldActionState);

protected:
	/// Override of create move action. The mock has its own implementation of move
	/// which doesn't require the creation of move actions.
	AMAction3* createMoveAction(double);

};

#endif // AMMOCKCONTROL_H
