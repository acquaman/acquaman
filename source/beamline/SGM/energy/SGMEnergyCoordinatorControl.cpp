#include "SGMEnergyCoordinatorControl.h"

#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMChangeToleranceAction.h"
#include "SGMGratingTranslationStepControl.h"
#include "util/AMErrorMonitor.h"
#include "SGMEnergyTrajectory.h"
#include "SGMGratingAngleControl.h"
#include "SGMUndulatorControl.h"

SGMEnergyCoordinatorControl::SGMEnergyCoordinatorControl(SGMUndulatorSupport::UndulatorHarmonic startingUndulatorHarmonic,
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

	undulatorControl_ = new SGMUndulatorControl(this);

	exitSlitPositionControl_ = new AMPVwStatusControl("Exit Slit Position",
	                                                  "PSL16114I1003:Y:mm:fbk",
	                                                  "PSL16114I1003:Y:mm:encsp",
	                                                  "SMTR16114I1003:status",
	                                                  "SMTR16114I1003:stop",
	                                                  this,
	                                                  0.1,
	                                                  2.0,
	                                                  new AMControlStatusCheckerDefault(1));
	exitSlitPositionControl_->setAttemptMoveWhenWithinTolerance(true);

	addChildControl(gratingAngleControl_);
	addChildControl(undulatorControl_);
	addChildControl(exitSlitPositionControl_);
	addChildControl(gratingTranslationStepControl_);
	addChildControl(undulatorControl_);
	addChildControl(exitSlitPositionControl_);

	setAttemptMoveWhenWithinTolerance(true);
	setMinimumValue(200);
	setMaximumValue(3000);
	setTolerance(SGMENERGYCONTROL_CLOSEDLOOP_TOLERANCE);
	setDisplayPrecision(5);

}

bool SGMEnergyCoordinatorControl::shouldMeasure() const
{
	return true;
}

bool SGMEnergyCoordinatorControl::shouldMove() const
{
	return true;
}

bool SGMEnergyCoordinatorControl::shouldStop() const
{
	return true;
}

bool SGMEnergyCoordinatorControl::canMeasure() const
{
	return (gratingAngleControl_->canMeasure() &&
	        gratingTranslationStepControl_->canMeasure() &&
	        undulatorControl_->canMeasure() &&
	        exitSlitPositionControl_->canMeasure());
}

bool SGMEnergyCoordinatorControl::canMove() const
{
	return (gratingAngleControl_->canMove() &&
	        gratingTranslationStepControl_->canMove() &&
	        undulatorControl_->canMove() &&
	        exitSlitPositionControl_->canMove());
}

bool SGMEnergyCoordinatorControl::canStop() const
{
	return (gratingAngleControl_->canStop() &&
	        gratingTranslationStepControl_->canStop() &&
	        undulatorControl_->canStop() &&
	        exitSlitPositionControl_->canStop());
}

SGMUndulatorSupport::UndulatorHarmonic SGMEnergyCoordinatorControl::undulatorHarmonic() const
{
	if(energyPositionController_) {

		return energyPositionController_->undulatorHarmonic();
	}

	return SGMUndulatorSupport::UnknownUndulatorHarmonic;
}

double SGMEnergyCoordinatorControl::undulatorOffset() const
{
	if(energyPositionController_) {

		return energyPositionController_->undulatorOffset();
	}

	return 0;
}

bool SGMEnergyCoordinatorControl::isUndulatorTracking() const
{
	if(energyPositionController_) {

		return energyPositionController_->isUndulatorTracking();
	}

	return false;
}

SGMGratingSupport::GratingTranslationOptimizationMode SGMEnergyCoordinatorControl::gratingTranslationOptimizationMode() const
{
	if(energyPositionController_) {

		return energyPositionController_->gratingTranslationOptimizationMode();
	}

	return SGMGratingSupport::ManualMode;
}

bool SGMEnergyCoordinatorControl::isExitSlitPositionTracking() const
{
	if(energyPositionController_) {

		return energyPositionController_->isExitSlitPositionTracking();
	}

	return false;
}

AMControl *SGMEnergyCoordinatorControl::gratingAngleControl() const
{
	return gratingAngleControl_;
}

AMControl *SGMEnergyCoordinatorControl::gratingTranslationControl() const
{
	return gratingTranslationStepControl_;
}

AMControl *SGMEnergyCoordinatorControl::undulatorPositionControl() const
{
	return undulatorControl_;
}

AMControl *SGMEnergyCoordinatorControl::exitSlitPositionControl() const
{
	return exitSlitPositionControl_;
}

