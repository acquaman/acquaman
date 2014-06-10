/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef CLSMDRIVEMOTORCONTROL_H
#define CLSMDRIVEMOTORCONTROL_H

#include "beamline/AMPVControl.h"

/// This class is a subclass of AMPVwStatusControl that is useful for IMS MDrive motors at the CLS using the initial version of the driver that doesn't support unit conversion. (Future versions of that driver will have unit conversion built-in, and this class can switch to using AMPVwStatusControl instead of AMPVwStatusAndUnitConversionControl.
class CLSMDriveMotorControl : public AMPVwStatusAndUnitConversionControl
{
	Q_OBJECT
public:
	/// Constructor
	/*!
	  \param name A unique representative name for this motor
	  \param basePVName The base of the PV name (ie: if the motor status was "SMTR16114I1022:status" then the base is "SMTR16114I1022")
	  \param units The name of the units that describe positions for this motor
	  \param unitsPerRev The scale factor used to convert from motor revolutions to desired units (ie: units/rev).  value() = step position / (200*microsteps steps/rev) * unitsPerRev + offset.  value() = encoder position / (4000 lines/rev) * unitsPerRev + offset.
	  \param offset The offset used in the conversion from motor revolutions to desired units (in desired units).  value() = step position / (200*microsteps steps/rev) * unitsPerRev + offset.  value() = encoder position / (4000 lines/rev) * unitsPerRev + offset.
	  \param microsteps The microstep setting for the motor. We need to know this because the step position depends on the microstep setting, but the encoder feedback is always in 4000 lines/rev.
	  \param description A human readable description for this motor
	  \param tolerance The tolerance that will be used to determine if moves succeed in getting close enough to be considered a success. It should be specified in our desired units.
	  \param moveStartTimeout How long the motor has to start moving before something is considered wrong
	  \param parent QObject parent class

	  Implementation note: We need to use separate readUnitConverters and writeUnitConverters, since the feedback PV (readPV) is in encoder counts at 4000lines/rev, and the move PV (writePV) is in steps (ie: 200*microsteps/rev).
	  */
 	virtual ~CLSMDriveMotorControl();
	CLSMDriveMotorControl(const QString &name, const QString &basePVName, const QString& units, double unitsPerRev, double offset, int microsteps, const QString &description, double tolerance, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0);

	/// A non-zero moveStartTolerance() allows "null moves" (moves with setpoints within moveStartTolerance() of the current feedback value) to start and succeed immediately without any motion.  This is necessary for controls that do not change their move status when told to go to the current position. (By default, this is 0 and has no effect.)
	/*! A "null move" is a move to the current position (or something very very close to it).  Some controls may not change their move status on a null move; in this case, the move would appear to fail, even though the control "reached" its target. This provides an optional work-around: if moveStartTolerance() is non-zero, and the current feedback value() is within moveStartTolerance() of the setpoint, a move() command will start and succeed immediately without any physical motion.  Note that the hardware is NOT told to move in this mode. (It if was, any move status change might be interpreted as the end of a subsequent move.)

	By default, moveStartTolerance_ is 0 and has no effect.
*/
	double moveStartTolerance() const { return moveStartTolerance_; }

	/// A non-zero moveTimeoutTolerance() allows short moves to succeed after the moveStartTimeout() EVEN IF THE MOVING STATUS HAS NOT CHANGED, but the value() has entered within moveTimeoutTolerance() of the setpoint.  This is necessary for controls that may not change their move status at all during short moves, even though the move actually takes place. (ex: serial-connected devices that are polled, etc.)
	/*! Unlike a "null-move" to the current position, this setting applies to real moves that are simply too short for the driver to update the moving status.  When enabled, if the moving status has not changed after moveStartTimeout() seconds, if the current position is within moveTimeoutTolerance() of the setpoint, the move will succeed; otherwise it will fail with a TimeoutFailure.

	  By default, moveTimeoutTolerance() is 0 and has no effect; the moving status MUST CHANGE to avoid a TimeoutFailure.
	  */
	double moveTimeoutTolerance() const { return moveTimeoutTolerance_; }

signals:

public slots:
	/// Start a move to the value setpoint (reimplemented)
	virtual FailureExplanation move(double setpoint);

	/// Set a non-zero moveStartTolerance() to allow "null moves" (setpoints within moveStartTolerance() of the current feedback value) to start and succeed immediately without any motion.  This is necessary for controls that do not change their move status when told to go to the current position. \see moveStartTolerance().
	void setMoveStartTolerance(double moveStartTolerance) { moveStartTolerance_ = moveStartTolerance; }

	/// Set a non-zero moveTimeoutTolerance() to allow short moves to succeed even if the moving status has not changed after moveStartTimeout(). This is necessary for controls that may not change the moving status for short moves. \see moveTimeoutTolerance().
	void setMoveTimeoutTolerance(double moveTimeoutTolerance) { moveTimeoutTolerance_ = moveTimeoutTolerance; }

protected slots:
	/// This is used to handle the timeout of a move start:
	void onMoveStartTimeout();

protected:
	/// If non-zero, reports moveSucceeded() immediately for move() requests that are within moveStartTolerance_ of the setpoint.  By default, this is zero and has no effect. \see moveStartTolerance()
	double moveStartTolerance_;
	/// If non-zero, allows moves to succeed if the position is within moveTimeoutTolerance_, even if the moving status has not changed after moveStartTimeout_ seconds. \see moveTimeoutTolerance()
	double moveTimeoutTolerance_;
};

#endif // CLSMDRIVEMOTORCONTROL_H
