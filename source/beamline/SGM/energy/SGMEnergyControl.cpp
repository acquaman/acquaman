#include "SGMEnergyControl.h"

#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "SGMGratingTranslationStepControl.h"
#include "util/AMErrorMonitor.h"

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

	gratingTranslationStepControl_ = new SGMGratingTranslationStepControl(this);

    undulatorPositionControl_ = new AMPVwStatusControl("Undulator Position",
                                                       "UND1411-01:gap:mm:fbk",
                                                       "UND1411-01:gap:mm",
                                                       "UND1411-01:moveStatus",
													   "UND1411-01:stop",
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

    addChildControl(gratingAngleControl_);
	addChildControl(gratingTranslationStepControl_);
    addChildControl(undulatorPositionControl_);
    addChildControl(exitSlitPositionControl_);

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
            exitSlitPositionControl_->isConnected();

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
            SGMGratingSupport::encoderStepsToEnum(value);

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

		double newGratingTranslationPosition = SGMGratingSupport::enumToEncoderSteps(energyPositionController_->gratingTranslation());
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
			SGMGratingSupport::encoderStepsToEnum(gratingTranslationStepControl_->value());

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

            // Create list action to move all components.
            double gratingTranslationNewValue = SGMGratingSupport::enumToEncoderSteps(helperEnergyPosition->gratingTranslation());
            double gratingAngleNewValue = helperEnergyPosition->gratingAngle();
            double undulatorPositionNewValue = helperEnergyPosition->undulatorPosition();
            double exitSlitPositionNewValue = helperEnergyPosition->exitSlitPosition();

			componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingTranslationStepControl_, gratingTranslationNewValue, false));
			componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleControl_, gratingAngleNewValue, false));
			componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(undulatorPositionControl_, undulatorPositionNewValue, false));
			componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(exitSlitPositionControl_, exitSlitPositionNewValue, false));

            // Create list action to wait for component motions to complete.
            AMListAction3* componentWaitAction = new AMListAction3(new AMListActionInfo3("Waiting for energy components",
                                                                                         "Waiting for energy components to complete motions"),
                                                                   AMListAction3::Parallel);

			componentWaitAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingTranslationStepControl_, gratingTranslationNewValue, 60, AMControlWaitActionInfo::MatchWithinTolerance));
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

			AMErrorMon::alert(this, SGMENERGYCONTROL_INVALID_STATE, QString("Move would put energy in invalid state: %1").arg(helperEnergyPosition->errorMessage()));
		}

        helperEnergyPosition->deleteLater();
    }
	return coordinatedMoveAction;
}