AMControl::FailureExplanation SGMEnergyCoordinatorControl::move(double targetSetpoint, double time)
{
	// Check that this control is connected and able to move before proceeding.

	if (!isConnected()) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_NOT_CONNECTED, QString("Failed to coordinated move %1: control is not connected.").arg(name()));
		return AMControl::NotConnectedFailure;
	}

	if (!canMove()) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_CANNOT_MOVE, QString("Failed to coordinated move %1: control cannot move.").arg(name()));
		return AMControl::OtherFailure;
	}

	if (isMoving() && !allowsMovesWhileMoving()) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_ALREADY_MOVING, QString("Failed to coordinated move %1: control is already moving.").arg(name()));
		return AMControl::AlreadyMovingFailure;
	}

	SGMEnergyTrajectory trajectoryHelper(value(),
					     targetSetpoint,
	                                     time,
					     SGMGratingSupport::encoderCountToEnum(gratingTranslationControl()->value()),
	                                     gratingAngleControl_->stepAccelerationControl()->value(),
	                                     gratingAngleControl_->stepsPerEncoderCount(),
	                                     undulatorControl_->stepAccelerationControl()->value(),
	                                     undulatorControl_->value(),
	                                     undulatorControl_->stepControl()->value(),
	                                     energyPositionController_->undulatorHarmonic());

	if(trajectoryHelper.hasErrors()) {
		AMErrorMon::alert(this, SGMENERGYCONTROL_INVALID_STATE, QString("Failed to move %1: \n%2").arg(name()).arg(trajectoryHelper.errorValidator()->fullFailureMessage()));
		return AMControl::OtherFailure;
	}

	if (!validSetpoint(targetSetpoint)) {
		AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_INVALID_SETPOINT, QString("Failed to move %1: one of the provided setpoints is invalid.").arg(name()));
		return AMControl::LimitFailure;
	}

	// Update the setpoint.
	setSetpoint(targetSetpoint);

	// I guess there's a chance that someone might perform a trajectory move from
	// out current position, to our current position. In which case we fake the
	// move.
	if (withinTolerance(targetSetpoint) && !attemptMoveWhenWithinTolerance()) {
		onMoveStarted(0);
		onMoveSucceeded(0);
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

void SGMEnergyCoordinatorControl::setUndulatorHarmonic(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic)
{
	if(energyPositionController_) {

		energyPositionController_->setUndulatorHarmonic(undulatorHarmonic);
	}
}

void SGMEnergyCoordinatorControl::setUndulatorOffset(double undulatorOffset)
{
	if(energyPositionController_) {

		energyPositionController_->setUndulatorOffset(undulatorOffset);
	}
}

void SGMEnergyCoordinatorControl::setUndulatorTracking(bool isTracking)
{
	if(energyPositionController_) {

		energyPositionController_->setUndulatorTracking(isTracking);
	}
}

void SGMEnergyCoordinatorControl::setGratingTranslationOptimizationMode(SGMGratingSupport::GratingTranslationOptimizationMode gratingTranslationOptimizationMode)
{
	if(energyPositionController_) {

		energyPositionController_->setGratingTranslationOptimizationMode(gratingTranslationOptimizationMode);
	}
}

void SGMEnergyCoordinatorControl::setExitSlitPositionTracking(bool isTracking)
{
	if(energyPositionController_) {

		energyPositionController_->setExitSlitPositionTracking(isTracking);
	}
}

void SGMEnergyCoordinatorControl::updateConnected()
{

	bool nowConnected = gratingAngleControl_->isConnected() &&
	        gratingTranslationStepControl_->isConnected() &&
	        undulatorControl_->isConnected() &&
	        exitSlitPositionControl_->isConnected();

	if(nowConnected && !energyPositionController_) {

		initializeEnergyPositionController();
	}

	setConnected(nowConnected);
}

void SGMEnergyCoordinatorControl::updateMoving()
{
	if(isConnected()) {
		setIsMoving(gratingAngleControl_->isMoving() ||
		            gratingTranslationStepControl_->isMoving() ||
		            undulatorControl_->isMoving() ||
		            exitSlitPositionControl_->isMoving());
	}
}

void SGMEnergyCoordinatorControl::updateValue()
{
	// Do this by monitoring the energy controllers energy changed signal
	// see onEnergyPositionControllerEnergyChanged(double)
}

void SGMEnergyCoordinatorControl::onGratingTranslationPVValueChanged(double value)
{
	SGMGratingSupport::GratingTranslation newGratingTranslation =
	        SGMGratingSupport::encoderCountToEnum(value);

	if(newGratingTranslation != SGMGratingSupport::UnknownGrating) {

		SGMGratingSupport::GratingTranslationOptimizationMode savedMode =
		        energyPositionController_->gratingTranslationOptimizationMode();

		energyPositionController_->blockSignals(true);
		energyPositionController_->setGratingTranslationOptimizationMode(SGMGratingSupport::ManualMode);
		energyPositionController_->blockSignals(false);

		energyPositionController_->setGratingTranslation(newGratingTranslation);

		energyPositionController_->blockSignals(true);
		energyPositionController_->setGratingTranslationOptimizationMode(savedMode);
		energyPositionController_->blockSignals(false);
	}
}

void SGMEnergyCoordinatorControl::onEnergyPositionControllerEnergyChanged(double value)
{
	setValue(value);
}

void SGMEnergyCoordinatorControl::onEnergyPositionUndulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic)
{
	double newUndulatorPosition = energyPositionController_->undulatorPosition();
	if(!undulatorControl_->withinTolerance(newUndulatorPosition)) {

		undulatorControl_->move(newUndulatorPosition);
	}
	emit undulatorHarmonicChanged(undulatorHarmonic);
}

