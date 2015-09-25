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


#ifndef CLSMAXVMOTOR_H
#define CLSMAXVMOTOR_H

#include "beamline/AMPVControl.h"
#include "actions3/AMAction3.h"

#define CLSMAXVMOTOR_NOT_CONNECTED 638989
#define CLSMAXVMOTOR_CANNOT_CALIBRATE 638990
#define CLSMAXVMOTOR_ALREADY_CALIBRATING 638991
#define CLSMAXVMOTOR_INVALID_CALIBRATION_ACTION 638992
#define CLSMAXVMOTOR_CALIBRATION_FAILED 638993

/// This function object provides the moving check for the CLSMAXvMotors
class CLSMAXvControlStatusChecker : public AMAbstractControlStatusChecker {
public:
	/// Status values will be compare to \c isStoppedValue, and return true if the status value is not equal to isStoppedValue (something that isn't stopped is moving)
	virtual ~CLSMAXvControlStatusChecker();
	CLSMAXvControlStatusChecker() {}

	/// Return true (moving) if the \c statusValue is not 0 (STOPPED) and is not 2 (AT LIMIT) and is not 3 (FORCED STOP) and is not 4 (ERROR)
	virtual bool operator()(quint32 statusValue) { return (statusValue != 0) && (statusValue != 2) && (statusValue != 3) && (statusValue != 4); }
};

/*!
  This class encapsulates many of the standard components found in a CLS VME motor.  Unlike standard AMControls, the CLS VME motors can set other things such as the velocity, acceleration, or directly access the steps
  rather then giving a distance in millimeters.  Therefore, this class is meant to give more control over the motors in the event that you want more control over the actual
  motor.
  */
