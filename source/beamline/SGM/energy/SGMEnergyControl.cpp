#include "SGMEnergyControl.h"

#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "SGMGratingTranslationStepControl.h"
#include "util/AMErrorMonitor.h"
#include "SGMEnergyTrajectory.h"
#include "SGMGratingAngleControl.h"

SGMEnergyControl::SGMEnergyControl(SGMUndulatorSupport::UndulatorHarmonic startingUndulatorHarmonic,
                                   QObject *parent) :
    AMPseudoMotorControl("Energy", "eV", parent, "SGM Monochromator Energy")
{
	value_ = 0;
	setpoint_ = 0;
	connected_ = false;
	isMoving_ = false;

    energyPositionController_ = 0;
    startingUndulatorHarmonic_ = startingUndulatorHarmonic;

	gratingAngleControl_ = new SGMGratingAngleControl(this);

	gratingTranslationStepControl_ = new SGMGratingTranslationStepControl(this);

    undulatorPositionControl_ = new AMPVwStatusControl("Undulator Position",
                                                       "UND1411-01:gap:mm:fbk",
                                                       "UND1411-01:gap:mm",
                                                       "UND1411-01:moveStatus",
                                                       "UND1411-01:stop",
                                                       this,
                                                       0.1);

	undulatorStepAccelerationControl_ = new AMPVControl("Undulator Acceleration",
														"SMTR1411-01:accel:sp",
														"SMTR1411-01:accel",
														QString(),
														this,
														1);

    exitSlitPositionControl_ = new AMPVwStatusControl("Exit Slit Position",
                                                      "PSL16114I1003:Y:mm:fbk",
                                                      "PSL16114I1003:Y:mm:encsp",
                                                      "SMTR16114I1003:status",
                                                      "SMTR16114I1003:stop",
                                                      this,
                                                      0.1,
                                                      2.0,
                                                      new AMControlStatusCheckerDefault(1));

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
	addChildControl(undulatorPositionControl_);
	addChildControl(exitSlitPositionControl_);
	addChildControl(undulatorStepControl_);
	addChildControl(undulatorStepVelocityControl_);
	addChildControl(gratingTranslationStepControl_);
    addChildControl(undulatorPositionControl_);
	addChildControl(exitSlitPositionControl_);

    setMinimumValue(200);
    setMaximumValue(3000);
	setTolerance(1.5);
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
	return (gratingAngleControl_->canMeasure() &&
			gratingTranslationStepControl_->canMeasure() &&
			undulatorPositionControl_->canMeasure() &&
			exitSlitPositionControl_->canMeasure());
}

bool SGMEnergyControl::canMove() const
{
	return (gratingAngleControl_->canMove() &&
			gratingTranslationStepControl_->canMove() &&
			undulatorPositionControl_->canMove() &&
			exitSlitPositionControl_->canMove());
}

bool SGMEnergyControl::canStop() const
{
	return (gratingAngleControl_->canStop() &&
			gratingTranslationStepControl_->canStop() &&
			undulatorPositionControl_->canStop() &&
			exitSlitPositionControl_->canStop());
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
	return gratingTranslationStepControl_;
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
										 energyPositionHelper->gratingTranslation(),
										 gratingAngleControl_->stepAccelerationControl()->value(),
										 gratingAngleControl_->stepsPerEncoderCount(),
										 undulatorStepAccelerationControl_->value(),
										 undulatorPositionControl_->value(),
										 undulatorStepControl_->value());

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
			gratingTranslationStepControl_->isConnected() &&
            undulatorPositionControl_->isConnected() &&
			exitSlitPositionControl_->isConnected() &&
			undulatorStepControl_->isConnected() &&
			undulatorStepVelocityControl_->isConnected();

    if(nowConnected && !energyPositionController_) {

        initializeEnergyPositionController();
    }

	setConnected(nowConnected);
}

void SGMEnergyControl::updateMoving()
{
	if(isConnected()) {
		setIsMoving(gratingAngleControl_->isMoving() ||
					gratingTranslationStepControl_->isMoving() ||
					undulatorPositionControl_->isMoving() ||
					exitSlitPositionControl_->isMoving());
	}
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

void SGMEnergyControl::onEnergyPositionUndulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic)
{
	double newUndulatorPosition = energyPositionController_->undulatorPosition();
	if(!undulatorPositionControl_->withinTolerance(newUndulatorPosition)) {

		undulatorPositionControl_->move(newUndulatorPosition);
	}
	emit undulatorHarmonicChanged(undulatorHarmonic);
}

