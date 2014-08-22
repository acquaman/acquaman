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


#ifndef VESPERSMONOCHOMATORCONTROL_H
#define VESPERSMONOCHOMATORCONTROL_H

#include "beamline/AMPVControl.h"

/// This class is a very simple extension of AMPVwStatusControl meant to overide the move() method to emit moveSucceeded() immediately if the new setpoint is less than 0.001 of the current value.
class VESPERSMonochomatorControl : public AMPVwStatusControl
{
	Q_OBJECT

public:
	/// Constructor.
	/*! \param name A unique representative name for this control
  \param readPVname The EPICS channel-access name for the feedback Process Variable
  \param writePVname The EPICS channel-access name for the setpoint Process Variable
  \param movingPVname The EPICS channel-access name for the move-monitor Process Variable
  \param tolerance The level of accuracy (max. distance between setpoint() and final value()) required for a move() to be successful
  \param moveStartTimeoutSeconds Time allowed after a move() for the Control to first start moving.  If it doesn't, we emit moveFailed(AMControl::TimeoutFailure).
  \param statusChecker An instance of an AMAbstractControlStatusChecker.  isMoving() is true when the movingPV's value passed into its operator()() function returns true.  The default status checker compares the movingPV's value to 1.  (Note: this class takes ownership of the statusChecker and deletes it when done.)
  \param stopPVname is the EPICS channel-access name for the Process Variable used to stop() a move in progress.
  \param stopValue is the value that will be written to the stopPV when stop() is called.
  \param parent QObject parent class
  */
 	virtual ~VESPERSMonochomatorControl();
	VESPERSMonochomatorControl(const QString& name,
							   const QString& readPVname,
							   const QString& writePVname,
							   const QString& movingPVname,
							   const QString& stopPVname = QString(),
							   QObject* parent = 0,
							   double tolerance = AMCONTROL_TOLERANCE_DONT_CARE,
							   double moveStartTimeoutSeconds = 2.0,
							   AMAbstractControlStatusChecker* statusChecker = new AMControlStatusCheckerDefault(1),
							   int stopValue = 1,
							   const QString &description = "");

public slots:
	/// Re-implemented move method.
	AMControl::FailureExplanation move(double setpoint);
};

#endif // VESPERSMONOCHOMATORCONTROL_H