class CLSMAXvMotor : public AMPVwStatusControl
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

	enum MotorType {
		MotorStepperOnly = 0,		///< A stepper motor
		MotorStepperAndEncoder = 1,	///< A stepper motor with an encoder
		MotorServo = 2,			///< A servo motor
		MotorTypeError = 3		///< There is an error determining the motor type
	};

	enum LimitActiveState {
		LimitActiveLow = 0,	///< Limits are an active low signal
		LimitActiveHigh = 1,	///< Limits are an active high signal
		LimitActiveError = 2	///< There is an error determining the limit active state
	};

	enum EncoderMovementType {
		EncoderMovementOff = 0,			///< Encoder movement approach is off
		EncoderMovementIncreaseOnly = 1,	///< Encoder movement approach is increase only
		EncoderMovementDecreaseOnly = 2,	///< Encoder movement approach is decrease only
		EncoderMovementIncrease = 3,		///< Encoder movement approach is increase
		EncoderMovementDecrease = 4,		///< Encoder movement approach is decrease
		EncoderMovementTypeError = 5		///< There is an error determining the encoder movement type
	};

	/// Constructor
	/*!
	  \param name A unique description of this motor
	  \param baseName The base of the PV name (if the motor status was "SMTR16114I1022:status" then the base is "SMTR16114I1022")
	  \param description A human readable description for this motor
	  \param hasEncoder Should be set to true if the motor has an encoder
	  \param tolerance The tolerance that will be used to determine if moves succeed in getting close enough to be considered a success
	  \param moveStartTimeout How long the motor has to start moving before something is considered amis
	  \param parent QObject parent class
	  */
	virtual ~CLSMAXvMotor();
	CLSMAXvMotor(const QString &name, const QString &baseName, const QString &description, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0, QString pvUnitFieldName=":mm");

	/// Indicates that all process variables for this motor are connected
	virtual bool isConnected() const;

	/// Indicates that this motor \em can currently be calibrated.
	virtual bool canCalibrate() const { return canMove(); }
	/// Indicates that this motor \em should (assuming it's connected) be able to be calibrated.
	virtual bool shouldCalibrate() const { return true; }

	/// Returns the basename of the MAXvMotor PVs
	QString pvBaseName() const;
	/// Returns the pv name of the MAXvMotor read PV
	QString readPVName() const;
	/// Returns the pv name of the MAXvMotor write PV
	QString writePVName() const;
	/// Returns the pv name of the MAXvMotor CW Limit PV (:cw)
	QString CWPVName() const;
	/// Returns the pv name of the MAXvMotor CCW Limit PV (:ccw)
	QString CCWPVName() const;
	/// Returns the pv name of the MAXvMotor status PV (:status)
	QString statusPVName() const;

	/// Returns the (EGU) velocity setting for the velocity profile.  Returns 0 if the motor isn't connected yet.
	double EGUVelocity() const;
	/// Returns the (EGU) base velocity setting for the velocity profile.  Returns 0 if the motor isn't connected yet.
	double EGUBaseVelocity() const;
	/// Returns the (EGU) acceleration setting for the velocity profile.  Returns 0 if the motor isn't connected yet.
	double EGUAcceleration() const;
	/// Returns the (EGU) current velocity of the motor (zero when not moving, presumably non-zero when in motion).  Returns 0 if the motor isn't connected yet.
	double EGUCurrentVelocity() const;
	/// Returns the EGU set position of the motor. Returns 0 if the motor isn't connected yet.
	double EGUSetPosition() const;
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

	/// Returns whether the kill PV is being used for stops
	bool usingKill() const;

	/// Returns the limit status for the motor. Returns LimitError if the motor isn't connected yet.
	CLSMAXvMotor::Limit atLimit() const;

	/// Returns the power state setting. Returns
	CLSMAXvMotor::PowerState powerState() const;
	/// Returns whether or not the invert power is on (returns true when set to invert). Returns false if the motor isn't connected yet.
	bool isPowerInverted() const;

	/// Returns the encoder calibration slope. Returns 0 if the motor isn't connected yet.
	double encoderCalibrationSlope() const;
	/// Returns the step calibration slope. Returns 0 if the motor isn't connected yet.
	double stepCalibrationSlope() const;
	/// Returns the encoder calibration offset.Returns 0 if the motor isn't connected yet.
	double encoderCalibrationOffset() const;
	/// Returns the encoder calibration absolute offset. Returns 0 if the motor isn't connected yet.
	double encoderCalibrationAbsoluteOffset() const;
	/// Returns the step calibration offset. Returns 0 if the motor isn't connected yet.
	double stepCalibrationOffset() const;

	/// Returns the motor type is use. Returns MotorTypeError if the motor isn't connected yet.
	CLSMAXvMotor::MotorType motorType() const;

	/// Returns the limits active state. Returns LimitsActiveError if the motor isn't connected yet.
	CLSMAXvMotor::LimitActiveState limitActiveState() const;
	/// Returns whether or not the limits are disabled (returns true when set to disabled). Returns false if the motor isn't connected yet.
	bool limitDisabled() const;

	/// Returns whether or not closed loop control is on (returns true when set to ON). Returns false if the motors isn't connected yet.
	bool closedLoopEnabled() const;
	/// Returns whether or not the servo PID is on (returns true when set to ON). Returns false if the motor isn't connected yet.
	bool servoPIDEnabled() const;

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

	/// Returns the status PV control, which can be used as the statusTagControl for control editor
	AMReadOnlyPVControl *statusPVControl();
	/// Returns the retries PV control.
	AMReadOnlyPVControl *retries() const { return retries_; }
	/// Returns the max retries PV control.
	AMReadOnlyPVControl *maxRetriesControl() const { return maxRetries_; }
	/// Returns the step setpoint PV control.
	AMReadOnlyPVControl *stepSetpointControl() const { return stepSetpoint_; }
	/// Returns the step-based motor feedback control.
	AMReadOnlyPVControl *stepMotorFeedbackControl() const { return stepMotorFeedback_; }
	/// Returns the power state PV control.
	AMPVControl *powerStateControl() const { return powerState_; }
	/// Returns the clockwise limit status PV control.
	AMReadOnlyPVControl *cwLimitControl() const { return cwLimit_; }
	/// Returns the counter-clockwise limit status PV control.
	AMReadOnlyPVControl *ccwLimitControl() const { return ccwLimit_; }
	/// Returns the EGU velocity PV control.
	AMPVControl* EGUVelocityControl() const { return EGUVelocity_; }
	/// Returns the EGU base velocity PV control.
	AMPVControl* EGUBaseVelocityControl() const { return EGUBaseVelocity_; }
	/// Returns the EGU acceleration PV control.
	AMPVControl* EGUAccelerationControl() const { return EGUAcceleration_; }
	/// Returns the EGU set position PV control.
	AMPVControl* EGUSetPositionControl() const { return EGUSetPosition_; }
	/// Returns the pre-deadband PV control.
	AMPVControl* preDeadBandControl() const { return preDeadBand_; }
	/// Returns the post-deadband PV control.
	AMPVControl* postDeadBandControl() const { return postDeadBand_; }
	/// Returns the encoder feedback PV control.
	AMReadOnlyPVControl *encoderFeedbackControl() const { return encoderFeedback_; }
	/// Returns the encoder movement type PV control.
	AMPVControl* encoderMovementTypeControl() const { return encoderMovementType_; }
	/// Returns the encoder/step soft ratio PV control.
	AMPVControl* encoderStepSoftRatioControl() const { return encoderStepSoftRatio_; }
	/// Returns the encoder calibration slope PV control.
	AMPVControl* encoderCalibrationSlopeControl() const { return encoderCalibrationSlope_; }
	/// Returns the step calibration slope PV control.
	AMPVControl* stepCalibrationSlopeControl() const { return stepCalibrationSlope_; }

	/// Returns a newly created action to move the motor. This is a convenience function that calls the EGU move action. Returns 0 if the control is not connected.
	AMAction3* createMotorMoveAction(double position);

	/// Returns a newly created action to move this motor in EGU terms. Returns 0 if the control is not connected.
	AMAction3 *createEGUMoveAction(double EGUPosition);
	/// Returns a newly created action to change the EGU velocity. Returns 0 if the control is not connected.
	AMAction3 *createEGUVelocityAction(double EGUVelocity);
	/// Returns a newly created action to change the EGU base velocity. Returns 0 if the control is not connected.
	AMAction3 *createEGUBaseVelocityAction(double EGUBaseVelocity);
	/// Returns a newly created action to change the EGU acceleration. Returns 0 if the control is not connected.
	AMAction3 *createEGUAccelerationAction(double EGUAcceleration);
	/// Returns a newly created action to change the EGU Set Position. Returns 0 if the control is not connected.
	AMAction3 *createEGUSetPositionAction(double EGUOffset);
	/// Returns a newly created action to change the EGU offset. Returns 0 if the control is not connected.
	AMAction3 *createEGUOffsetAction(double EGUOffset);

	/// Returns a newly created action to change move this motor in step terms. Returns 0 if the control is not connected.
	AMAction3 *createStepMoveAction(double stepPosition);
	/// Returns a newly created action to change the step velocity. Returns 0 if the control is not connected.
	AMAction3 *createStepVelocityAction(double stepVelocity);
	/// Returns a newly created action to change the step base velocity. Returns 0 if the control is not connected.
	AMAction3 *createStepBaseVelocityAction(double stepBaseVelocity);
	/// Returns a newly created action to change the step acceleration. Returns 0 if the control is not connected.
	AMAction3 *createStepAccelerationAction(double stepAcceleration);

	/// Returns a newly created action to stop the motor. Returns 0 if the control is not connected.
	AMAction3 *createStopAction();

	/// Returns a newly created action to change the encoder calibration slope. Returns 0 if the control is not connected.
	AMAction3 *createEncoderCalibrationSlopeAction(double encoderCalibrationSlope);
	/// Returns a newly created action to change the step calibration slope. Returns 0 if the control is not connected.
	AMAction3 *createStepCalibrationSlopeAction(double stepCalibrationSlope);
	/// Returns a newly created action to change the encoder calibration offset. Returns 0 if the control is not connected.
	AMAction3 *createEncoderCalibrationOffsetAction(double encoderCalibrationAbsoluteOffset);
	/// Returns a newly created action to change the encoder calibration absolute offset. Returns 0 if the control is not connected.
	AMAction3 *createEncoderCalibrationAbsoluteOffsetAction(double encoderCalibrationAbsoluteOffset);
	/// Returns a newly created action to change the step calibration offset. Returns 0 if the control is not connected.
	AMAction3 *createStepCalibrationOffsetAction(double stepCalibrationOffset);

	/// Returns a newly created action to change the motor type. Returns 0 if the control is not connected.
	AMAction3 *createMotorTypeAction(CLSMAXvMotor::MotorType motorType);

	/// Returns a newly created action to change the limit active state. Returns 0 if the control is not connected.
	AMAction3 *createLimitActiveStateAction(CLSMAXvMotor::LimitActiveState limitActiveState);
	/// Returns a newly created action to change the limit disabled state. Returns 0 if the control is not connected.
	AMAction3 *createLimitDisabledAction(bool limitDisabled);
	/// Returns a newly created action to change the closed loop enabled mode. Returns 0 if the control is not connected.
	AMAction3 *createClosedLoopEnabledAction(bool closedLoopEnabled);
	/// Returns a newly created action to change the servo PID enabled mode. Returns 0 if the control is not connected.
	AMAction3 *createServoPIDEnabledAction(bool servoPIDEnabled);

	/// Returns a newly created action to move the motor directly in encoder terms. Returns 0 if the control is not connected.
	AMAction3 *createEncoderMoveAction(double encoderPosition);
	/// Returns a newly created action to change the encoder movement type. Returns 0 if the control is not connected.
	AMAction3 *createEncoderMovementTypeAction(CLSMAXvMotor::EncoderMovementType encoderMovementType);
	/// Returns a newly created action to change the pre-deadband setting. Returns 0 if the control is not connected.
	AMAction3 *createPreDeadBandAction(double preDeadBand);
	/// Returns a newly created action to change the post-deadband setting. Returns 0 if the control is not connected.
	AMAction3 *createPostDeadBandAction(double postDeadBand);
	/// Returns a newly created action to change the max retries. Returns 0 if the control is not connected.
	AMAction3 *createMaxRetriesAction(double maxRetries);
	/// Returns a newly created action to change the encoder percent approach. Returns 0 if the control is not connected.
	AMAction3 *createEncoderPercentApproachAction(double encoderPercentApproach);
	/// Returns a newly created action to change the encoder/step soft ratio. Returns 0 if the control is not connected.
	AMAction3 *createEncoderStepSoftRatioAction(double encoderStepSoftRatio);

	/// Returns a newly created action to change the power state. Returns 0 if the control is not connected.
	AMAction3 *createPowerAction(CLSMAXvMotor::PowerState newState);

	/// Returns a newly created action to alert when CCW is at limit
	AMAction3 *createCCWLimitWaitAction(CLSMAXvMotor::Limit ccwLimitState);

	/// Returns a newly created action to alert when CW is at limit
	AMAction3 *createCWLimitWaitAction(CLSMAXvMotor::Limit ccwLimitState);

	/// Returns a newly created action to calibrate this motor. Moves this motor to the oldPosition and sets it as the newPosition.
	AMAction3 *createCalibrationAction(double oldPosition, double newPosition);

