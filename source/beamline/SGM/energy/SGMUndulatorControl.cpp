#include "SGMUndulatorControl.h"

#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/AMControlSet.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMControlWaitAction.h"
SGMUndulatorControl::SGMUndulatorControl(QObject *parent) :
    AMPseudoMotorControl("Undulator", "mm", parent, "SGM Undulator Gap")
{

	encoderControl_ = new AMPVwStatusControl("Undulator Position",
	                                         "UND1411-01:gap:mm:fbk",
	                                         "UND1411-01:gap:mm",
	                                         "UND1411-01:moveStatus",
	                                         "UND1411-01:stop",
	                                         this,
						 0.5);

	stepControl_ = new AMPVwStatusControl("Undulator Step",
	                                      "SMTR1411-01:step:sp",
	                                      "SMTR1411-01:step",
	                                      "SMTR1411-01:status",
	                                      "SMTR1411-01:stop",
	                                      this,
	                                      10,
	                                      20,
	                                      new CLSMAXvControlStatusChecker());

	stepVelocityControl_ = new AMPVControl("Undulator Velocity",
	                                       "SMTR1411-01:velo:sp",
	                                       "SMTR1411-01:velo",
	                                       QString(),
	                                       this,
	                                       1);

	stepAccelerationControl_ = new AMPVControl("Undulator Acceleration",
	                                           "SMTR1411-01:accel:sp",
	                                           "SMTR1411-01:accel",
	                                           QString(),
	                                           this,
	                                           1);

	addChildControl(encoderControl_);
	addChildControl(stepControl_);

	AMControlSet* allControls = new AMControlSet(this);

	allControls->addControl(encoderControl_);
	allControls->addControl(stepControl_);
	allControls->addControl(stepVelocityControl_);
	allControls->addControl(stepAccelerationControl_);
	connect(allControls, SIGNAL(connected(bool)),
	        this, SLOT(onControlSetConnectionChanged(bool)));

	setAttemptMoveWhenWithinTolerance(true);
        setTolerance(encoderControl_->tolerance());
	setMinimumValue(encoderControl_->minimumValue());
	setMaximumValue(encoderControl_->maximumValue());

}

bool SGMUndulatorControl::shouldMeasure() const
{
	return encoderControl_->canMeasure() &&
	        stepControl_->canMeasure();
}

bool SGMUndulatorControl::shouldMove() const
{
	return encoderControl_->shouldMove() &&
	        stepControl_->shouldMove();
}

bool SGMUndulatorControl::shouldStop() const
{
	return encoderControl_->shouldStop() &&
	        stepControl_->shouldStop();
}

bool SGMUndulatorControl::canMeasure() const
{
	return encoderControl_->canMeasure() &&
	        stepControl_->canMeasure();
}

bool SGMUndulatorControl::canMove() const
{
	return encoderControl_->canMove() &&
	        stepControl_->canMove();
}

bool SGMUndulatorControl::canStop() const
{
	return encoderControl_->canStop() &&
	        stepControl_->canStop();
}

AMControl * SGMUndulatorControl::stepVelocityControl() const
{
	return stepVelocityControl_;
}

AMControl * SGMUndulatorControl::stepAccelerationControl() const
{
	return stepAccelerationControl_;
}

AMControl * SGMUndulatorControl::stepControl() const
{
	return stepControl_;
}

void SGMUndulatorControl::updateConnected()
{
	setConnected(encoderControl_->isConnected() &&
	             stepControl_->isConnected() &&
	             stepVelocityControl_->isConnected() &&
	             stepAccelerationControl_->isConnected());
}

void SGMUndulatorControl::updateValue()
{
	setValue(encoderControl_->value());
}

void SGMUndulatorControl::updateMoving()
{
	setIsMoving(encoderControl_->isMoving() ||
	            stepControl_->isMoving());
}

void SGMUndulatorControl::onControlSetConnectionChanged(bool)
{
	updateConnected();
}

AMAction3 * SGMUndulatorControl::createMoveAction(double setpoint)
{
	AMListAction3* moveAction = new AMListAction3(new AMListActionInfo3("Moving Undulator",
	                                                                    "Moving Undulator"),
	                                              AMListAction3::Sequential);

	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(encoderControl_,
	                                                                 setpoint));

	return moveAction;
}

