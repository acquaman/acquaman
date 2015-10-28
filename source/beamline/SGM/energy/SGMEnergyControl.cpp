#include "SGMEnergyControl.h"

#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "SGMGratingTranslationStepControl.h"
#include "util/AMErrorMonitor.h"
#include "SGMEnergyTrajectory.h"

SGMEnergyControl::SGMEnergyControl(SGMUndulatorSupport::UndulatorHarmonic startingUndulatorHarmonic,
                                   QObject *parent) :
    AMPseudoMotorControl("Energy", "eV", parent, "SGM Monochromator Energy")
{
    energyPositionController_ = 0;
    startingUndulatorHarmonic_ = startingUndulatorHarmonic;

    gratingAngleControl_ = new AMPVwStatusControl("Grating Angle",
                                                  "SMTR16114I1002:enc:fbk",
                                                  "SMTR16114I1002:encTarget",
                                                  "SMTR16114I1002:status",
                                                  "SMTR16114I1002:stop",
                                                  this,
                                                  5,
                                                  2.0,
                                                  new CLSMAXvControlStatusChecker(),
                                                  1);

	gratingTranslationControl_ = new SGMGratingTranslationStepControl(this);

    undulatorPositionControl_ = new AMPVwStatusControl("Undulator Position",
                                                       "UND1411-01:gap:mm:fbk",
                                                       "UND1411-01:gap:mm",
                                                       "UND1411-01:moveStatus",
                                                       "SMTR1411-01:stop",
                                                       this,
                                                       0.1);

    exitSlitPositionControl_ = new AMPVwStatusControl("Exit Slit Position",
                                                      "PSL16114I1003:Y:mm:fbk",
                                                      "PSL16114I1003:Y:mm:encsp",
                                                      "SMTR16114I1003:status",
                                                      "SMTR16114I1003:stop",
                                                      this,
                                                      0.1,
                                                      2.0,
                                                      new AMControlStatusCheckerDefault(1));

	gratingAngleBaseVelocityControl_ = new AMPVControl("Grating Angle Base Velocity",
													   "SMTR16114I1002:veloBase",
													   "SMTR16114I1002:veloBase",
													   QString(),
													   this,
													   1);

	gratingAngleTargetVelocityControl_ = new AMPVControl("Grating Angle Target Velocity",
														 "SMTR16114I1002:velo",
														 "SMTR16114I1002:velo",
														 QString(),
														 this,
														 1);

	gratingAngleAccelerationControl_ = new AMPVControl("Grating Angle Acceleration",
													   "SMTR16114I1002:accel",
													   "SMTR16114I1002:accel",
													   QString(),
													   this,
													   1);

	undulatorStepControl_ = new AMPVControl("Undulator Step",
											"SMTR1411-01:step:sp",
											"SMTR1411-01:step",
											"SMTR1411-01:stop",
											this,
											20,
											20);

	undulatorStepVelocityControl_ = new AMPVControl("Undulator Velocity",
													"SMTR1411-01:velo:sp",
													"SMTR1411-01:velo",
													QString(),
													this,
													1);

	addChildControl(gratingAngleControl_);
	addChildControl(gratingTranslationControl_);
	addChildControl(undulatorPositionControl_);
	addChildControl(exitSlitPositionControl_);
	addChildControl(gratingAngleBaseVelocityControl_);
	addChildControl(gratingAngleTargetVelocityControl_);
	addChildControl(gratingAngleAccelerationControl_);
	addChildControl(undulatorStepControl_);
	addChildControl(undulatorStepVelocityControl_);

    setMinimumValue(200);
    setMaximumValue(3000);
    setTolerance(0.5);
    setDisplayPrecision(5);

}

bool SGMEnergyControl::shouldMeasure() const
{
    return true;
}

bool SGMEnergyControl::shouldMove() const
{
    return true;
}

bool SGMEnergyControl::shouldStop() const
{
    return true;
}

bool SGMEnergyControl::canMeasure() const
{
    return isConnected() && shouldMeasure();
}

bool SGMEnergyControl::canMove() const
{
    return isConnected() && shouldMove();
}

bool SGMEnergyControl::canStop() const
{
    return isConnected() && shouldStop();
}

