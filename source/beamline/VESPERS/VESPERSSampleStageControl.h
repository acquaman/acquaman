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


#ifndef VESPERSSAMPLESTAGECONTROL_H
#define VESPERSSAMPLESTAGECONTROL_H

#include <QObject>
#include <QPair>

#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions/AMBeamlineActionItem.h"

/*!
	This class offers an encapsulated way of using a tri-motor sample stage.  It uses the steps from the base motors for determining if the sample stage is going to over the limit.
	It then uses the AMPVwStatusControls to move the motor as one would expect to actually moves.  The important thing is that it prevents the motor from reaching it's hard limits
	and then possibly having unexpected behaviour.  This is why the reading stages are using the counts from the motor rather than the engineering units.  This way the protection
	aspect of this class is always functional, even if the other motors are no longer calibrated correctly.

	Note:  All the controls must exist already.

	The ability to change what motors are the horizontal, vertical, and normal directions is also offered.  However, you must ensure that the scalers are set to the appropriate
	values because they are reset to 1 for all directions.  The ranges are left as they were because they might not have been set or the step motors wouldn't have been changed.
  */
class VESPERSSampleStageControl : public QObject
{
	Q_OBJECT
public:
	/// Motor status enum.
	enum MotorStatus { MoveDone = 0, MoveActive, AtLimit, ForcedStop, Error };

	/// Default constructor.  Takes in the 3 motor controls and their count readout controls.
	explicit VESPERSSampleStageControl(AMControl *horiz, AMControl *vert, AMControl *norm, QObject *parent = 0);

	/// Returns the connected state of the sample stage.
	bool isConnected() const { return connected_; }

	/// Sets the range for the motor in the x direction.
	void setXRange(int low, int high) { xRange_ = qMakePair(low, high); }
	/// Sets the range for the motor in the y direction.
	void setYRange(int low, int high) { yRange_ = qMakePair(low, high); }
	/// Sets the range for the motor in the x direction.
	void setZRange(int low, int high) { zRange_ = qMakePair(low, high); }

	/// Returns the range for the motor in the x direction.  Returned as a QPair<low, high>.
	QPair<int, int> xRange() const { return xRange_; }
	/// Returns the range for the motor in the y direction.  Returned as a QPair<low, high>.
	QPair<int, int> yRange() const { return yRange_; }
	/// Returns the range for the motor in the z direction.  Returned as a QPair<low, high>.
	QPair<int, int> zRange() const { return zRange_; }

	/// Returns the status of the sample stage (ie: if it's moving or not).
	bool status() { return horiz_->isMoving() || vert_->isMoving() || norm_->isMoving(); }
	/// Returns the status of the horizontal motor.  This is the MotorStatus enum found in the beamline.
	MotorStatus horizontalStatus() { return intToStatus(((AMPVwStatusControl *)horiz_)->movingPVValue()); }
	/// Returns the status of the vertical motor.  This is the MotorStatus enum found in the beamline.
	MotorStatus verticalStatus() { return intToStatus(((AMPVwStatusControl *)vert_)->movingPVValue()); }
	/// Returns the status of the normal motor.  This is the MotorStatus enum found in the beamline.
	MotorStatus normalStatus() { return intToStatus(((AMPVwStatusControl *)norm_)->movingPVValue()); }

	/// Returns the current position of the horizontal control.
	double horizontalPosition() const { return horiz_->value(); }
	/// Returns the current position of the vertical control.
	double verticalPosition() const { return vert_->value(); }
	/// Returns the current position of the normal control.
	double normalPosition() const { return norm_->value(); }

	// Pointers to the controls.  This should be removed, but some code still requires it.  Once that code has been changed, then I'll remove these.
	/// Returns the pointer to the horizontal motor.
	AMControl *horiz() const { return horiz_; }
	/// Returns the pointer to the vertical motor.
	AMControl *vert() const { return vert_; }
	/// Returns the pointer to the normal motor.
	AMControl *norm() const { return norm_; }

