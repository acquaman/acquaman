#include "SGMGratingTranslationStepControl.h"

#include "beamline/AMPVControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
SGMGratingTranslationStepControl::SGMGratingTranslationStepControl(QObject *parent) :
    AMPseudoMotorControl("Grating Translation Steps", "Steps", parent, "SGM Monochromator Grating Translation Steps")
{
	gratingTranslationStepPV_ = new AMPVwStatusControl("Grating Translation",
	                                                   "SMTR16114I1016:step:fbk",
	                                                   "SMTR16114I1016:step",
	                                                   "SMTR16114I1016:state",
	                                                   "SMTR16114I1016:emergStop",
	                                                   this,
							   1,
	                                                   2.0,
	                                                   new AMControlStatusCheckerStopped(0));

	value_ = 0;
	setpoint_ = 0;
	connected_ = false;
	isMoving_ = false;

	addChildControl(gratingTranslationStepPV_);

	setMinimumValue(8500);
	setMaximumValue(62000);
	setAttemptMoveWhenWithinTolerance(false);
	setTolerance(gratingTranslationStepPV_->tolerance());
	setDisplayPrecision(12);

	updateStates();
}

bool SGMGratingTranslationStepControl::shouldMeasure() const
{
	return true;
}

bool SGMGratingTranslationStepControl::shouldMove() const
{
	return true;
}

bool SGMGratingTranslationStepControl::shouldStop() const
{
	return true;
}

bool SGMGratingTranslationStepControl::canMeasure() const
{
	return gratingTranslationStepPV_->canMove();
}

bool SGMGratingTranslationStepControl::canMove() const
{
	return gratingTranslationStepPV_->canMove();
}

bool SGMGratingTranslationStepControl::canStop() const
{
	return gratingTranslationStepPV_->canMove();
}

void SGMGratingTranslationStepControl::updateConnected()
{
	setConnected(gratingTranslationStepPV_->isConnected());
}

void SGMGratingTranslationStepControl::updateMoving()
{
	if(isConnected()) {

		setIsMoving(gratingTranslationStepPV_->isMoving());
	}
}

void SGMGratingTranslationStepControl::updateValue()
{
	setValue(gratingTranslationStepPV_->value());
}

AMAction3 * SGMGratingTranslationStepControl::createMoveAction(double setpoint)
{
	AMListAction3* moveAction = new AMListAction3(new AMListActionInfo3("Moving Grating Translation",
	                                                                    "Moving Grating Translation"),
	                                              AMListAction3::Sequential);

	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingTranslationStepPV_,
	                                                                 setpoint,
	                                                                 false));

	return moveAction;
}
