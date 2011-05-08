#ifndef PIDLOOPCONTROL_H
#define PIDLOOPCONTROL_H

#include <QObject>

#include "beamline/AMControl.h"

/*!
	This class monitors the PID feedback loops for the sample stage or wire stage.  The motor behaves erratically when the loops are off, and thus there should be a mechanism to turn them back on.
  */
class PIDLoopControl : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  Takes in the three PID loops for each motor.
	explicit PIDLoopControl(AMPVControl *pidX, AMPVControl *pidY, AMPVControl *pidZ, QObject *parent = 0);

	/// Returns the state of PID for the x-direction motor.
	bool isOnX() { return (int)x_->value() == 1 ? true : false; }
	/// Returns the state of PID for the y-direction motor.
	bool isOnY() { return (int)y_->value() == 1 ? true : false; }
	/// Returns the state of PID for the z-direction motor.
	bool isOnZ() { return (int)z_->value() == 1 ? true : false; }

	/// Returns the overall state of all three PID loops together.
	bool state() { return isOnX() && isOnY() && isOnZ(); }

signals:
	/// Notifies if the overall state of the three PID loops changes.
	void stateChanged(bool);

public slots:
	/// Turns on the X PID loop.
	void turnOnX() { if (!isOnX()) x_->move(1); }
	/// Turns on the Y PID loop.
	void turnOnY() { if (!isOnY()) y_->move(1); }
	/// Turns on the Z PID loop.
	void turnOnZ() { if (!isOnZ()) z_->move(1); }
	/// Turn on all PID loops.
	void turnOn() { turnOnX(); turnOnY(); turnOnZ(); }

protected slots:
	/// Handles when the x value changes.
	void onXStateChanged() { emit stateChanged(isOnX()); }
	/// Handles when the y value changes.
	void onYStateChanged() { emit stateChanged(isOnY()); }
	/// Handles when the z value changes.
	void onZStateChanged() { emit stateChanged(isOnZ()); }

protected:

	/// Pointer to the PID control for the x motor.
	AMPVControl *x_;
	/// Pointer to the PID control for the y motor.
	AMPVControl *y_;
	/// Pointer to the PID control for the z motor.
	AMPVControl *z_;

};

#endif // PIDLOOPCONTROL_H
