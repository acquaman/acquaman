#ifndef CLSBISTATECONTROL_H
#define CLSBISTATECONTROL_H

#include "beamline/AMControl.h"

/*!
  This class implements AMControl to encapsulate the access convention to valves and shutters at the CLS.  Unlike most other controls/process variables, valves and shutters
  have three PVs: one for the current state of the valve/shutter, one to open it, and one to close it.
  */
class CLSBiStateControl : public AMControl
{
	Q_OBJECT

public:
	CLSBiStateControl();

	/// This represents the current value/position of the control. Must reimplement for actual controls.
	virtual double value() const { return -1; }

	/// this is the "setpoint": the last place the control was told to go:
	virtual double setpoint() const { return -1; }

	/// this indicates whether a control is "in position" (ie: its value is within tolerance of the setpoint)
	virtual bool inPosition() const { return fabs(value()-setpoint()) < tolerance(); }

	/// this indicates whether a contorl is "within tolerance" of a given target (ie: the target specified is the same as the current value within the set tolerance)
	virtual bool withinTolerance(double target) const { return fabs(value()-target) < tolerance(); }

	/// \name Capabilities
	/// These indicate the current cabilities and status of this control. Unconnected controls can't do anything.
	//@{
	/// Indicates a "fully-functional" control, ready for action
	virtual bool isConnected() const { return false; }
	/// Indicates that this control \em can currently take measurements.
	virtual bool canMeasure() const { return false; }
	/// Indicates that this control type \em should (assuming it's connected) be able to measure values.
	virtual bool shouldMeasure() const { return false; }
	/// Indicates thatthis control \em can (currently) move to setpoints:
	virtual bool canMove() const { return false; }
	/// Indicates that this control \em should (assuming it's connected) be able to move to setpoints:
	virtual bool shouldMove() const { return false; }
	/// Indicates that this control \em can (currently) issue stop() commands while moves are in progress.
	virtual bool canStop() const { return false; }
	/// Indicates that this control \em shoule (assuming it's connected) be able to issue stop() commands while moves are in progress.
	virtual bool shouldStop() const { return false; }

	virtual bool isMoving() const { return false; }

	virtual bool moveInProgress() const { return false; }

	/// \name Information on the allowed range for this control:
	//@{
	/// These are the minimum and maximum allowed values for this control.
	virtual QPair<double,double> range() const { return QPair<double,double>(minimumValue(), maximumValue()); }
	virtual double minimumValue() const { return -1; }
	virtual double maximumValue() const { return -1; }

public slots:
	/// This is used to move the control to a setpoint.  Must reimplement for actual controls
	virtual void move(double setpoint) {
		Q_UNUSED(setpoint);
		emit moveFailed(AMControl::NotConnectedFailure);  // The default implementation cannot move.
	}

	/// This is used to cancel or stop a move in progress. Must reimplement for actual controls.  It will be successful only if canStop() is true.  Returns true if the stop command was successfully sent.  (Note: this DOES NOT guarantee that the motor actually stopped!)
	virtual bool stop() { return false; }
};

#endif // CLSBISTATECONTROL_H