void SGMEnergyControl::onEnergyPositionGratingTranslationOptimizationModeChanged(SGMEnergyPosition::GratingTranslationOptimizationMode optimizationMode)
{
	if(energyPositionController_->gratingTranslation() != SGMGratingSupport::UnknownGrating) {

		double newGratingTranslationPosition = SGMGratingSupport::enumToEncoderCount(energyPositionController_->gratingTranslation());
		if(!gratingTranslationControl()->withinTolerance(newGratingTranslationPosition)) {

			gratingTranslationControl()->move(newGratingTranslationPosition);
		}
	}
	emit gratingTranslationOptimizationModeChanged(optimizationMode);
}

void SGMEnergyControl::initializeEnergyPositionController()
{
    double startingGratingAngle = gratingAngleControl_->value();
    double startingUndulatorPosition = undulatorPositionControl_->value();
    double startingExitSlitPosition = exitSlitPositionControl_->value();
    SGMGratingSupport::GratingTranslation startingGratingTranslation =
			SGMGratingSupport::encoderCountToEnum(gratingTranslationStepControl_->value());

	energyPositionController_ = new SGMEnergyPosition(startingGratingTranslation,
                                                      startingGratingAngle,
                                                      startingUndulatorHarmonic_,
                                                      startingUndulatorPosition,
                                                      0,
                                                      startingExitSlitPosition,
                                                      this);

    energyPositionController_->setAutoDetectUndulatorHarmonic(false);

    connect(energyPositionController_, SIGNAL(undulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic)),
			this, SLOT(onEnergyPositionUndulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic)));

    connect(energyPositionController_, SIGNAL(undulatorOffsetChanged(double)),
            this, SIGNAL(undulatorOffsetChanged(double)));

    connect(energyPositionController_, SIGNAL(undulatorTrackingChanged(bool)),
            this, SIGNAL(undulatorTrackingChanged(bool)));

	connect(energyPositionController_, SIGNAL(gratingTranslationOptimizationModeChanged(SGMEnergyPosition::GratingTranslationOptimizationMode)),
			this, SLOT(onEnergyPositionGratingTranslationOptimizationModeChanged(SGMEnergyPosition::GratingTranslationOptimizationMode)));

    connect(energyPositionController_, SIGNAL(exitSlitTrackingChanged(bool)),
            this, SIGNAL(exitSlitTrackingChanged(bool)));

    connect(energyPositionController_, SIGNAL(energyChanged(double)),
            this, SLOT(onEnergyPositionControllerEnergyChanged(double)));

    connect(gratingAngleControl_, SIGNAL(valueChanged(double)),
            energyPositionController_, SLOT(setGratingAngle(double)));

	connect(gratingTranslationStepControl_, SIGNAL(valueChanged(double)),
            this, SLOT(onGratingTranslationPVValueChanged(double)));

    connect(undulatorPositionControl_, SIGNAL(valueChanged(double)),
            energyPositionController_, SLOT(setUndulatorPosition(double)));

    connect(exitSlitPositionControl_, SIGNAL(valueChanged(double)),
            energyPositionController_, SLOT(setExitSlitPosition(double)));

	onEnergyPositionControllerEnergyChanged(energyPositionController_->resultantEnergy());
	updateStates();
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

			// Ensure controls are in their default state for encoder moves
			AMAction3* setDefaultsAction = gratingAngleControl_->setDefaultsAction();

            // Create list action to move all components.
			double gratingTranslationNewValue = SGMGratingSupport::enumToEncoderCount(helperEnergyPosition->gratingTranslation());
            double gratingAngleNewValue = helperEnergyPosition->gratingAngle();
            double undulatorPositionNewValue = helperEnergyPosition->undulatorPosition();
            double exitSlitPositionNewValue = helperEnergyPosition->exitSlitPosition();

			componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingTranslationStepControl_, gratingTranslationNewValue));
			componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleControl_, gratingAngleNewValue));
			if(energyPositionController_->isUndulatorTracking()) {
				componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(undulatorPositionControl_, undulatorPositionNewValue));
			}

			if(energyPositionController_->isExitSlitPositionTracking()) {
				componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(exitSlitPositionControl_, exitSlitPositionNewValue));
			}

            // Create list action to wait for component motions to complete.
            AMListAction3* componentWaitAction = new AMListAction3(new AMListActionInfo3("Waiting for energy components",
                                                                                         "Waiting for energy components to complete motions"),
                                                                   AMListAction3::Parallel);

            componentWaitAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingTranslationStepControl_, gratingTranslationNewValue, 60, AMControlWaitActionInfo::MatchWithinTolerance));
            componentWaitAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleControl_, gratingAngleNewValue, 60, AMControlWaitActionInfo::MatchWithinTolerance));
			if(energyPositionController_->isUndulatorTracking()) {
				componentWaitAction->addSubAction(AMActionSupport::buildControlWaitAction(undulatorPositionControl_, undulatorPositionNewValue, 60, AMControlWaitActionInfo::MatchWithinTolerance));
			}

			if(energyPositionController_->isExitSlitPositionTracking()) {
				componentWaitAction->addSubAction(AMActionSupport::buildControlWaitAction(exitSlitPositionControl_, exitSlitPositionNewValue, 60, AMControlWaitActionInfo::MatchWithinTolerance));
			}

            QString moveActionName = QString("Moving to energy %1eV").arg(setpoint);
            coordinatedMoveAction = new AMListAction3(new AMListActionInfo3(moveActionName,
                                                                            moveActionName),
                                                      AMListAction3::Sequential);

			coordinatedMoveAction->addSubAction(setDefaultsAction);
            coordinatedMoveAction->addSubAction(componentMoveAction);
            coordinatedMoveAction->addSubAction(componentWaitAction);

		} else {

			AMErrorMon::alert(this, SGMENERGYCONTROL_INVALID_STATE, QString("Move would put energy in invalid state: %1").arg(helperEnergyPosition->errorValidator()->fullFailureMessage()));
		}

        helperEnergyPosition->deleteLater();
    }
	return coordinatedMoveAction;
}