	// Actions others might find useful. Primarily ControlMove actions and ControlStop actions.
	/// Returns a newly created move action for the horizontal postion.  Returns 0 if not connected.
	AMBeamlineActionItem *createHorizontalMoveAction(double position);
	/// Returns a newly created stop action for the horizontal position.  Returns 0 if not connected.
	AMBeamlineActionItem *createHorizontalStopAction();
	/// Returns a newly created move action for the vertical postion.  Returns 0 if not connected.
	AMBeamlineActionItem *createVerticalMoveAction(double position);
	/// Returns a newly created stop action for the vertical position.  Returns 0 if not connected.
	AMBeamlineActionItem *createVerticalStopAction();
	/// Returns a newly created move action for the normal postion.  Returns 0 if not connected.
	AMBeamlineActionItem *createNormalMoveAction(double position);
	/// Returns a newly created stop action for the normal position.  Returns 0 if not connected.
	AMBeamlineActionItem *createNormalStopAction();
	/// Returns a newly created action that stops ALL the motors.  Returns 0 if not all motors are connected.
	AMBeamlineActionItem *createStopAllAction();

signals:
	/// Notifies whether the sample stage is currently connected.
	void connected(bool);
	/// Notifies whether the sample stage is currently moving.
	void movingChanged(bool);
	/// Notifies whether the horizontal motor setpoint has changed.
	void horizontalSetpointChanged(double);
	/// Notifies whether the vertical motor setpoint has changed.
	void verticalSetpointChanged(double);
	/// Notifies whether the normal motor setpoint has changed.
	void normalSetpointChanged(double);
	/// Notifier that a horizontal move was unsuccessful.  Passes an bool that states which of the direction failed.  False is past the low limit, true is past the high limit.
	void horizontalMoveError(bool);
	/// Notifier that a vertical move was unsuccessful.  Passes an bool that states which of the direction failed.  False is past the low limit, true is past the high limit.
	void verticalMoveError(bool);
	/// Notifier that a normal move was unsuccessful.  Passes an bool that states which of the direction failed.  False is past the low limit, true is past the high limit.
	void normalMoveError(bool);

public slots:
	/// Moves the sample stage in the horizontal direction.
	void moveHorizontal(double setpoint) { horiz_->move(setpoint); }
	/// Moves the sample stage in the vertical direction.
	void moveVertical(double setpoint) { vert_->move(setpoint); }
	/// Moves the sample stage in the normal direction.
	void moveNormal(double setpoint) { norm_->move(setpoint); }
	/// Stops the sample stage in the horizontal direction.
	void stopHorizontal() { horiz_->stop(); }
	/// Stops the sample stage in the vertical direction.
	void stopVertical() { vert_->stop(); }
	/// Stops the sample stage in the normal direction.
	void stopNormal() { norm_->stop(); }
	/// Stops all three motors.
	void stopAll() { horiz_->stop(); vert_->stop(); norm_->stop(); }

protected slots:
	/// Listens to the connection signals from the motors and emits connected.
	void onConnectedChanged() { connected_ = horiz_->isConnected() && vert_->isConnected() && norm_->isConnected(); emit connected(connected_); }
	/// Listens to the step feedback for the x motor and stops the sample stage if it exceeds the range.
	void onXStepChanged(double step);
	/// Listens to the step feedback for the y motor and stops the sample stage if it exceeds the range.
	void onYStepChanged(double step);
	/// Listens to the step feedback for the z motor and stops the sample stage if it exceeds the range.
	void onZStepChanged(double step);

protected:
	/// Returns the motor status enum corresponding to an integer.
	MotorStatus intToStatus(int val)
	{
		switch (val){
		case 0:
			return MoveDone;
		case 1:
			return MoveActive;
		case 2:
			return AtLimit;
		case 3:
			return ForcedStop;
		default:
			return Error;
		}
	}

	// The limits.
	QPair<int, int> xRange_;
	QPair<int, int> yRange_;
	QPair<int, int> zRange_;

	// The motor controls
	AMControl *horiz_;
	AMControl *vert_;
	AMControl *norm_;

	// Connected bool.
	bool connected_;
};

#endif // VESPERSSAMPLESTAGECONTROL_H