public slots:
	/// Calibrates the motor. Moves to old position and the sets that position to the new position.
	virtual FailureExplanation calibrate(double oldValue, double newValue);

	/// Sets the (EGU) velocity setting for the velocity profile
	void setEGUVelocity(double EGUVelocity);
	/// Sets the (EGU) base velocity setting for the velocity profile
	void setEGUBaseVelocity(double EGUBaseVelocity);
	/// Sets the (EGU) acceleration setting for the velocity profile
	void setEGUAcceleration(double EGUAcceleration);
	/// Sets the EGU offset for the motor
	void setEGUSetPosition(double EGUSetPosition);
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

	/// Sets whether the kill PV is being used for stops (if false, then the rEGUlar stop PV is used)
	void setUsingKill(bool usingKill);

	/// Tell the motor to stop. If usingKill is true, then the kill PV will be used otherwise the stop PV will be used (Note: For safety, this will send the stop instruction whether we think we're moving or not.)
	virtual bool stop();

	/// Sets the encoder calibration slope
	void setEncoderCalibrationSlope(double encoderCalibrationSlope);
	/// Sets the step calibration slope
	void setStepCalibrationSlope(double stepCalibrationSlope);
	/// Sets the encoder calibration offset
	void setEncoderCalibrationOffset(double encoderCalibrationAbsoluteOffset);
	/// Sets the encoder calibration absolute offset.
	void setEncoderCalibrationAbsoluteOffset(double encoderCalibrationAbsoluteOffset);
	/// Sets the step calibration offset
	void setStepCalibrationOffset(double stepCalibrationOffset);

	/// Sets the motor type to use
	void setMotorType(CLSMAXvMotor::MotorType motorType);

	/// Sets the limit active state
	void setLimitActiveState(CLSMAXvMotor::LimitActiveState limitActiveState);
	/// Sets the limit disabled state
	void setLimitDisabled(bool disabled);

	/// Sets the closed loop enabled
	void setClosedLoopEnabled(bool closedLoopEnabled);
	/// Sets the servo PID enabled
	void setServoPIDEnabled(bool servoPIDEnabled);

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
	/// Start a move to the value setpoint (reimplemented)
	virtual FailureExplanation move(double setpoint);

