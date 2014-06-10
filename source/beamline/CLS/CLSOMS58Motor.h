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


#ifndef CLSOMS58MOTOR_H
#define CLSOMS58MOTOR_H

#include "beamline/AMPVControl.h"

/// This function object provides the moving check for the CLSOMS58Motors
class AMControlStatusCheckerCLSOMS58 : public AMAbstractControlStatusChecker {
public:
	/// Status values will be compare to \c isStoppedValue, and return true if the status value is not equal to isStoppedValue (something that isn't stopped is moving)
 	virtual ~AMControlStatusCheckerCLSOMS58();
	AMControlStatusCheckerCLSOMS58() {}

	/// Return true (moving) if the \c statusValue is not 0 (STOPPED) and is not 3 (FORCED STOP) and is not 4 (ERROR)
	virtual bool operator()(quint32 statusValue) { return (statusValue != 0) && (statusValue != 3) && (statusValue != 4); }
};
/*
class CLSOMS58Motor : public AMPVwStatusControl
{
Q_OBJECT

public:
	enum Limit {
		LimitNone = 0,	///< Not at a limit
		LimitCW = 1,	///< At the CW limit
		LimitCCW = 2,	///< At the CCW limit
		LimitError = 3	///< There is a limit switch error (both of them are on at the same time or not connected)
	};

	enum PowerState {
		PowerOff = 0,		///< Power is OFF
		PowerOn = 1,		///< Power is ON
		PowerAutoHardware = 2,	///< Power is set to AutoHardware
		PowerAutoSoftware = 3,	///< Power is set to AutoSoftware
		PowerError = 4		///< There is an error in the power information
	};

	enum EncoderMovementType {
		EncoderMovementOff = 0,			///< Encoder movement approach is off
		EncoderMovementIncreaseOnly = 1,	///< Encoder movement approach is increase only
		EncoderMovementDecreaseOnly = 2,	///< Encoder movement approach is decrease only
		EncoderMovementIncrease = 3,		///< Encoder movement approach is increase
		EncoderMovementDecrease = 4,		///< Encoder movement approach is decrease
		EncoderMovementTypeError = 5		///< There is an error determining the encoder movement type
	};
*/
	/// Constructor
	/*!
	  \param name A unique description of this motor
	  \param baseName The base of the PV name (if the motor status was "SMTR16114I1022:status" then the base is "SMTR16114I1022")
	  \param description A human readable description for this motor
	  \param hasEncoder Should be set to true if the motor has an encoder
	  \param hasEGU Should be set to true if the motor is using EGU conversions (operates in mm rather than steps for example)
	  \param tolerance The tolerance that will be used to determine if moves succeed in getting close enough to be considered a success
	  \param moveStartTimeout How long the motor has to start moving before something is considered amis
	  \param parent QObject parent class
	  */