void SGMEnergyCoordinatorControl::onEnergyPositionUndulatorOffsetChanged(double /*value*/)
{
	double newUndulatorPosition = energyPositionController_->undulatorPosition();
	if(!undulatorControl_->withinTolerance(newUndulatorPosition)) {

		undulatorControl_->move(newUndulatorPosition);
	}
}

void SGMEnergyCoordinatorControl::onEnergyPositionGratingTranslationOptimizationModeChanged(SGMGratingSupport::GratingTranslationOptimizationMode optimizationMode)
{
	if(energyPositionController_->gratingTranslation() != SGMGratingSupport::UnknownGrating) {

		double newGratingTranslationPosition = SGMGratingSupport::enumToEncoderCount(energyPositionController_->gratingTranslation());
		if(!gratingTranslationControl()->withinTolerance(newGratingTranslationPosition)) {

			gratingTranslationControl()->move(newGratingTranslationPosition);
		}
	}
	emit gratingTranslationOptimizationModeChanged(optimizationMode);
}

void SGMEnergyCoordinatorControl::initializeEnergyPositionController()
{
	double startingGratingAngle = gratingAngleControl_->value();
	double startingUndulatorPosition = undulatorControl_->value();
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
		this, SLOT(onEnergyPositionUndulatorOffsetChanged(double)));

	connect(energyPositionController_, SIGNAL(undulatorOffsetChanged(double)),
	        this, SIGNAL(undulatorOffsetChanged(double)));

	connect(energyPositionController_, SIGNAL(undulatorTrackingChanged(bool)),
	        this, SIGNAL(undulatorTrackingChanged(bool)));

	connect(energyPositionController_, SIGNAL(gratingTranslationOptimizationModeChanged(SGMGratingSupport::GratingTranslationOptimizationMode)),
	        this, SLOT(onEnergyPositionGratingTranslationOptimizationModeChanged(SGMGratingSupport::GratingTranslationOptimizationMode)));

	connect(energyPositionController_, SIGNAL(exitSlitTrackingChanged(bool)),
	        this, SIGNAL(exitSlitTrackingChanged(bool)));

	connect(energyPositionController_, SIGNAL(energyChanged(double)),
	        this, SLOT(onEnergyPositionControllerEnergyChanged(double)));

	connect(gratingAngleControl_, SIGNAL(valueChanged(double)),
	        energyPositionController_, SLOT(setGratingAngle(double)));

	connect(gratingTranslationStepControl_, SIGNAL(valueChanged(double)),
	        this, SLOT(onGratingTranslationPVValueChanged(double)));

	connect(undulatorControl_, SIGNAL(valueChanged(double)),
	        energyPositionController_, SLOT(setUndulatorPosition(double)));

	connect(exitSlitPositionControl_, SIGNAL(valueChanged(double)),
	        energyPositionController_, SLOT(setExitSlitPosition(double)));

	onEnergyPositionControllerEnergyChanged(energyPositionController_->resultantEnergy());
	updateStates();
}

AMAction3 *SGMEnergyCoordinatorControl::createMoveAction(double setpoint)
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
			AMListAction3* setDefaultsAction = new AMListAction3(new AMListActionInfo3("Setting Defaults",
			                                                                           "Setting Defaults"),
			                                                     AMListAction3::Parallel);

                        setDefaultsAction->addSubAction(gratingAngleControl_->createDefaultsAction());

			// Create list action to move all components.
			double gratingTranslationNewValue = SGMGratingSupport::enumToEncoderCount(helperEnergyPosition->gratingTranslation());
			double gratingAngleNewValue = helperEnergyPosition->gratingAngle();
			double undulatorPositionNewValue = helperEnergyPosition->undulatorPosition();
			double exitSlitPositionNewValue = helperEnergyPosition->exitSlitPosition();

			componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingTranslationStepControl_, gratingTranslationNewValue));
			componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleControl_, gratingAngleNewValue));
			if(energyPositionController_->isUndulatorTracking()) {
				componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(undulatorControl_, undulatorPositionNewValue));
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
				componentWaitAction->addSubAction(AMActionSupport::buildControlWaitAction(undulatorControl_, undulatorPositionNewValue, 60, AMControlWaitActionInfo::MatchWithinTolerance));
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