signals:
	/// Emitted when the (EGU) velocity setting changes
	void EGUVelocityChanged(double EGUVelocity);
	/// Emitted when the (EGU) base velocity setting changes
	void EGUBaseVelocityChanged(double EGUBaseVelocity);
	/// Emitted when the (EGU) acceleration setting changes
	void EGUAccelerationChanged(double EGUAcceleration);
	/// Emitted when the (EGU) current velocity reading changes
	void EGUCurrentVelocityChanged(double EGUCurrentVelocity);
	/// Emitted when the EGU set position changes
	void EGUSetPositionChanged(double EGUSetPosition);
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
	void atLimitChanged(CLSMAXvMotor::Limit limit);

	/// Emitted when the power state changes
	void powerStatedChanged(CLSMAXvMotor::PowerState powerState);
	/// Emitted when the power inverted state changes
	void powerInvertedChanged(bool isPowerInverted);

	/// Emitted when the encoder calibration slope changes
	void encoderCalibrationSlopeChanged(double newCalibration);
	/// Emitted when the step calibration slope changes
	void stepCalibrationSlopeChanged(double newCalibration);
	/// Emitted when the encoder calibration offset changes
	void encoderCalibrationOffsetChanged(double newCalibration);
	/// Emitted when the encoder calibration absolute offset changes.
	void encoderCalibrationAbsoluteOffsetChanged(double newOffset);
	/// Emitted when the step calibration offset changes
	void stepCalibrationOffsetChanged(double newCalibration);

	/// Emitted when the motor type in use changes
	void motorTypeChanged(CLSMAXvMotor::MotorType motorType);

	/// Emitted when the limits active state changes
	void limitActiveStateChanged(CLSMAXvMotor::LimitActiveState limitActiveState);
	/// Emitted when the limits disabled state changes
	void limitDisabledChanged(bool disabled);

	/// Emitted when the closed loop state changes
	void closedLoopEnabledChanged(bool closedLoopEnabled);
	/// Emitted when the servo PID state changes
	void servoPIDEnabledChanged(bool servoPIDEnabled);

	/// Emitted when the encoder target changes
	void encoderTargetChanged(double encoderTarget);
	/// Emitted when the encoder movement type changes
	void encoderMovementTypeChanged(CLSMAXvMotor::EncoderMovementType encoderMovementType);
	/// Emitted when the pre-deadband value changes
	void preDeadBandChanged(double preDeadBand);
	/// Emitted when the post-deadband value changes
	void postDeadBandChanged(double postDeadBand);
	/// Emitted when the max retries value changes
	void maxRetriesChanged(double maxRetries);
	/// Emitted when the actual retries value changes
	void actualRetriesChanged(double actualRetries);
	/// Emitted when the encoder percent approach changes
	void encoderPercentApproachChanged(double encoderPercentApproach);
	/// Emitted when the encoder/step soft ratio changes
	void encoderStepSoftRatioChanged(double encoderStepSoftRatio);