SGMUndulatorSupport::UndulatorHarmonic SGMEnergyControl::undulatorHarmonic() const
{
    if(energyPositionController_) {

        return energyPositionController_->undulatorHarmonic();
    }

    return SGMUndulatorSupport::UnknownUndulatorHarmonic;
}

double SGMEnergyControl::undulatorOffset() const
{
    if(energyPositionController_) {

        return energyPositionController_->undulatorOffset();
    }

    return 0;
}

bool SGMEnergyControl::isUndulatorTracking() const
{
    if(energyPositionController_) {

        return energyPositionController_->isUndulatorTracking();
    }

    return false;
}

SGMEnergyPosition::GratingTranslationOptimizationMode SGMEnergyControl::gratingTranslationOptimizationMode() const
{
    if(energyPositionController_) {

        return energyPositionController_->gratingTranslationOptimizationMode();
    }

    return SGMEnergyPosition::ManualMode;
}

bool SGMEnergyControl::isExitSlitPositionTracking() const
{
    if(energyPositionController_) {

        return energyPositionController_->isExitSlitPositionTracking();
    }

    return false;
}

AMControl *SGMEnergyControl::gratingAngleControl() const
{
    return gratingAngleControl_;
}

AMControl *SGMEnergyControl::gratingTranslationControl() const
{
    return gratingTranslationControl_;
}

AMControl *SGMEnergyControl::undulatorPositionControl() const
{
    return undulatorPositionControl_;
}

AMControl *SGMEnergyControl::exitSlitPositionControl() const
{
    return exitSlitPositionControl_;
}

AMControl::FailureExplanation SGMEnergyControl::move(double startSetpoint, double finalSetpoint, double targetVelocity)
{
	// Check that this control is connected and able to move before proceeding.

	if (!isConnected()) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_NOT_CONNECTED, QString("Failed to move %1: control is not connected.").arg(name()));
		return AMControl::NotConnectedFailure;
	}

	if (!canMove()) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_CANNOT_MOVE, QString("Failed to move %1: control cannot move.").arg(name()));
		return AMControl::OtherFailure;
	}

	if (isMoving() && !allowsMovesWhileMoving()) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_ALREADY_MOVING, QString("Failed to move %1: control is already moving.").arg(name()));
		return AMControl::AlreadyMovingFailure;
	}

	SGMEnergyPosition* energyPositionHelper = energyPositionController_->clone();
	energyPositionHelper->requestEnergy(startSetpoint);

	double timeTaken = qAbs(finalSetpoint - startSetpoint) / targetVelocity;

	SGMEnergyTrajectory trajectoryHelper(startSetpoint,
										 finalSetpoint,
										 timeTaken,
										 energyPositionHelper->gratingTranslation());

	if(trajectoryHelper.hasErrors()) {
		AMErrorMon::alert(this, SGMENERGYCONTROL_INVALID_STATE, QString("Failed to move %1: \n%2").arg(name()).arg(trajectoryHelper.errorValidator()->fullFailureMessage()));
		energyPositionHelper->deleteLater();
		return AMControl::OtherFailure;
	}

	if (!validSetpoint(startSetpoint) || !validSetpoint(finalSetpoint)) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_INVALID_SETPOINT, QString("Failed to move %1: one of the provided setpoints is invalid.").arg(name()));
		energyPositionHelper->deleteLater();
		return AMControl::LimitFailure;
	}

	// Update the setpoint.
	setSetpoint(finalSetpoint);

	// I guess there's a chance that someone might perform a trajectory move from
	// out current position, to our current position. In which case we fake the
	// move.
	if (withinTolerance(startSetpoint) && withinTolerance(finalSetpoint)) {
		onMoveStarted(0);
		onMoveSucceeded(0);
		energyPositionHelper->deleteLater();
		return AMControl::NoFailure;
	}

	// Otherwise, an actual move is needed.
	// Create new move action.
	AMAction3 *moveAction = createMoveAction(&trajectoryHelper);

	// Check that a valid move action was generated.
	// If an invalid move action was created, abort the move.

	if (!moveAction) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_INVALID_MOVE_ACTION, QString("Did not move %1: invalid move action generated.").arg(name()));
		onMoveStarted(0);
		onMoveFailed(0);
		energyPositionHelper->deleteLater();
		return AMControl::LimitFailure;
	}

	// Otherwise, proceed with initializing and running the move action.
	// Create move action signal mappings.

	startedMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(started()), startedMapper_, SLOT(map()) );

	cancelledMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(cancelled()), cancelledMapper_, SLOT(map()) );

	failedMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(failed()), failedMapper_, SLOT(map()) );

	succeededMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(succeeded()), succeededMapper_, SLOT(map()) );

	// Run action.

	moveAction->start();

	return AMControl::NoFailure;
}

