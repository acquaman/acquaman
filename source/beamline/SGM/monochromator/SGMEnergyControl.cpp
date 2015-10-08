#include "SGMEnergyControl.h"

#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"

#include "actions3/AMAction3.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include <QDebug>
SGMEnergyControl::SGMEnergyControl(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic,
                                   QObject *parent) :
    AMPseudoMotorControl("SGM Energy", "eV", parent, "SGM Monochromator Energy")
{

    energyPositionController_ = 0;
    startingUndulatorHarmonic_ = undulatorHarmonic;

    gratingAnglePV_ = new AMPVwStatusControl("Grating Angle",
                                             "SMTR16114I1002:enc:fbk",
                                             "SMTR16114I1002:encTarget",
                                             "SMTR16114I1002:status",
                                             "SMTR16114I1002:stop",
                                             this,
                                             5,
                                             2.0,
                                             new CLSMAXvControlStatusChecker(),
                                             1);

    gratingTranslationPV_ = new AMPVwStatusControl("Grating Translation",
                                                   "SG16114I1001:choice",
                                                   "SG16114I1001:choice",
                                                   "SMTR16114I1016:state",
                                                   "SMTR16114I1016:emergStop",
                                                   this,
                                                   0.1,
                                                   2.0,
                                                   new AMControlStatusCheckerStopped(0));

    undulatorPositionPV_ = new AMPVwStatusControl("Undulator Position",
                                                  "UND1411-01:gap:mm:fbk",
                                                  "UND1411-01:gap:mm",
                                                  "UND1411-01:moveStatus",
                                                  "SMTR1411-01:stop",
                                                  this,
                                                  0.1);

    exitSlitPositionPV_ = new AMPVwStatusControl("Exit Slit Position",
                                                 "PSL16114I1003:Y:mm:fbk",
                                                 "PSL16114I1003:Y:mm:encsp",
                                                 "SMTR16114I1003:status",
                                                 "SMTR16114I1003:stop",
                                                 this,
                                                 0.1,
                                                 2.0,
                                                 new AMControlStatusCheckerDefault(1));

    addChildControl(gratingAnglePV_);
    addChildControl(gratingTranslationPV_);
    addChildControl(undulatorPositionPV_);
    addChildControl(exitSlitPositionPV_);

	setMinimumValue(200);
	setMaximumValue(2500);
	setTolerance(0.1);
	setDisplayPrecision(5);
}

SGMUndulatorSupport::UndulatorHarmonic SGMEnergyControl::undulatorHarmonic() const
{
    if(energyPositionController_) {

        return energyPositionController_->undulatorHarmonic();
    }

    return SGMUndulatorSupport::UnknownUndulatorHarmonic;
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
	return shouldMeasure() && isConnected();
}

bool SGMEnergyControl::canMove() const
{
	return shouldMove() && isConnected();
}

