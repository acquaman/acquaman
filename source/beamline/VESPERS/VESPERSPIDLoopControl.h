/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef VESPERSPIDLOOPCONTROL_H
#define VESPERSPIDLOOPCONTROL_H

#include <QObject>

#include "beamline/AMPVControl.h"
#include "actions3/AMAction3.h"
#include "util/AMErrorMonitor.h"

/*!
	This class monitors the PID feedback loops for the sample stage or wire stage.  The motor behaves erratically when the loops are off, and thus there should be a mechanism to turn them back on.
  */
class VESPERSPIDLoopControl : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  Takes in the three PID loops for each motor as well as a description.
 	virtual ~VESPERSPIDLoopControl();
		explicit VESPERSPIDLoopControl(QString name, AMControl *pidX, AMControl *pidY, AMControl *pidZ, QObject *parent = 0);

	/// Returns the name of the PID control.
	QString name() const { return name_; }
	/// Returns the state of PID for the x-direction motor.
	bool isOnX() { return (int)x_->value() == 1 ? true : false; }
	/// Returns the state of PID for the y-direction motor.
	bool isOnY() { return (int)y_->value() == 1 ? true : false; }
	/// Returns the state of PID for the z-direction motor.
	bool isOnZ() { return (int)z_->value() == 1 ? true : false; }

	/// Returns the overall state of all three PID loops together.
	bool state() { return isOnX() && isOnY() && isOnZ(); }

	// Actions to turn them on and off.
	/// Returns a newly created action that turns the PID X loop control on or off depending on \param turnOn.  Returns 0 if not connected.
	AMAction3 *createPIDXChangeStateAction(bool turnOn);
	/// Returns a newly created action that turns the PID Y loop control on or off depending on \param turnOn.  Returns 0 if not connected.
	AMAction3 *createPIDYChangeStateAction(bool turnOn);
	/// Returns a newly created action that turns the PID Z loop control on or off depending on \param turnOn.  Returns 0 if not connected.
	AMAction3 *createPIDZChangeStateAction(bool turnOn);
	/// Returns a newly created action that turns all the PID loop controls on or off depending on \param turnOn.  Returns 0 if not all of the PID loop controls are connected.
	AMAction3 *createPIDChangeStateAction(bool turnOn);

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
	/// Repairs the sample stage.  Combines stopping the motors (if they haven't stopped yet) and turns on all the PID loop controls.  Due to the real time nature of these events, actions are used instead of setters.
	void repair();

protected slots:
	/// Handles when the x value changes.
	void onXStateChanged() { emit stateChanged(state()); }
	/// Handles when the y value changes.
	void onYStateChanged() { emit stateChanged(state()); }
	/// Handles when the z value changes.
	void onZStateChanged() { emit stateChanged(state()); }
	/// Handles what happens when the repair action fails.
	void onRepairFailed() { AMErrorMon::alert(this, 0, "The PID loop repair has failed.  Either try again or contact the beamline staff"); }

protected:

	QString name_;
	/// Pointer to the PID control for the x motor.
	AMControl *x_;
	/// Pointer to the PID control for the y motor.
	AMControl *y_;
	/// Pointer to the PID control for the z motor.
	AMControl *z_;

};

#endif // VESPERSPIDLOOPCONTROL_H