AMAction3 *SGMEnergyCoordinatorControl::createMoveAction(SGMEnergyTrajectory* energyTrajectory)
{
	AMListAction3* continuousMoveAction = 0;

	if(energyTrajectory->energyVelocity() != 0) {
		if(energyPositionController_) {

			if(!energyTrajectory->hasErrors()) {

				QString actionTitle = QString("Coordinated move of Energy from %1eV to %2eV")
				        .arg(energyTrajectory->startEnergy())
				        .arg(energyTrajectory->endEnergy());

				continuousMoveAction = new AMListAction3(new AMListActionInfo3(actionTitle, actionTitle),
				                                         AMListAction3::Sequential);

				continuousMoveAction->addSubAction(gratingAngleControl_->createDefaultsAction());

				// Set motion properties (velocities, accelerations, coordinated tolerance etc)
				AMListAction3* setMotionPropertiesAction = new AMListAction3(new AMListActionInfo3("Set motion properties",
				                                                                                   "Set motion properties"),
				                                                             AMListAction3::Parallel);

				double gratingAngleVelocity = qAbs(energyTrajectory->gratingAngleVelocityProfile().targetVelocity());
				double gratingAngleAcceleration = energyTrajectory->gratingAngleVelocityProfile().acceleration();

				setMotionPropertiesAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleControl_->stepVelocityControl(),
				                                                                                gratingAngleVelocity));

				setMotionPropertiesAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleControl_->stepAccelerationControl(),
				                                                                                gratingAngleAcceleration));

				setMotionPropertiesAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleControl_->movementTypeControl(),
				                                                                                0));

				double undulatorStepVelocity = qAbs(energyTrajectory->undulatorVelocityProfile().targetVelocity());

				double undulatorStepAcceleration = energyTrajectory->undulatorVelocityProfile().acceleration();

				if(energyPositionController_->isUndulatorTracking()) {

					setMotionPropertiesAction->addSubAction(AMActionSupport::buildControlMoveAction(undulatorControl_->stepAccelerationControl(),
					                                                                                undulatorStepAcceleration));

					setMotionPropertiesAction->addSubAction(AMActionSupport::buildControlMoveAction(undulatorControl_->stepVelocityControl(),
					                                                                                undulatorStepVelocity));
				}
				continuousMoveAction->addSubAction(setMotionPropertiesAction);

				AMChangeToleranceAction* continuousToleranceSet = new AMChangeToleranceAction(new AMChangeToleranceActionInfo(toInfo(), SGMENERGYCONTROL_COORDINATED_TOLERANCE), this);
				continuousMoveAction->addSubAction(continuousToleranceSet);

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

					waitForMotionPropertiesAction->addSubAction(AMActionSupport::buildControlWaitAction(undulatorControl_->stepAccelerationControl(),
					                                                                                    undulatorStepAcceleration,
					                                                                                    10,
					                                                                                    AMControlWaitActionInfo::MatchWithinTolerance));

					waitForMotionPropertiesAction->addSubAction(AMActionSupport::buildControlWaitAction(undulatorControl_->stepVelocityControl(),
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
					trajectoryMove->addSubAction(AMActionSupport::buildControlMoveAction(undulatorControl_->stepControl(),
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
					trajectoryWait->addSubAction(AMActionSupport::buildControlWaitAction(undulatorControl_->stepControl(),
					                                                                     undulatorStepsTarget,
					                                                                     60,
					                                                                     AMControlWaitActionInfo::MatchWithinTolerance));
				}


				continuousMoveAction->addSubAction(trajectoryWait);

				// Put the grating angle & undulator back to their default mode.
                                continuousMoveAction->addSubAction(gratingAngleControl_->createDefaultsAction());
				continuousMoveAction->addSubAction(createDefaultsAction());


			} else {
				AMErrorMon::alert(this, SGMENERGYCONTROL_INVALID_STATE, QString("Move would put energy in invalid state: %1").arg(energyTrajectory->errorValidator()->fullFailureMessage()));
			}

		}
	} else {

		AMErrorMon::alert(this, SGMENERGYCONTROL_ZERO_ENERGY_VELOCITY, QString("Velocity is invalid (cannot be zero)"));
	}

	return continuousMoveAction;
}

AMAction3 * SGMEnergyCoordinatorControl::createDefaultsAction()
{
	return new AMChangeToleranceAction(new AMChangeToleranceActionInfo(toInfo(), SGMENERGYCONTROL_CLOSEDLOOP_TOLERANCE),this);
}


