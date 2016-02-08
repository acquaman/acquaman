#include "SGMGratingAngleControl.h"

#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "util/AMTrapezoidVelocityProfile.h"
#include "util/AMRange.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMChangeToleranceAction.h"
#include "beamline/AMControlSet.h"
SGMGratingAngleControl::SGMGratingAngleControl(QObject *parent) :
    AMPseudoMotorControl("Grating Angle Encoder", "Count", parent, "SGM Monochromator Grating Angle")
{
	encoderControl_ = new AMPVwStatusControl("Grating Angle Encoder",
	                                         "SMTR16114I1002:enc:fbk",
	                                         "SMTR16114I1002:encTarget",
	                                         "SMTR16114I1002:status",
	                                         "SMTR16114I1002:stop",
	                                         this,
	                                         5,
	                                         2.0,
	                                         new CLSMAXvControlStatusChecker(),
	                                         1);

	stepMotorControl_ = new AMPVwStatusControl("Grating Angle Step",
	                                           "SMTR16114I1002:step:sp",
	                                           "SMTR16114I1002:step",
	                                           "SMTR16114I1002:status",
	                                           "SMTR16114I1002:stop",
	                                           this,
	                                           5,
	                                           2.0,
	                                           new CLSMAXvControlStatusChecker());

	stepVelocityControl_ = new AMPVControl("Grating Angle Step Velocity",
	                                       "SMTR16114I1002:velo:sp",
	                                       "SMTR16114I1002:velo",
	                                       QString(),
	                                       this,
	                                       1);

	stepAccelerationControl_ = new AMPVControl("Grating Angle Step Acceleration",
	                                           "SMTR16114I1002:accel:sp",
	                                           "SMTR16114I1002:accel",
	                                           QString(),
	                                           this,
	                                           0.1);

	stepsPerEncoderCountControl_ = new AMSinglePVControl("Grating Angle Steps Per Encoder Pulse",
	                                                     "SMTR16114I1002:softRatio",
	                                                     this,
	                                                     0.1);

	movementTypeControl_ = new AMSinglePVControl("Grating Angle Move Type",
	                                             "SMTR16114I1002:encMoveType",
	                                             this,
	                                             0.5);

	AMControlSet* allControls = new AMControlSet(this);
	allControls->addControl(encoderControl_);
	allControls->addControl(stepMotorControl_);
	allControls->addControl(stepVelocityControl_);
	allControls->addControl(stepAccelerationControl_);
	allControls->addControl(stepsPerEncoderCountControl_);
	allControls->addControl(movementTypeControl_);
	connect(allControls, SIGNAL(connected(bool)), this, SLOT(onControlSetConnectionChanged(bool)));

	addChildControl(encoderControl_);
	addChildControl(stepMotorControl_);
	setMinimumValue(encoderControl_->minimumValue());
	setMaximumValue(encoderControl_->maximumValue());
	setTolerance(encoderControl_->tolerance());
	setAttemptMoveWhenWithinTolerance(true);

	updateStates();
}

bool SGMGratingAngleControl::shouldMeasure() const
{
	return encoderControl_->canMeasure() &&
	        stepMotorControl_->canMeasure();
}

bool SGMGratingAngleControl::shouldMove() const
{
	return encoderControl_->shouldMove() &&
	        stepMotorControl_->shouldMove();
}

bool SGMGratingAngleControl::shouldStop() const
{
	return encoderControl_->shouldStop() &&
	        stepMotorControl_->shouldStop();
}

bool SGMGratingAngleControl::canMeasure() const
{
	return encoderControl_->canMeasure() &&
	        stepMotorControl_->canMeasure();
}

bool SGMGratingAngleControl::canMove() const
{
	return encoderControl_->canMove() &&
	        stepMotorControl_->canMove();
}

bool SGMGratingAngleControl::canStop() const
{
	return encoderControl_->canStop() &&
	        stepMotorControl_->canStop();
}

bool SGMGratingAngleControl::isClosedLoop() const
{
	// 0 = None (ie not closed loop)
	return !movementTypeControl_->withinTolerance(0);
}

AMControl * SGMGratingAngleControl::stepVelocityControl() const
{
	return stepVelocityControl_;
}

AMControl * SGMGratingAngleControl::stepAccelerationControl() const
{
	return stepAccelerationControl_;
}

AMControl * SGMGratingAngleControl::movementTypeControl() const
{
	return movementTypeControl_;
}

double SGMGratingAngleControl::stepsPerEncoderCount() const
{
	return stepsPerEncoderCountControl_->value();
}