void SGMEnergyControl::setUndulatorHarmonic(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic)
{
    if(energyPositionController_) {

        energyPositionController_->setUndulatorHarmonic(undulatorHarmonic);
    }
}

void SGMEnergyControl::setUndulatorOffset(double undulatorOffset)
{
    if(energyPositionController_) {

        energyPositionController_->setUndulatorOffset(undulatorOffset);
    }
}

void SGMEnergyControl::setUndulatorTracking(bool isTracking)
{
    if(energyPositionController_) {

        energyPositionController_->setUndulatorTracking(isTracking);
    }
}

void SGMEnergyControl::setGratingTranslationOptimizationMode(SGMEnergyPosition::GratingTranslationOptimizationMode gratingTranslationOptimizationMode)
{
    if(energyPositionController_) {

        energyPositionController_->setGratingTranslationOptimizationMode(gratingTranslationOptimizationMode);
    }
}

void SGMEnergyControl::setExitSlitPositionTracking(bool isTracking)
{
    if(energyPositionController_) {

        energyPositionController_->setExitSlitPositionTracking(isTracking);
    }
}

void SGMEnergyControl::updateConnected()
{

    bool nowConnected = gratingAngleControl_->isConnected() &&
            gratingTranslationControl_->isConnected() &&
            undulatorPositionControl_->isConnected() &&
			exitSlitPositionControl_->isConnected() &&
			gratingAngleBaseVelocityControl_->isConnected() &&
			gratingAngleTargetVelocityControl_->isConnected() &&
			gratingAngleAccelerationControl_->isConnected() &&
			undulatorStepControl_->isConnected() &&
			undulatorStepVelocityControl_->isConnected();

    if(nowConnected && !energyPositionController_) {

        initializeEnergyPositionController();
    }

    setConnected(nowConnected);
}

void SGMEnergyControl::updateMoving()
{
    setIsMoving(gratingAngleControl_->isMoving() ||
                gratingTranslationControl_->isMoving() ||
                undulatorPositionControl_->isMoving() ||
                exitSlitPositionControl_->isMoving());
}

void SGMEnergyControl::updateValue()
{
    // Do this by monitoring the energy controllers energy changed signal
    // see onEnergyPositionControllerEnergyChanged(double)
}

void SGMEnergyControl::onGratingTranslationPVValueChanged(double value)
{
    SGMGratingSupport::GratingTranslation newGratingTranslation =
			SGMGratingSupport::encoderCountToEnum(value);

    if(newGratingTranslation != SGMGratingSupport::UnknownGrating) {

		SGMEnergyPosition::GratingTranslationOptimizationMode savedMode =
				energyPositionController_->gratingTranslationOptimizationMode();

		energyPositionController_->blockSignals(true);
		energyPositionController_->setGratingTranslationOptimizationMode(SGMEnergyPosition::ManualMode);
		energyPositionController_->blockSignals(false);

        energyPositionController_->setGratingTranslation(newGratingTranslation);

		energyPositionController_->blockSignals(true);
		energyPositionController_->setGratingTranslationOptimizationMode(savedMode);
		energyPositionController_->blockSignals(false);
    }
}

void SGMEnergyControl::onEnergyPositionControllerEnergyChanged(double value)
{
    setValue(value);
}