protected slots:
	/// Handles changes in the limit controls
	void onLimitsChanged(double value);

	/// Handles changes in the power state control
	void onPowerStateChanged(double value);

	/// Handles changes in the power inverted control
	void onPowerInvertedChanged(double value);

	/// Handles changes in the motor type control
	void onMotorTypeChanged(double value);

	/// Handles changes in the limit active state
	void onLimitActiveStateChanged(double value);

	/// Hanldes changes in the limit disabled state
	void onLimitDisabledChanged(double value);

	/// Handles changes in the closed loop state
	void onClosedLoopEnabledChanged(double value);

	/// Handles changes in the servo PID state
	void onServoPIDEnabledChanged(double value);

	/// Handles change in the encoder movement type
	void onEncoderMovementTypeChanged(double value);

protected:
	/// the baseName of the MAXvMotor PVs
	QString pvBaseName_;

	/// Read-only control for the step setpoint.
	AMReadOnlyPVControl *stepSetpoint_;

	/// Read-only control for the motor feedback, based on steps.
	AMReadOnlyPVControl *stepMotorFeedback_;

	/// Read-write control for the (EGU) velocity setting
	AMPVControl *EGUVelocity_;
	/// Read-write control for (EGU) base velocity setting
	AMPVControl *EGUBaseVelocity_;
	/// Read-write control for (EGU) acceleration setting
	AMPVControl *EGUAcceleration_;
	/// Readonly control for actual (EGU) velocity feedback
	AMReadOnlyPVControl *EGUCurrentVelocity_;
	/// Read-write control for the EGU set position
	AMPVControl *EGUSetPosition_;
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

	/// PV for the "kill" functionality
	AMProcessVariable *killPV_;

	/// Readonly control for whether or not the motor is at the CW limit
	AMReadOnlyPVControl *cwLimit_;
	/// Readonly contorl for whether or not the motor is at the CCW limit
	AMReadOnlyPVControl *ccwLimit_;

	/// Readonly contorl for PV status, which can be used as the statusTagControl for control editor
	AMReadOnlyPVControl *statusPVControl_;

	/// Read-write control for the power state settings
	AMPVControl *powerState_;
	/// Read-write control for the invert power setting
	AMPVControl *powerInverted_;

	/// Read-write control for the encoder calibration slope
	AMPVControl *encoderCalibrationSlope_;
	/// Read-write control for the step calibration slope
	AMPVControl *stepCalibrationSlope_;
	/// Read-write control for the encoder calibration offset
	AMPVControl *encoderCalibrationOffset_;
	/// Read-write control for the encoder calibration absolute offset.
	AMPVControl *encoderCalibrationAbsoluteOffset_;
	/// Read-write control for the step calibration offset
	AMPVControl *stepCalibrationOffset_;

	/// Read-write control for the motor type
	AMPVControl *motorType_;

	/// Read-write control for the limit's active state
	AMPVControl *limitActiveState_;
	/// Read-write control for whether or not the limits are enabled
	AMPVControl *limitDisabled_;

	/// Read-write control for closed loop mode
	AMPVControl *closedLoopEnabled_;
	/// Read-write control for servo PID
	AMPVControl *servoPIDEnabled_;

	/// Read-write control for the encoder target
	AMPVwStatusControl *encoderTarget_;
	/// Read-only control for the encoder feedback.
	AMReadOnlyPVControl *encoderFeedback_;
	/// Read-write control for the encoder movement type
	AMPVControl *encoderMovementType_;
	/// Read-write control for pre-deadband value
	AMPVControl *preDeadBand_;
	/// Read-write control for post-deadband value
	AMPVControl *postDeadBand_;
	/// Read-only control for the number of retries.
	AMReadOnlyPVControl *retries_;
	/// Read-write control for the max retries value
	AMPVControl *maxRetries_;
	/// Read-write control for the encoder percent approach
	AMPVControl *encoderPercentApproach_;
	/// Read-write control for encoder/step soft ratio
	AMPVControl *encoderStepSoftRatio_;

	/// Toggle boolean for which stop PV to use (kill stops the motor IMMEDIATELY, while stop follows the velocity profile)
	bool usingKill_;

	/// Holds whether or not this motor was constructed (programmatically) with an encoder
	bool hasEncoder_;
};

#endif // CLSMAXVMOTOR_H