AMAction3 * SGMGratingAngleControl::createDefaultsAction()
{
	AMListAction3* returnAction = new AMListAction3(new AMListActionInfo3("Set Grating Angle Defaults",
	                                                                      "Set Grating Angle Defaults"),
	                                                AMListAction3::Sequential);

	returnAction->addSubAction(new AMChangeToleranceAction(new AMChangeToleranceActionInfo(toInfo(), 5),this));

	AMListAction3* moveAction = new AMListAction3(new AMListActionInfo3("Set Values",
	                                                                    "Set Values"),
	                                              AMListAction3::Parallel);

	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(stepVelocityControl_, DEFAULT_GRATING_ANGLE_VELOCITY));
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(stepAccelerationControl_, DEFAULT_GRATING_ANGLE_ACCELERATION));
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(movementTypeControl_, DEFAULT_GRATING_ANGLE_MOVE_TYPE));

	AMListAction3* waitAction = new AMListAction3(new AMListActionInfo3("Wait for Values",
	                                                                    "Wait for Values"),
	                                              AMListAction3::Parallel);

	waitAction->addSubAction(AMActionSupport::buildControlWaitAction(stepVelocityControl_, DEFAULT_GRATING_ANGLE_VELOCITY, 10, AMControlWaitActionInfo::MatchWithinTolerance));
	waitAction->addSubAction(AMActionSupport::buildControlWaitAction(stepAccelerationControl_, DEFAULT_GRATING_ANGLE_ACCELERATION, 10, AMControlWaitActionInfo::MatchWithinTolerance));
	waitAction->addSubAction(AMActionSupport::buildControlWaitAction(movementTypeControl_, DEFAULT_GRATING_ANGLE_MOVE_TYPE, 10, AMControlWaitActionInfo::MatchWithinTolerance));

	returnAction->addSubAction(moveAction);
	returnAction->addSubAction(waitAction);

	return returnAction;
}

AMRange SGMGratingAngleControl::timeBoundsForEnergyMove(double startEnergy,
                                                        double endEnergy,
                                                        SGMGratingSupport::GratingTranslation currentTranslation)
{

	if(currentTranslation == SGMGratingSupport::UnknownGrating
	        || startEnergy < 0
	        || endEnergy < 0
	        || qAbs(endEnergy - startEnergy) < 1) {

		return AMRange();
	}

	double stepStartPos = SGMGratingSupport::gratingAngleFromEnergy(currentTranslation, startEnergy) * stepsPerEncoderCount();
	double stepEndPos = SGMGratingSupport::gratingAngleFromEnergy(currentTranslation, endEnergy) * stepsPerEncoderCount();


	double maxTime = AMTrapezoidVelocityProfile::timeForVelocity(stepStartPos,
	                                                             stepEndPos,
	                                                             stepAccelerationControl_->value(),
	                                                             SGM_GRATING_MIN_CONTINUOUS_STEP_VELOCITY);

	double minTime = AMTrapezoidVelocityProfile::timeForVelocity(stepStartPos,
	                                                             stepEndPos,
	                                                             stepAccelerationControl_->value(),
	                                                             SGM_GRATING_MAX_CONTINUOUS_STEP_VELOCITY);

	return AMRange(minTime, maxTime);

}

void SGMGratingAngleControl::updateConnected()
{
	setConnected(encoderControl_->isConnected() &&
	             stepMotorControl_->isConnected() &&
	             stepVelocityControl_->isConnected() &&
	             stepAccelerationControl_->isConnected() &&
	             stepsPerEncoderCountControl_->isConnected() &&
	             movementTypeControl_->isConnected());
}

void SGMGratingAngleControl::updateValue()
{
	setValue(encoderControl_->value());
}

void SGMGratingAngleControl::updateMoving()
{
	setIsMoving(encoderControl_->isMoving());
}

void SGMGratingAngleControl::onControlSetConnectionChanged(bool)
{
	updateConnected();
}

AMAction3 * SGMGratingAngleControl::createMoveAction(double setpoint)
{
	AMListAction3* moveAction = new AMListAction3(new AMListActionInfo3("Moving Grating Angle",
	                                                                    "Moving Grating Angle"),
	                                              AMListAction3::Sequential);


	if(isClosedLoop()) {

		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(encoderControl_, setpoint));
	} else {

		// Get distance to move in terms of the encoder
		double deltaDistanceEncoder = setpoint - value();

		// Convert into steps
		double deltaDistanceSteps = deltaDistanceEncoder * stepsPerEncoderCount();

		// Get current step position
		double currentStepPosition = stepMotorControl_->value();
		// Get the setpoint in terms of steps
		double stepSetpoint = currentStepPosition + deltaDistanceSteps;

		// Up our tolerance
		moveAction->addSubAction(new AMChangeToleranceAction(new AMChangeToleranceActionInfo(toInfo(), 400),this));

		// Do the move
		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(stepMotorControl_, stepSetpoint));

	}

	return moveAction;
}