void SGMEnergyControl::initializeEnergyPositionController()
{
    double startingGratingAngle = gratingAngleControl_->value();
    double startingUndulatorPosition = undulatorPositionControl_->value();
    double startingExitSlitPosition = exitSlitPositionControl_->value();
    SGMGratingSupport::GratingTranslation startingGratingTranslation =
			SGMGratingSupport::encoderCountToEnum(gratingTranslationControl_->value());

    energyPositionController_ = new SGMEnergyPosition(startingGratingTranslation,
                                                      startingGratingAngle,
                                                      startingUndulatorHarmonic_,
                                                      startingUndulatorPosition,
                                                      0,
                                                      startingExitSlitPosition,
                                                      this);

    energyPositionController_->setAutoDetectUndulatorHarmonic(false);

    connect(energyPositionController_, SIGNAL(undulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic)),
            this, SIGNAL(undulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic)));

    connect(energyPositionController_, SIGNAL(undulatorOffsetChanged(double)),
            this, SIGNAL(undulatorOffsetChanged(double)));

    connect(energyPositionController_, SIGNAL(undulatorTrackingChanged(bool)),
            this, SIGNAL(undulatorTrackingChanged(bool)));

	connect(energyPositionController_, SIGNAL(gratingTranslationOptimizationModeChanged(SGMEnergyPosition::GratingTranslationOptimizationMode)),
			this, SIGNAL(gratingTranslationOptimizationModeChanged(SGMEnergyPosition::GratingTranslationOptimizationMode)));

    connect(energyPositionController_, SIGNAL(exitSlitTrackingChanged(bool)),
            this, SIGNAL(exitSlitTrackingChanged(bool)));

    connect(energyPositionController_, SIGNAL(energyChanged(double)),
            this, SLOT(onEnergyPositionControllerEnergyChanged(double)));

    connect(gratingAngleControl_, SIGNAL(valueChanged(double)),
            energyPositionController_, SLOT(setGratingAngle(double)));

    connect(gratingTranslationControl_, SIGNAL(valueChanged(double)),
            this, SLOT(onGratingTranslationPVValueChanged(double)));

    connect(undulatorPositionControl_, SIGNAL(valueChanged(double)),
            energyPositionController_, SLOT(setUndulatorPosition(double)));

    connect(exitSlitPositionControl_, SIGNAL(valueChanged(double)),
            energyPositionController_, SLOT(setExitSlitPosition(double)));

    updateStates();
    setValue(energyPositionController_->resultantEnergy());
}

AMAction3 *SGMEnergyControl::createMoveAction(double setpoint)
{
    AMListAction3* coordinatedMoveAction = 0;

    if(energyPositionController_) {

        AMListAction3* componentMoveAction = new AMListAction3(new AMListActionInfo3("Moving energy components",
                                                                                     "Moving energy components"),
                                                               AMListAction3::Parallel);

        // Set up starting position of all the components by cloning our controller.
        SGMEnergyPosition* helperEnergyPosition = energyPositionController_->clone();

        // Move helper energy position to the new energy.
        helperEnergyPosition->requestEnergy(setpoint);

        if(!helperEnergyPosition->hasErrors()) {

            // Create list action to move all components.
			double gratingTranslationNewValue = SGMGratingSupport::enumToEncoderCount(helperEnergyPosition->gratingTranslation());
            double gratingAngleNewValue = helperEnergyPosition->gratingAngle();
            double undulatorPositionNewValue = helperEnergyPosition->undulatorPosition();
            double exitSlitPositionNewValue = helperEnergyPosition->exitSlitPosition();

            componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingTranslationControl_, gratingTranslationNewValue, false));
            componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleControl_, gratingAngleNewValue, false));
            componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(undulatorPositionControl_, undulatorPositionNewValue, false));
            componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(exitSlitPositionControl_, exitSlitPositionNewValue, false));

            // Create list action to wait for component motions to complete.
            AMListAction3* componentWaitAction = new AMListAction3(new AMListActionInfo3("Waiting for energy components",
                                                                                         "Waiting for energy components to complete motions"),
                                                                   AMListAction3::Parallel);

            componentWaitAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingTranslationControl_, gratingTranslationNewValue, 60, AMControlWaitActionInfo::MatchWithinTolerance));
            componentWaitAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleControl_, gratingAngleNewValue, 60, AMControlWaitActionInfo::MatchWithinTolerance));
            componentWaitAction->addSubAction(AMActionSupport::buildControlWaitAction(undulatorPositionControl_, undulatorPositionNewValue, 60, AMControlWaitActionInfo::MatchWithinTolerance));
            componentWaitAction->addSubAction(AMActionSupport::buildControlWaitAction(exitSlitPositionControl_, exitSlitPositionNewValue, 60, AMControlWaitActionInfo::MatchWithinTolerance));

            QString moveActionName = QString("Moving to energy %1eV").arg(setpoint);
            coordinatedMoveAction = new AMListAction3(new AMListActionInfo3(moveActionName,
                                                                            moveActionName),
                                                      AMListAction3::Sequential);

            coordinatedMoveAction->addSubAction(componentMoveAction);
            coordinatedMoveAction->addSubAction(componentWaitAction);

		} else {
			AMErrorMon::alert(this, SGMENERGYCONTROL_INVALID_STATE, QString("Move would put energy in invalid state: %1").arg(helperEnergyPosition->errorValidator()->fullFailureMessage()));
		}

        helperEnergyPosition->deleteLater();
    }

	return coordinatedMoveAction;
}