AMAction3 *SGMEnergyControl::createMoveAction(SGMEnergyTrajectory* energyTrajectory)
{
	AMListAction3* continuousMoveAction = 0;

	double startSetpoint = energyTrajectory->startEnergy();
	double velocity = energyTrajectory->energyVelocity();

	if(velocity != 0) {
		if(energyPositionController_) {

			if(!energyTrajectory->hasErrors()) {

				QString actionTitle = QString("Coordinated move of Energy from %1eV to %2eV")
						.arg(energyTrajectory->startEnergy())
						.arg(energyTrajectory->endEnergy());

				continuousMoveAction = new AMListAction3(new AMListActionInfo3(actionTitle, actionTitle),
														 AMListAction3::Sequential);

				continuousMoveAction->addSubAction(gratingAngleControl_->setDefaultsAction());
				// Moving to start position and set exit slit position:
				AMListAction3* initializationActions = new AMListAction3(new AMListActionInfo3("Setting up movement",
																							   "Setting up movement"),
																		 AMListAction3::Parallel);

				bool savedExitSlitTracking = energyPositionController_->isExitSlitPositionTracking();

				energyPositionController_->setExitSlitPositionTracking(false);
				initializationActions->addSubAction(createMoveAction(startSetpoint));
				energyPositionController_->setExitSlitPositionTracking(savedExitSlitTracking);
				continuousMoveAction->addSubAction(initializationActions);

				if(energyPositionController_->isExitSlitPositionTracking()) {
					// Move Exit Slit to mean position
					double meanExitSlitPosition = (energyTrajectory->startExitSlitPosition() + energyTrajectory->endExitSlitPosition()) / 2;
					initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(exitSlitPositionControl_,
																							   meanExitSlitPosition));

					continuousMoveAction->addSubAction(AMActionSupport::buildControlWaitAction(exitSlitPositionControl_,
																							   meanExitSlitPosition,
																							   10,
																							   AMControlWaitActionInfo::MatchWithinTolerance));
				}

				continuousMoveAction->addSubAction(AMActionSupport::buildControlWaitAction(this, startSetpoint, 60, AMControlWaitActionInfo::MatchWithinTolerance));

				// Set motion properties (velocities, accelerations etc)
				AMListAction3* setMotionPropertiesAction = new AMListAction3(new AMListActionInfo3("Set motion properties",
																								   "Set motion properties"),
																			 AMListAction3::Parallel);

				double gratingAngleVelocity = energyTrajectory->gratingAngleVelocityProfile().targetVelocity();
				double gratingAngleAcceleration = energyTrajectory->gratingAngleVelocityProfile().acceleration();

				setMotionPropertiesAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleControl_->stepVelocityControl(),
																								gratingAngleVelocity));

				setMotionPropertiesAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleControl_->stepAccelerationControl(),
																								gratingAngleAcceleration));

				setMotionPropertiesAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleControl_->movementTypeControl(),
																								0));

				double undulatorStepVelocity = energyTrajectory->undulatorVelocityProfile().targetVelocity();
				double undulatorStepAcceleration = energyTrajectory->undulatorVelocityProfile().acceleration();

				if(energyPositionController_->isUndulatorTracking()) {

					setMotionPropertiesAction->addSubAction(AMActionSupport::buildControlMoveAction(undulatorStepAccelerationControl_,
																									undulatorStepAcceleration));

					setMotionPropertiesAction->addSubAction(AMActionSupport::buildControlMoveAction(undulatorStepVelocityControl_,
																									undulatorStepVelocity));
				}
				continuousMoveAction->addSubAction(setMotionPropertiesAction);

				// Wait for motion properties to take
				AMListAction3* waitForMotionPropertiesAction = new AMListAction3(new AMListActionInfo3("Wait for motion properties",
																									   "Wait for motion properies"),
																				 AMListAction3::Parallel);


				waitForMotionPropertiesAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleControl_->stepVelocityControl(),
																									gratingAngleVelocity,
																									10,
																									AMControlWaitActionInfo::MatchWithinTolerance));

				waitForMotionPropertiesAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleControl_->stepAccelerationControl(),
																									gratingAngleAcceleration,
																									10,
																									AMControlWaitActionInfo::MatchWithinTolerance));

				waitForMotionPropertiesAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleControl_->movementTypeControl(),
																									0,
																									10,
																									AMControlWaitActionInfo::MatchWithinTolerance));

				if(energyPositionController_->isUndulatorTracking()) {

					waitForMotionPropertiesAction->addSubAction(AMActionSupport::buildControlWaitAction(undulatorStepAccelerationControl_,
																										undulatorStepAcceleration,
																										10,
																										AMControlWaitActionInfo::MatchWithinTolerance));

					waitForMotionPropertiesAction->addSubAction(AMActionSupport::buildControlWaitAction(undulatorStepVelocityControl_,
																										undulatorStepVelocity,
																										10,
																										AMControlWaitActionInfo::MatchWithinTolerance));
				}

				continuousMoveAction->addSubAction(waitForMotionPropertiesAction);

				// Build the trajectory move
				AMListAction3* trajectoryMove = new AMListAction3(new AMListActionInfo3("Energy Trajectory",
																						"Energy Trajectory"),
																  AMListAction3::Parallel);


				double gratingAngleTarget = energyTrajectory->endGratingAngleEncoderCount();
				double undulatorStepsTarget = energyTrajectory->undulatorVelocityProfile().endPosition();

				trajectoryMove->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleControl_,
																					 gratingAngleTarget));

				if(energyPositionController_->isUndulatorTracking()) {
					trajectoryMove->addSubAction(AMActionSupport::buildControlMoveAction(undulatorStepControl_,
																						undulatorStepsTarget));
				}

				continuousMoveAction->addSubAction(trajectoryMove);


				// Build the trajectory wait
				AMListAction3* trajectoryWait = new AMListAction3(new AMListActionInfo3("Energy Trajectory Wait",
																						"Energy Trajectory Wait"),
																  AMListAction3::Parallel);


				trajectoryWait->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleControl_,
																					 gratingAngleTarget,
																					 60,
																					 AMControlWaitActionInfo::MatchWithinTolerance));

				if(energyPositionController_->isUndulatorTracking()) {
					trajectoryWait->addSubAction(AMActionSupport::buildControlWaitAction(undulatorStepControl_,
																						 undulatorStepsTarget,
																						 60,
																						 AMControlWaitActionInfo::MatchWithinTolerance));
				}


				continuousMoveAction->addSubAction(trajectoryWait);

				// Put the grating angle back into its default mode.
				continuousMoveAction->addSubAction(gratingAngleControl_->setDefaultsAction());


			} else {
				AMErrorMon::alert(this, SGMENERGYCONTROL_INVALID_STATE, QString("Move would put energy in invalid state: %1").arg(energyTrajectory->errorValidator()->fullFailureMessage()));
			}

		}
	} else {

		AMErrorMon::alert(this, SGMENERGYCONTROL_ZERO_ENERGY_VELOCITY, QString("Velocity is invalid (cannot be zero)"));
	}

	return continuousMoveAction;
}