/*
 	virtual ~CLSOMS58Motor();
	CLSOMS58Motor(const QString &name, const QString &baseName, const QString &description, bool hasEncoder, bool hasEGU, double tolerance, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0);

	/// Indicates that all process variables for this motor are connected
	virtual bool isConnected() const;

	/// Returns whether of not this motor was set up to have EGU enabled
	bool hasEGU() const;
	/// Returns the (EGU) velocity setting for the velocity profile.  Returns 0 if the motor isn't connected yet.
	double EGUVelocity() const;
	/// Returns the (EGU) base velocity setting for the velocity profile.  Returns 0 if the motor isn't connected yet.
	double EGUBaseVelocity() const;
	/// Returns the (EGU) acceleration setting for the velocity profile.  Returns 0 if the motor isn't connected yet.
	double EGUAcceleration() const;
	/// Returns the (EGU) current velocity of the motor (zero when not moving, presumably non-zero when in motion).  Returns 0 if the motor isn't connected yet.
	double EGUCurrentVelocity() const;
	/// Returns the EGU offset of the motor. Returns 0 if the motor isn't connected yet.
	double EGUOffset() const;

	/// Returns the step setting.  Returns 0.5 if the motor isn't connected yet.  This is because steps are actually given in integers.
	double step() const;
	/// Returns the (step) velocity setting for the velocity profile.  Returns 0.5 if the motor isn't connected yet.  This is because velocity is actually given in integers.
	double stepVelocity() const;
	/// Returns the (step) base velocity setting for the velocity profile.  Returns 0.5 if the motor isn't connected yet.  This is because baseVelocity is actually given in integers.
	double stepBaseVelocity() const;
	/// Returns the (step) acceleration setting for the velocity profile.  Returns 0.5 if the motor isn't connected yet.  This is because acceleration is actually given in integers.
	double stepAcceleration() const;
	/// Returns the (step) current velocity of the motor (zero when not moving, presumably non-zero when in motion).  Returns 0.5 if the motor isn't connected yet.  This is because currentVelocity is actually given in integers.
	double stepCurrentVelocity() const;

	/// Returns the limit status for the motor. Returns LimitError if the motor isn't connected yet.
	CLSOMS58Motor::Limit atLimit() const;

	/// Returns the power state setting. Returns
	CLSOMS58Motor::PowerState powerState() const;

	/// Returns the encoder calibration slope. Returns 0 if the motor isn't connected yet.
	double encoderCalibrationSlope() const;
	/// Returns the step calibration slope. Returns 0 if the motor isn't connected yet.
	double stepCalibrationSlope() const;
	/// Returns the encoder calibration offset.Returns 0 if the motor isn't connected yet.
	double encoderCalibrationOffset() const;
	/// Returns the step calibration offset. Returns 0 if the motor isn't connected yet.
	double stepCalibrationOffset() const;

	/// Returns the encoder target position. Returns 0.5 if the motor isn't connected yet.  This is because encoder target is actually given in integers.
	double encoderTarget() const;
	/// Returns the encoder movement type. Returns EncoderMovementTypeError if the motor isn't connected yet.
	CLSMAXvMotor::EncoderMovementType encoderMovementType() const;
	/// Returns the pre-deadband value. Returns 0.5 if the motor isn't connected yet.  This is because predeadband is actually given in integers.
	double preDeadBand() const;
	/// Returns the post-deadband value. Returns 0.5 if the motor isn't connected yet.  This is because postdeadband is actually given in integers.
	double postDeadBand() const;
	/// Returns the max retries value. Returns 0.5 if the motor isn't connected yet.  This is because max retries is actually given in integers.
	double maxRetries() const;
	/// Returns the encoder percent approach. Returns -1 if the motor isn't connected yet.
	double encoderPercentApproach() const;
	/// Returns the encoder/step soft ratio. Returns 0 if the motor isn't connected yet.
	double encoderStepSoftRatio() const;

	/// Returns a newly created action to move the motor. This is a convenience function that calls the EGU move action. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createMotorMoveAction(double position);

	/// Returns a newly created action to move this motor in EGU terms. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createEGUMoveAction(double EGUPosition);
	/// Returns a newly created action to change the EGU velocity. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createEGUVelocityAction(double EGUVelocity);
	/// Returns a newly created action to change the EGU base velocity. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createEGUBaseVelocityAction(double EGUBaseVelocity);
	/// Returns a newly created action to change the EGU acceleration. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createEGUAccelerationAction(double EGUAcceleration);
	/// Returns a newly created action to change the EGU offset. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createEGUOffsetAction(double EGUOffset);

	/// Returns a newly created action to change move this motor in step terms. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createStepMoveAction(double stepPosition);
	/// Returns a newly created action to change the step velocity. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createStepVelocityAction(double stepVelocity);
	/// Returns a newly created action to change the step base velocity. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createStepBaseVelocityAction(double stepBaseVelocity);
	/// Returns a newly created action to change the step acceleration. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createStepAccelerationAction(double stepAcceleration);

	/// Returns a newly created action to stop the motor. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createStopAction();

	/// Returns a newly created action to change the encoder calibration slope. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createEncoderCalibrationSlopeAction(double encoderCalibrationSlope);
	/// Returns a newly created action to change the step calibration slope. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createStepCalibrationSlopeAction(double stepCalibrationSlope);
	/// Returns a newly created action to change the encoder calibration offset. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createEncoderCalibrationOffsetAction(double encoderCalibrationAbsoluteOffset);
	/// Returns a newly created action to change the step calibration offset. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createStepCalibrationOffsetAction(double stepCalibrationOffset);

	/// Returns a newly created action to move the motor directly in encoder terms. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createEncoderMoveAction(double encoderPosition);
	/// Returns a newly created action to change the encoder movement type. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createEncoderMovementTypeAction(CLSMAXvMotor::EncoderMovementType encoderMovementType);
	/// Returns a newly created action to change the pre-deadband setting. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createPreDeadBandAction(double preDeadBand);
	/// Returns a newly created action to change the post-deadband setting. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createPostDeadBandAction(double postDeadBand);
	/// Returns a newly created action to change the max retries. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createMaxRetriesAction(double maxRetries);
	/// Returns a newly created action to change the encoder percent approach. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createEncoderPercentApproachAction(double encoderPercentApproach);
	/// Returns a newly created action to change the encoder/step soft ratio. Returns 0 if the control is not connected.
	AMBeamlineActionItem* createEncoderStepSoftRatioAction(double encoderStepSoftRatio);

public slots:
	/// Sets the (EGU) velocity setting for the velocity profile
	void setEGUVelocity(double EGUVelocity);
	/// Sets the (EGU) base velocity setting for the velocity profile
	void setEGUBaseVelocity(double EGUBaseVelocity);
	/// Sets the (EGU) acceleration setting for the velocity profile
	void setEGUAcceleration(double EGUAcceleration);
	/// Sets the EGU offset for the motor
	void setEGUOffset(double EGUOffset);

	/// Sets the step setting.  NOTE: This will move the motor based on the value you give it.  Similar to \code move(double).
	void setStep(double step);
	/// Sets the (step) velocity setting for the velocity profile
	void setStepVelocity(double stepVelocity);
	/// Sets the (step) base velocity setting for the velocity profile
	void setStepBaseVelocity(double stepBaseVelocity);
	/// Sets the (step) acceleration setting for the velocity profile
	void setStepAcceleration(double stepAcceleration);

	/// Tell the motor to stop. If usingKill is true, then the kill PV will be used otherwise the stop PV will be used (Note: For safety, this will send the stop instruction whether we think we're moving or not.)
	virtual bool stop();

	/// Sets the encoder calibration slope
	void setEncoderCalibrationSlope(double encoderCalibrationSlope);
	/// Sets the step calibration slope
	void setStepCalibrationSlope(double stepCalibrationSlope);
	/// Sets the encoder calibration offset
	void setEncoderCalibrationOffset(double encoderCalibrationAbsoluteOffset);
	/// Sets the step calibration offset
	void setStepCalibrationOffset(double stepCalibrationOffset);

	/// Sets the encoder target
	void setEncoderTarget(double encoderTarget);
	/// Sets the encoder movement type
	void setEncoderMovementType(CLSMAXvMotor::EncoderMovementType encoderMovementType);
	/// Sets the pre-deadband value
	void setPreDeadBand(double preDeadBand);
	/// Sets the post-deadband value
	void setPostDeadBand(double postDeadBand);
	/// Sets the max retries value
	void setMaxRetries(double maxRetries);
	/// Sets the encoder percent approach
	void setEncoderPercentApproach(double encoderPercentApproach);
	/// Sets the encoder/step soft ratio
	void setEncoderStepSoftRatio(double encoderStepSoftRatio);

signals:
	/// Emitted when the (EGU) velocity setting changes
	void EGUVelocityChanged(double EGUVelocity);
	/// Emitted when the (EGU) base velocity setting changes
	void EGUBaseVelocityChanged(double EGUBaseVelocity);
	/// Emitted when the (EGU) acceleration setting changes
	void EGUAccelerationChanged(double EGUAcceleration);
	/// Emitted when the (EGU) current velocity reading changes
	void EGUCurrentVelocityChanged(double EGUCurrentVelocity);
	/// Emitted when the EGU offset changes
	void EGUOffsetChanged(double EGUOffset);

	/// Emitted when the step setting changes.
	void stepChanged(double step);
	/// Emitted when the (step) velocity setting changes
	void stepVelocityChanged(double stepVelocity);
	/// Emitted when the (step) base velocity setting changes
	void stepBaseVelocityChanged(double stepBaseVelocity);
	/// Emitted when the (step) acceleration setting changes
	void stepAccelerationChanged(double stepAcceleration);
	/// Emitted when the (step) current velocity reading changes
	void stepCurrentVelocityChanged(double stepCurrentVelocity);

	/// Emitted when the limit state changes
	void atLimitChanged(CLSOMS58Motor::Limit limit);

	/// Emitted when the power state changes
	void powerStatedChanged(CLSMAXvMotor::PowerState powerState);

	/// Emitted when the encoder calibration slope changes
	void encoderCalibrationSlopeChanged(double newCalibration);
	/// Emitted when the step calibration slope changes
	void stepCalibrationSlopeChanged(double newCalibration);
	/// Emitted when the encoder calibration offset changes
	void encoderCalibrationOffsetChanged(double newCalibration);
	/// Emitted when the step calibration offset changes
	void stepCalibrationOffsetChanged(double newCalibration);

	/// Emitted when the encoder target changes
	void encoderTargetChanged(double encoderTarget);
	/// Emitted when the encoder movement type changes
	void encoderMovementTypeChanged(CLSOMS58Motor::EncoderMovementType encoderMovementType);
	/// Emitted when the pre-deadband value changes
	void preDeadBandChanged(double preDeadBand);
	/// Emitted when the post-deadband value changes
	void postDeadBandChanged(double postDeadBand);
	/// Emitted when the max retries value changes
	void maxRetriesChanged(double maxRetries);
	/// Emitted when the encoder percent approach changes
	void encoderPercentApproachChanged(double encoderPercentApproach);
	/// Emitted when the encoder/step soft ratio changes
	void encoderStepSoftRatioChanged(double encoderStepSoftRatio);

protected slots:
	/// Handles changes in the limit controls
	void onLimitsChanged(double value);

	/// Handles changes in the power state control
	void onPowerStateChanged(double value);

	/// Handles change in the encoder movement type
	void onEncoderMovementTypeChanged(double value);

protected:
	/// Read-write control for the (EGU) velocity setting
	AMPVControl *EGUVelocity_;
	/// Read-write control for (EGU) base velocity setting
	AMPVControl *EGUBaseVelocity_;
	/// Read-write control for (EGU) acceleration setting
	AMPVControl *EGUAcceleration_;
	/// Readonly control for actual (EGU) velocity feedback
	AMReadOnlyPVControl *EGUCurrentVelocity_;
	/// Read-write control for the EGU offset
	AMPVControl *EGUOffset_;

	/// Read-write control for the step setting.
	AMPVControl *step_;
	/// Read-write control for the (step) velocity setting
	AMPVControl *stepVelocity_;
	/// Read-write control for (step) base velocity setting
	AMPVControl *stepBaseVelocity_;
	/// Read-write control for (step) acceleration setting
	AMPVControl *stepAcceleration_;
	/// Readonly control for actual (step) velocity feedback
	AMReadOnlyPVControl *stepCurrentVelocity_;

	/// Readonly control for whether or not the motor is at the CW limit
	AMReadOnlyPVControl *cwLimit_;
	/// Readonly contorl for whether or not the motor is at the CCW limit
	AMReadOnlyPVControl *ccwLimit_;

	/// Read-write control for the power state settings
	AMPVControl *powerState_;

	/// Read-write control for the encoder calibration slope
	AMPVControl *encoderCalibrationSlope_;
	/// Read-write control for the step calibration slope
	AMPVControl *stepCalibrationSlope_;
	/// Read-write control for the encoder calibration offset
	AMPVControl *encoderCalibrationOffset_;
	/// Read-write control for the step calibration offset
	AMPVControl *stepCalibrationOffset_;

	/// Read-write control for the encoder target
	AMPVwStatusControl *encoderTarget_;
	/// Read-write control for the encoder movement type
	AMPVControl *encoderMovementType_;
	/// Read-write control for pre-deadband value
	AMPVControl *preDeadBand_;
	/// Read-write control for post-deadband value
	AMPVControl *postDeadBand_;
	/// Read-write control for the max retries value
	AMPVControl *maxRetries_;
	/// Read-write control for the encoder percent approach
	AMPVControl *encoderPercentApproach_;
	/// Read-write control for encoder/step soft ratio
	AMPVControl *encoderStepSoftRatio_;

	/// Holds whether or not this motor was constructed (programmatically) with an encoder
	bool hasEncoder_;

	/// Holds whether or not this motor was constructed (programatically) to use EGU
	bool hasEGU_;
};
*/
#endif // CLSOMS58MOTOR_H
