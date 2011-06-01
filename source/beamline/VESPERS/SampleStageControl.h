#ifndef SAMPLESTAGECONTROL_H
#define SAMPLESTAGECONTROL_H

#include <QObject>
#include <QPair>

#include "beamline/AMControl.h"

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
	explicit SampleStageControl(AMPVwStatusControl *horiz, AMPVwStatusControl *vert, AMPVwStatusControl *norm, AMReadOnlyPVControl *xMotor, AMReadOnlyPVControl *yMotor, AMReadOnlyPVControl *zMotor, QObject *parent = 0);

	/// Changes which motors are the used for the horizontal, vertical, and normal directions.  Sets all scalers to 1.  Returns false if unsuccessfully changed.
	bool setMotors(AMControl *horiz, AMControl *vert, AMControl *norm);

	/// Sets the range for the motor in the x direction.
	void setXRange(int low, int high) { xLow_ = low; xHigh_ = high; }
	/// Sets the range for the motor in the y direction.
	void setYRange(int low, int high) { yLow_ = low; yHigh_ = high; }
	/// Sets the range for the motor in the x direction.
	void setZRange(int low, int high) { zLow_ = low; zHigh_ = high; }

	/// Returns the range for the motor in the x direction.  Returned as a QPair<low, high>.
	QPair<int, int> xRange() const { return qMakePair(xLow_, xHigh_); }
	/// Returns the range for the motor in the y direction.  Returned as a QPair<low, high>.
	QPair<int, int> yRange() const { return qMakePair(yLow_, yHigh_); }
	/// Returns the range for the motor in the z direction.  Returned as a QPair<low, high>.
	QPair<int, int> zRange() const { return qMakePair(zLow_, zHigh_); }

	/// Sets the scaling for the x motor.  This is to accomodate if the directions are not simple motor movements but more sophisticated moves.
	void setXScaler(double sx) { sx_ = sx; }
	/// Sets the scaling for the y motor.  This is to accomodate if the directions are not simple motor movements but more sophisticated moves.
	void setYScaler(double sy) { sy_ = sy; }
	/// Sets the scaling for the z motor.  This is to accomodate if the directions are not simple motor movements but more sophisticated moves.
	void setZScaler(double sz) { sz_ = sz; }
	/// Sets the scaling for all three motors.
	void setScalers(double sx, double sy, double sz) { sx_ = sx; sy_ = sy; sz_ = sz; }

	/// Returns the scaling for the x motor.
	double xScaler() const { return sx_; }
	/// Returns the scaling for the y motor.
	double yScaler() const { return sy_; }
	/// Returns the scaling for the z motor.
	double zScaler() const { return sz_; }

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
	AMPVwStatusControl *horiz() const { return horiz_; }
	/// Returns the vertical motor control.
	AMPVwStatusControl *vert() const { return vert_; }
	/// Returns the normal motor control.
	AMPVwStatusControl *norm() const { return norm_; }
	/// Returns the x position motor reading control.
	AMReadOnlyPVControl *xMotor() const { return xMotor_; }
	/// Returns the y position motor reading control.
	AMReadOnlyPVControl *yMotor() const { return yMotor_; }
	/// Returns the z position motor reading control.
	AMReadOnlyPVControl *zMotor() const { return zMotor_; }

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

public slots:
	/// Moves the sample stage in the horizontal direction.
	void moveHorizontal(double setpoint);
	/// Moves the sample stage in the vertical direction.
	void moveVertical(double setpoint);
	/// Moves the sample stage in the normal direction.
	void moveNormal(double setpoint);

protected:

	/// Helper function.  Determines if the new x position falls within the x range.  If the lower limit or upper limit is not set (ie: equal to 0) then it returns true.
	bool validNewXPosition(double setpoint);
	/// Helper function.  Determines if the new y position falls within the y range.  If the lower limit or upper limit is not set (ie: equal to 0) then it returns true.
	bool validNewYPosition(double setpoint);
	/// Helper function.  Determines if the new z position falls within the z range.  If the lower limit or upper limit is not set (ie: equal to 0) then it returns true.
	bool validNewZPosition(double setpoint);

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
	int xLow_;
	int yLow_;
	int zLow_;
	int xHigh_;
	int yHigh_;
	int zHigh_;

	// Scalers.
	double sx_;
	double sy_;
	double sz_;

	// The motor controls
	AMPVwStatusControl *horiz_;
	AMPVwStatusControl *vert_;
	AMPVwStatusControl *norm_;
	AMReadOnlyPVControl *xMotor_;
	AMReadOnlyPVControl *yMotor_;
	AMReadOnlyPVControl *zMotor_;
};

#endif // SAMPLESTAGECONTROL_H
