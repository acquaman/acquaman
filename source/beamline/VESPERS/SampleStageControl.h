#ifndef SAMPLESTAGECONTROL_H
#define SAMPLESTAGECONTROL_H

#include <QObject>
#include <QPair>

#include "beamline/CLS/CLSVMEMotor.h"

/*!
	This class offers an encapsulated way of using a tri-motor sample stage.  It uses the steps from the base motors for determining if the sample stage is going to over the limit.
	It then uses the AMPVwStatusControls to move the motor as one would expect to actually moves.  The important thing is that it prevents the motor from reaching it's hard limits
	and then possibly having unexpected behaviour.  This is why the reading stages are using the counts from the motor rather than the engineering units.  This way the protection
	aspect of this class is always functional, even if the other motors are no longer calibrated correctly.

	Note:  All the controls must exist already.

	The ability to change what motors are the horizontal, vertical, and normal directions is also offered.  However, you must ensure that the scalers are set to the appropriate
	values because they are reset to 1 for all directions.  The ranges are left as they were because they might not have been set or the step motors wouldn't have been changed.
  */
class SampleStageControl : public QObject
{
	Q_OBJECT
public:
	/// Motor status enum.
	enum MotorStatus { MoveDone = 0, MoveActive, AtLimit, ForcedStop, Error };

	/// Default constructor.  Takes in the 3 motor controls and their count readout controls.
	explicit SampleStageControl(AMControl *horiz, AMControl *vert, AMControl *norm, QObject *parent = 0);

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
	MotorStatus horizontalStatus() { return intToStatus(horiz_->movingPVValue()); }
	/// Returns the status of the vertical motor.  This is the MotorStatus enum found in the beamline.
	MotorStatus verticalStatus() { return intToStatus(vert_->movingPVValue()); }
	/// Returns the status of the normal motor.  This is the MotorStatus enum found in the beamline.
	MotorStatus normalStatus() { return intToStatus(norm_->movingPVValue()); }

	/// Returns the current position of the horizontal control.
	double horizontalPosition() const { return horiz_->value(); }
	/// Returns the current position of the vertical control.
	double verticalPosition() const { return vert_->value(); }
	/// Returns the current position of the normal control.
	double normalPosition() const { return norm_->value(); }

	// Return the controls used in this class.
	/// Returns the horizontal motor control.
	AMControl *horiz() const { return horiz_; }
	/// Returns the vertical motor control.
	AMControl *vert() const { return vert_; }
	/// Returns the normal motor control.
	AMControl *norm() const { return norm_; }

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
	/// Notifier that a move was unsuccessful.  Passes an error string that states which of the directions failed.
	void moveError(QString);

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
};

#endif // SAMPLESTAGECONTROL_H