#include <QDebug>
AMAction3 *SGMEnergyControl::createMoveAction(SGMEnergyTrajectory* energyTrajectory)
{
	AMListAction3* continuousMoveAction = 0;

	double startSetpoint = energyTrajectory->startEnergy();
	double velocity = energyTrajectory->energyVelocity();

	if(velocity != 0) {
		if(energyPositionController_) {

			if(!energyTrajectory->hasErrors()) {

				// Move to start using simple fast method
				qDebug() << "\t#1 Move to initial position at" << startSetpoint;
				AMAction3* initialMoveToStart = createMoveAction(startSetpoint);

				// Set velocities
				double gratingAngleBaseVelocity = energyTrajectory->gratingAngleVelocityProfile().baseVelocity();
				double gratingAngleTargetVelocity = energyTrajectory->gratingAngleVelocityProfile().targetVelocity();
				double gratingAngleAcceleration = energyTrajectory->gratingAngleVelocityProfile().initialAcceleration();

				double undulatorStepStart = SGMUndulatorSupport::undulatorStepFromPosition(energyTrajectory->startUndulatorPosition(),
																						   undulatorPositionControl_->value(),
																						   undulatorStepControl_->value());

				double undulatorStepEnd = SGMUndulatorSupport::undulatorStepFromPosition(energyTrajectory->endUndulatorPosition(),
																						 undulatorPositionControl_->value(),
																						 undulatorStepControl_->value());

				double undulatorStepVelocity = qAbs(undulatorStepEnd - undulatorStepStart) / energyTrajectory->time();

				qDebug() << "\t#2 Set Velocities:";
				qDebug() << "\t\t#2a Set grating angle base velocity to" << gratingAngleBaseVelocity;
				qDebug() << "\t\t#2b Set grating angle target velocity to" << gratingAngleTargetVelocity;
				qDebug() << "\t\t#2c Set grating angle acceleration to" << gratingAngleAcceleration;
				qDebug() << "\t\t#2d Set undulator step velocity to" << undulatorStepVelocity;
				AMListAction3* setVelocitiesAction = new AMListAction3(new AMListActionInfo3("Set Velocities",
																							 "Set Energy Control Velocities"),
																	   AMListAction3::Parallel);

				setVelocitiesAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleBaseVelocityControl_,
																						  gratingAngleBaseVelocity,
																						  false));

				setVelocitiesAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleTargetVelocityControl_,
																						  gratingAngleTargetVelocity,
																						  false));

				setVelocitiesAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleAccelerationControl_,
																						  gratingAngleAcceleration,
																						  false));

				setVelocitiesAction->addSubAction(AMActionSupport::buildControlMoveAction(undulatorStepVelocityControl_,
																						  undulatorStepVelocity,
																						  false));


				AMListAction3* waitForVelocitiesAction = new AMListAction3(new AMListActionInfo3("Wait for Velocities",
																								 "Wait for energy control velocities to be set"),
																		   AMListAction3::Parallel);

				waitForVelocitiesAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleBaseVelocityControl_,
																							  gratingAngleBaseVelocity,
																							  10,
																							  AMControlWaitActionInfo::MatchWithinTolerance));

				waitForVelocitiesAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleTargetVelocityControl_,
																							  gratingAngleTargetVelocity,
																							  10,
																							  AMControlWaitActionInfo::MatchWithinTolerance));

				waitForVelocitiesAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleAccelerationControl_,
																							  gratingAngleAcceleration,
																							  10,
																							  AMControlWaitActionInfo::MatchWithinTolerance));

				waitForVelocitiesAction->addSubAction(AMActionSupport::buildControlWaitAction(undulatorStepVelocityControl_,
																							  undulatorStepVelocity,
																							  10,
																							  AMControlWaitActionInfo::MatchWithinTolerance));
				double meanExitSlitPosition = (energyTrajectory->startExitSlitPosition() + energyTrajectory->endExitSlitPosition()) / 2;
				qDebug() << "\t#3 Set Exit Slit position to mean position at" << meanExitSlitPosition;
				AMAction3* positionExitSlitAction = AMActionSupport::buildControlMoveAction(exitSlitPositionControl_,
																							meanExitSlitPosition,
																							false);
				AMAction3* waitForExitSlitAction = AMActionSupport::buildControlWaitAction(exitSlitPositionControl_,
																						  meanExitSlitPosition,
																						  10,
																						  AMControlWaitActionInfo::MatchWithinTolerance);

				AMListAction3* trajectoryMove = new AMListAction3(new AMListActionInfo3("Energy Trajectory",
																						"Energy Trajectory"),
																  AMListAction3::Parallel);

				double undulatorStepTarget = SGMUndulatorSupport::undulatorStepFromPosition(energyTrajectory->endUndulatorPosition(),
																							undulatorPositionControl_->value(),
																							undulatorStepControl_->value());
				double gratingAngleTarget = energyTrajectory->endGratingAngleEncoderCount();
				qDebug() << "\t#3 Performing move:";
				qDebug() << "\t\t#3a Moving undulator step to" << undulatorStepTarget;
				qDebug() << "\t\t#3b Moving grating angle to" << gratingAngleTarget;
				trajectoryMove->addSubAction(AMActionSupport::buildControlMoveAction(undulatorStepControl_,
																					 undulatorStepTarget,
																					 false));
				trajectoryMove->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleControl_,
																					 gratingAngleTarget,
																					 false));

				AMListAction3* trajectoryWait = new AMListAction3(new AMListActionInfo3("Energy Trajectory Wait",
																						"Energy Trajectory Wait"),
																  AMListAction3::Parallel);
				trajectoryWait->addSubAction(AMActionSupport::buildControlWaitAction(undulatorStepControl_,
																					 undulatorStepTarget,
																					 10,
																					 AMControlWaitActionInfo::MatchWithinTolerance));

				trajectoryWait->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleControl_,
																					 gratingAngleTarget,
																					 10,
																					 AMControlWaitActionInfo::MatchWithinTolerance));


				continuousMoveAction = new AMListAction3(new AMListActionInfo3("Moving Energy",
																			   "Moving Energy"),
														 AMListAction3::Sequential);

				continuousMoveAction->addSubAction(initialMoveToStart);
				continuousMoveAction->addSubAction(setVelocitiesAction);
				continuousMoveAction->addSubAction(waitForVelocitiesAction);
				continuousMoveAction->addSubAction(positionExitSlitAction);
				continuousMoveAction->addSubAction(waitForExitSlitAction);
				continuousMoveAction->addSubAction(trajectoryMove);
				continuousMoveAction->addSubAction(trajectoryWait);

			} else {
				AMErrorMon::alert(this, SGMENERGYCONTROL_INVALID_STATE, QString("Move would put energy in invalid state: %1").arg(energyTrajectory->errorValidator()->fullFailureMessage()));
			}

		}
	} else {

		AMErrorMon::alert(this, SGMENERGYCONTROL_ZERO_ENERGY_VELOCITY, QString("Velocity is invalid (cannot be zero)"));
	}

	//return continuousMoveAction;
	return 0;
}


