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


#include "CLSMDriveMotorControl.h"

#include <QStringBuilder>

#include "util/AMErrorMonitor.h"

 CLSMDriveMotorControl::~CLSMDriveMotorControl(){}
CLSMDriveMotorControl::CLSMDriveMotorControl(const QString &name, const QString &baseName, const QString& units, double unitsPerRev, double offset, int microsteps, const QString &description, double tolerance, double moveStartTimeoutSeconds, QObject *parent)
	: AMPVwStatusAndUnitConversionControl(name, baseName % ":enc:fbk", baseName % ":step", baseName % ":status", baseName % ":stop", new AMScaleAndOffsetUnitConverter(units, unitsPerRev/4000.0, offset), new AMScaleAndOffsetUnitConverter(units, unitsPerRev/(200.0*microsteps), offset), parent, tolerance, moveStartTimeoutSeconds, new AMControlStatusCheckerDefault(1), 1, description) {

	// Unlike MaxV controllers, these motors can support move updates while moving:
	setAllowsMovesWhileMoving(true);
	// Because of the polled communication, it can take a while for these motors to send MOVE ACTIVE then MOVE DONE for null moves. Recommend setting the moveStartTolerance() [in converted units, not microsteps] when setting up these motors. It should be set very small... equivalent to a few microsteps.
	// ex: setMoveStartTolerance(writeUnitConverter()->convertFromRaw(5))
	moveStartTolerance_ = 0;
	moveTimeoutTolerance_ = 0;
}

// Start a move to the value setpoint:
AMControl::FailureExplanation CLSMDriveMotorControl::move(double Setpoint) {

	if(isMoving()) {
		if(!allowsMovesWhileMoving()) {
			AMErrorMon::debug(this, AMPVCONTROL_COULD_NOT_MOVE_WHILE_MOVING, QString("AMPVControl: Could not move %1 (%2) to %3, because the control is already moving.").arg(name()).arg(writePV_->pvName()).arg(setpoint_));
			return AlreadyMovingFailure;
		}

		if(!moveInProgress()) {
			// the control is already moving, but it's not one of our moves. In this situation, there is no way that we can start a move and be assured that we'll be notified when OUR move finishes.
			AMErrorMon::debug(this, AMPVCONTROL_COULD_NOT_MOVE_WHILE_MOVING_EXTERNAL, QString("AMPVControl: Could not move %1 (%2) to %3, because the control is already moving.").arg(name()).arg(writePV_->pvName()).arg(setpoint_));
			return AlreadyMovingFailure;
		}

		// Otherwise: This control supports mid-move updates, and we're already moving. We just need to update the setpoint and send it.
		setpoint_ = writeUnitConverter()->convertToRaw(Setpoint);
		writePV_->setValue(setpoint_);
		// since the settling phase is considered part of a move, it's OK to be here while settling... But for Acquaman purposes, this will be considered a single re-targetted move, even though the hardware will see two.  If we're settling, disable the settling timer, because we only want to respond to the end of the second move.
		if(settlingInProgress_) {
			settlingInProgress_ = false;
			settlingTimer_.stop();
		}
		emit moveReTargetted(); // re-targetted moves will emit moveReTargetted(), although no moveSucceeded()/moveFailed() will be issued for the initial move.
	}

	else {
		settlingInProgress_ = false;
		stopInProgress_ = false;
		moveInProgress_ = false;
		// Flag that "our" move started:
		startInProgress_ = true;

		// This is our new target:
		setpoint_ = writeUnitConverter()->convertToRaw(Setpoint);

		// Special case: "null move" should complete immediately. Use only if moveStartTolerance() is non-zero, and the move distance is within moveStartTolerance().
		if(moveStartTolerance() != 0 && fabs(setpoint()-value()) < moveStartTolerance()) {
			startInProgress_ = false;
			moveInProgress_ = true;
			emit moveStarted();
			moveInProgress_ = false;
			emit moveSucceeded();
		}
		// Normal move:
		else {
			// Issue the move command:
			writePV_->setValue(setpoint_);
			// start the timer to check if our move failed to start:
			moveStartTimer_.start(int(moveStartTimeout_*1000.0));
		}
	}

	return NoFailure;
}

// This is used to handle the timeout of a move start:
void CLSMDriveMotorControl::onMoveStartTimeout() {

	moveStartTimer_.stop();

	// This is only meaningful if one of our moves is in progress.
	if(startInProgress_) {
		// give up on this move:
		startInProgress_ = false;

		// Special case: only applies if moveTimeoutTolerance_ != 0 AND we've gotten within moveTimeoutTolerance_ of the setpoint.
		if(moveTimeoutTolerance() != 0.0 && fabs(setpoint() - value()) < moveTimeoutTolerance_) {
			moveInProgress_ = true;
			emit moveStarted();
			moveInProgress_ = false;
			emit moveSucceeded();
		}
		else {
			// The move didn't start within our allowed start period. That counts as a move failed.
			emit moveFailed(AMControl::TimeoutFailure);
		}
	}
}