bool SGMEnergyControl::canStop() const
{
	return shouldStop() && isConnected();
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

double SGMEnergyControl::gratingAngle() const
{
	if(energyPositionController_) {

		return energyPositionController_->gratingAngle();
	}

	return 0;
}

SGMGratingSupport::GratingTranslation SGMEnergyControl::gratingTranslation() const
{

	if(energyPositionController_) {

		return energyPositionController_->gratingTranslation();
	}

	return SGMGratingSupport::UnknownGrating;
}

double SGMEnergyControl::undulatorPosition() const
{
	if(energyPositionController_) {

		return energyPositionController_->undulatorPosition();
	}

	return 0;
}

double SGMEnergyControl::exitSlitPosition() const
{
	if(energyPositionController_) {

		return energyPositionController_->exitSlitPosition();
	}

	return 0;
}

void SGMEnergyControl::setUndulatorHarmonic(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic)
{
	if(energyPositionController_ &&
			energyPositionController_->undulatorHarmonic() != undulatorHarmonic) {

		energyPositionController_->setUndulatorHarmonic(undulatorHarmonic);
	}
}

void SGMEnergyControl::setUndulatorOffset(double undulatorOffset)
{
	if(energyPositionController_ &&
			energyPositionController_->undulatorOffset() != undulatorOffset) {

		energyPositionController_->setUndulatorOffset(undulatorOffset);
	}
}

void SGMEnergyControl::setUndulatorTracking(bool isTracking)
{
	if(energyPositionController_ &&
			energyPositionController_->isUndulatorTracking() != isTracking) {

		energyPositionController_->setUndulatorTracking(isTracking);
	}

}

void SGMEnergyControl::setGratingTranslationOptimizationMode(SGMEnergyPosition::GratingTranslationOptimizationMode gratingTranslationOptimizationMode)
{
	if(energyPositionController_ &&
			energyPositionController_->gratingTranslationOptimizationMode() != gratingTranslationOptimizationMode) {

		energyPositionController_->setGratingTranslationOptimizationMode(gratingTranslationOptimizationMode);
	}
}

void SGMEnergyControl::setExitSlitPositionTracking(bool isTracking)
{
	if(energyPositionController_ &&
			energyPositionController_->isExitSlitPositionTracking() != isTracking) {

		energyPositionController_->setExitSlitPositionTracking(isTracking);
	}
}

void SGMEnergyControl::updateConnected()
{
	bool isConnected =
			gratingAnglePV_->isConnected() &&
			gratingTranslationPV_->isConnected() &&
			undulatorPositionPV_->isConnected() &&
			exitSlitPositionPV_->isConnected();

	if(isConnected && !energyPositionController_) {

		initializeEnergyPositionController();
	}

	setConnected(isConnected);
}

void SGMEnergyControl::updateMoving()
{
    setIsMoving(gratingAnglePV_->isMoving() ||
                gratingTranslationPV_->isMoving() ||
                undulatorPositionPV_->isMoving() ||
                exitSlitPositionPV_->isMoving());
}

void SGMEnergyControl::updateValue()
{
   // updating of value performed by listening to energyPositionController_
   // energyChanged() signal.
}

void SGMEnergyControl::onGratingTranslationPVValueChanged(double value)
{
    if(energyPositionController_) {

        // If we're getting a signal from the PV, we have to force the energy position
        // controller to take the value.
        SGMEnergyPosition::GratingTranslationOptimizationMode savedMode = energyPositionController_->gratingTranslationOptimizationMode();

        energyPositionController_->blockSignals(true);
        energyPositionController_->setGratingTranslationOptimizationMode(SGMEnergyPosition::ManualMode);
        energyPositionController_->blockSignals(false);

        energyPositionController_->setGratingTranslation(gratingTranslationDoubleToEnum(value));

        energyPositionController_->blockSignals(true);
        energyPositionController_->setGratingTranslationOptimizationMode(savedMode);
        energyPositionController_->blockSignals(false);
    }
}

void SGMEnergyControl::onEnergyPositionControllerEnergyChanged(double value)
{
    setValue(value);
}

double SGMEnergyControl::gratingTranslationEnumToDouble(SGMGratingSupport::GratingTranslation gratingTranslationEnum)
{
    switch(gratingTranslationEnum) {
    case SGMGratingSupport::LowGrating:
        return 2;
    case SGMGratingSupport::MediumGrating:
        return 1;
    case SGMGratingSupport::HighGrating:
        return 0;
    default:
        return -1;
    }
}

SGMGratingSupport::GratingTranslation SGMEnergyControl::gratingTranslationDoubleToEnum(double gratingTranslationValue)
{
    int gratingTranslationInt = int(gratingTranslationValue);
    switch(gratingTranslationInt) {
    case -1:
        return SGMGratingSupport::UnknownGrating;
    case 0:
        return SGMGratingSupport::HighGrating;
    case 1:
        return SGMGratingSupport::MediumGrating;
    default:
        return SGMGratingSupport::LowGrating;
    }
}

void SGMEnergyControl::initializeEnergyPositionController()
{
    double startingGratingAngle = gratingAnglePV_->value();
    double startingUndulatorPosition = undulatorPositionPV_->value();
    double startingExitSlitPosition = exitSlitPositionPV_->value();
    SGMGratingSupport::GratingTranslation startingGratingTranslation =
            gratingTranslationDoubleToEnum(gratingTranslationPV_->value());

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
    connect(energyPositionController_, SIGNAL(exitSlitTrackingChanged(bool)),
            this, SIGNAL(exitSlitTrackingChanged(bool)));

    connect(energyPositionController_, SIGNAL(energyChanged(double)),
            this, SLOT(onEnergyPositionControllerEnergyChanged(double)));

	connect(gratingAnglePV_, SIGNAL(valueChanged(double)),
			energyPositionController_, SLOT(setGratingAngle(double)));
	connect(gratingTranslationPV_, SIGNAL(valueChanged(double)),
			this, SLOT(onGratingTranslationPVValueChanged(double)));
	connect(undulatorPositionPV_, SIGNAL(valueChanged(double)),
			energyPositionController_, SLOT(setUndulatorPosition(double)));
	connect(exitSlitPositionPV_, SIGNAL(valueChanged(double)),
			energyPositionController_, SLOT(setExitSlitPosition(double)));

	connect(energyPositionController_, SIGNAL(gratingAngleChanged(double)),
			this, SIGNAL(gratingAngleChanged(double)));
	connect(energyPositionController_, SIGNAL(gratingTranslationChanged(SGMGratingSupport::GratingTranslation)),
			this, SIGNAL(gratingTranslationChanged(SGMGratingSupport::GratingTranslation)));
	connect(energyPositionController_, SIGNAL(undulatorPositionChanged(double)),
			this, SIGNAL(undulatorPositionChanged(double)));
	connect(energyPositionController_, SIGNAL(exitSlitPositionChanged(double)),
			this, SIGNAL(exitSlitPositionChanged(double)));

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
			double gratingTranslationNewValue = gratingTranslationEnumToDouble(helperEnergyPosition->gratingTranslation());
			double gratingAngleNewValue = helperEnergyPosition->gratingAngle();
			double undulatorPositionNewValue = helperEnergyPosition->undulatorPosition();
			double exitSlitPositionNewValue = helperEnergyPosition->exitSlitPosition();

            componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingTranslationPV_, gratingTranslationNewValue, false));
            componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAnglePV_, gratingAngleNewValue, false));
            componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(undulatorPositionPV_, undulatorPositionNewValue, false));
            componentMoveAction->addSubAction(AMActionSupport::buildControlMoveAction(exitSlitPositionPV_, exitSlitPositionNewValue, false));

            // Create list action to wait for component motions to complete.
            AMListAction3* componentWaitAction = new AMListAction3(new AMListActionInfo3("Waiting for energy components",
                                                                                         "Waiting for energy components to complete motions"),
                                                                   AMListAction3::Parallel);

            componentWaitAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingTranslationPV_, gratingTranslationNewValue, 60, AMControlWaitActionInfo::MatchWithinTolerance));
            componentWaitAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingAnglePV_, gratingAngleNewValue, 60, AMControlWaitActionInfo::MatchWithinTolerance));
            componentWaitAction->addSubAction(AMActionSupport::buildControlWaitAction(undulatorPositionPV_, undulatorPositionNewValue, 60, AMControlWaitActionInfo::MatchWithinTolerance));
            componentWaitAction->addSubAction(AMActionSupport::buildControlWaitAction(exitSlitPositionPV_, exitSlitPositionNewValue, 60, AMControlWaitActionInfo::MatchWithinTolerance));

            QString moveActionName = QString("Moving to energy %1eV").arg(setpoint);
            coordinatedMoveAction = new AMListAction3(new AMListActionInfo3(moveActionName,
                                                                            moveActionName),
                                                      AMListAction3::Sequential);

            coordinatedMoveAction->addSubAction(componentMoveAction);
            coordinatedMoveAction->addSubAction(componentWaitAction);

			// while testing just output the returned action, don't return it. Don't want
			// to move anything quite yet.
			qDebug() << QString("Request to move to energy %1eV").arg(setpoint);
			qDebug() << QString("Actually targetted %1eV").arg(helperEnergyPosition->resultantEnergy());
			qDebug() << QString("Grating Translation moved to %1").arg(gratingTranslationNewValue);
			qDebug() << QString("Grating Angle moved to %1").arg(gratingAngleNewValue);
			qDebug() << QString("Undulator Position moved to %1").arg(undulatorPositionNewValue);
			qDebug() << QString("Exit Slit Position moved to %1").arg(exitSlitPositionNewValue);
        }

		if(!coordinatedMoveAction) {
			qDebug() << QString("No move action created due to errors: %1").arg(helperEnergyPosition->errorMessage());
		}

		helperEnergyPosition->deleteLater();
    }



    return 0;
	//return coordinatedMoveAction;
}




