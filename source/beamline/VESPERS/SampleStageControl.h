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
  */
class SampleStageControl : public QObject
{
	Q_OBJECT
public:
	/// Default constructor.  Takes in the 3 motor controls and their count readout controls.
	explicit SampleStageControl(AMPVwStatusControl *horiz, AMPVwStatusControl *vert, AMPVwStatusControl *norm, AMReadOnlyPVControl *xMotor, AMReadOnlyPVControl *yMotor, AMReadOnlyPVControl *zMotor, QObject *parent = 0);

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

	/// Returns the status of the sample stage (ie: if it's moving or not).
	bool status() { return horiz_->isMoving() && vert_->isMoving() && norm_->isMoving(); }

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

public slots:
	/// Moves the sample stage in the horizontal direction.
	void moveHorizontal(double setpoint);
	/// Moves the sample stage in the vertical direction.
	void moveVertical(double setpoint);
	/// Moves the sample stage in the normal direction.
	void moveNormal(double setpoint);

private:

	// The limits.
	int xLow_;
	int yLow_;
	int zLow_;
	int xHigh_;
	int yHigh_;
	int zHigh_;

	// The motor controls
	AMPVwStatusControl *horiz_;
	AMPVwStatusControl *vert_;
	AMPVwStatusControl *norm_;
	AMReadOnlyPVControl *xMotor_;
	AMReadOnlyPVControl *yMotor_;
	AMReadOnlyPVControl *zMotor_;
};

#endif // SAMPLESTAGECONTROL_H
