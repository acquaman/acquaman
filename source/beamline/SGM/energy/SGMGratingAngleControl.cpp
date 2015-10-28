#include "SGMGratingAngleControl.h"

#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
SGMGratingAngleControl::SGMGratingAngleControl(QObject *parent) :
	AMPseudoMotorControl("Grating Angle Encoder", "Count", parent, "SGM Monochromator Grating Angle")
{
	isClosedLoop_ = true;

	gratingAngleEncoderControl_ = new AMPVwStatusControl("Grating Angle Encoder",
														 "SMTR16114I1002:enc:fbk",
														 "SMTR16114I1002:encTarget",
														 "SMTR16114I1002:status",
														 "SMTR16114I1002:stop",
														 this,
														 5,
														 2.0,
														 new CLSMAXvControlStatusChecker(),
														 1);

	gratingAngleStepControl_ = new AMPVwStatusControl("Grating Angle Step",
													  "SMTR16114I1002:step:sp",
													  "SMTR16114I1002:step",
													  "SMTR16114I1002:status",
													  "SMTR16114I1002:stop",
													  this,
													  5,
													  2.0,
													  new CLSMAXvControlStatusChecker());

	gratingAngleStepVelocityControl_ = new AMPVControl("Grating Angle Step Velocity",
													   "SMTR16114I1002:velo:sp",
													   "SMTR16114I1002:velo",
													   QString(),
													   this,
													   0.1);

	gratingAngleStepAccelerationControl_ = new AMPVControl("Grating Angle Step Acceleration",
														   "SMTR16114I1002:accel:sp",
														   "SMTR16114I1002:accel",
														   QString(),
														   this,
														   0.1);

	gratingAngleStepsPerEncoderCountControl_ = new AMSinglePVControl("Grating Angle Steps Per Encoder Pulse",
																	 "SMTR16114I1002:softRatio",
																	 this,
																	 0.1);

	gratingAngleMoveType_ = new AMSinglePVControl("Grating Angle Move Type",
												  "SMTR16114I1002:encMoveType",
												  this,
												  0.5);

	addChildControl(gratingAngleEncoderControl_);
	addChildControl(gratingAngleStepControl_);
}

bool SGMGratingAngleControl::shouldMeasure() const
{
	return gratingAngleEncoderControl_->canMeasure() &&
			gratingAngleStepControl_->canMeasure();
}

bool SGMGratingAngleControl::shouldMove() const
{
	return gratingAngleEncoderControl_->shouldMove() &&
			gratingAngleStepControl_->shouldMove();
}

bool SGMGratingAngleControl::shouldStop() const
{
	return gratingAngleEncoderControl_->shouldStop() &&
			gratingAngleStepControl_->shouldStop();
}

bool SGMGratingAngleControl::canMeasure() const
{
	return gratingAngleEncoderControl_->canMeasure() &&
			gratingAngleStepControl_->canMeasure();
}

bool SGMGratingAngleControl::canMove() const
{
	return gratingAngleEncoderControl_->canMove() &&
			gratingAngleStepControl_->canMove();
}

bool SGMGratingAngleControl::canStop() const
{
	return gratingAngleEncoderControl_->canStop() &&
			gratingAngleStepControl_->canStop();
}

void SGMGratingAngleControl::updateConnected()
{
	return gratingAngleEncoderControl_->isConnected() &&
			gratingAngleStepControl_->isConnected() &&
			gratingAngleStepVelocityControl_->isConnected() &&
			gratingAngleStepAccelerationControl_->isConnected() &&
			gratingAngleStepsPerEncoderCountControl_->isConnected();
}

void SGMGratingAngleControl::updateValue()
{
	if(isClosedLoop_) {
		setValue(gratingAngleEncoderControl_->value());
	} else {
		setValue(gratingAngleStepControl_->value());
	}
}

void SGMGratingAngleControl::updateMoving()
{
	setIsMoving(gratingAngleEncoderControl_->isMoving());
}

AMAction3 * SGMGratingAngleControl::createMoveAction(double setpoint)
{
	AMListAction3* moveAction = new AMListAction3(new AMListActionInfo3("Moving Grating Angle",
																		"Moving Grating Angle"),
												  AMListAction3::Sequential);


	if(isClosedLoop_) {

		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleEncoderControl_, setpoint, false, false));
		moveAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleEncoderControl_, setpoint, 20, AMControlWaitActionInfo::MatchWithinTolerance));
	} else {

	}

	return moveAction;
}
