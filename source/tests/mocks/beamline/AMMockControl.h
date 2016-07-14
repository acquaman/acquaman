#ifndef AMMOCKCONTROL_H
#define AMMOCKCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
#include "actions3/AMAction3.h"

/*!
  * A class representing a mock control. The control can be moved at a rate of
  * 1 unit per second.
  */
class AMMockControl : public AMPseudoMotorControl
{
    Q_OBJECT
public:
	/*!
	  * Creates an instance of an AMMockControl, with the provided name and units.
	  */
    explicit AMMockControl(const QString& name,
	                       const QString& units,
	                       QObject *parent = 0);

	/// Return true. Mock controls can always measure.
	virtual bool canMeasure() const { return true; }
	/// Returns true. Mock controls can always move.
	virtual bool canMove() const { return true; }
	/// Returns false. Mock controls can always be stopped.
	virtual bool canStop() const { return true; }
signals:

public slots:
	/// Stops the control.
	virtual bool stop();
protected slots:
	/// Does nothing. The mock control depends on the connection state of no other
	/// objects, and thus is always connected.
	virtual void updateConnected() {}
	/// Updates the current value.
	virtual void updateValue() {}

	/// Handles changes in state from the AMMockMoveAction which simulates the move.
	/// Sets this control's state to moving when the newActionState is Running,
	/// and sets it to not moving when it is Cancelled, Paused, Succeeded or Failed.
	void onMoveActionStateChanged(int newActionState, int oldActionState);

protected:
	/// Creates an AMMockMoveAction which simulates the motion of this control.
	AMAction3* createMoveAction(double);

	/// The current action performing a move. If no move is being performed.
	AMAction3* currentMoveAction_;
};

#endif // AMMOCKCONTROL_H
