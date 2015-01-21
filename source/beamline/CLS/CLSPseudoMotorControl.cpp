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


#include "CLSPseudoMotorControl.h"

CLSPseudoMotorControl::CLSPseudoMotorControl(const QString &name, const QString &readPVname, const QString &writePVname, const QString &movingPVname, const QString &stopPVname, QObject *parent, double tolerance, double moveStartTimeoutSeconds, AMAbstractControlStatusChecker *statusChecker, int stopValue, const QString &description)
	: AMPVwStatusControl(name, readPVname, writePVname, movingPVname, stopPVname, parent, tolerance, moveStartTimeoutSeconds, statusChecker, stopValue, description)
{
	delayMove_ = false;
	delayMoveSetpoint_ = 0.0;
}

AMControl::FailureExplanation CLSPseudoMotorControl::move(double setpoint)
{
	if(isMoving()) {

		delayMove_ = true;
		delayMoveSetpoint_ = setpoint;
	}

	else {

		if (delayMove_)
			delayMove_ = false;

		settlingInProgress_ = false;
		stopInProgress_ = false;
		moveInProgress_ = false;
		// Flag that "our" move started:
		startInProgress_ = true;

		// This is our new target:
		setpoint_ = setpoint;

		// Normal move:
		// Issue the move command, check on attemptMoveWhenWithinTolerance
		if(!attemptMoveWhenWithinTolerance_ && inPosition())
			emit moveSucceeded();
		else{
			writePV_->setValue(setpoint_);
			// start the timer to check if our move failed to start:
			moveStartTimer_.start(int(moveStartTimeout_*1000.0));
		}
	}

	return NoFailure;
}

bool CLSPseudoMotorControl::stop()
{
	if (delayMove_)
		delayMove_ = false;

	return AMPVwStatusControl::stop();
}

void CLSPseudoMotorControl::onMovingChanged(int isMovingValue)
{
	AMPVwStatusControl::onMovingChanged(isMovingValue);

	if (delayMove_)
		move(delayMoveSetpoint_);
}
