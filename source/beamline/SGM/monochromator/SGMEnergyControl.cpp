#include "SGMEnergyControl.h"
#include "SGMEnergyPosition.h"

#include "beamline/AMPVControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
SGMEnergyControl::SGMEnergyControl(QObject *parent)
    : AMPseudoMotorControl("Energy", "eV", parent, "SGM Monochromator Energy")
{
    gratingAnglePV_ = new AMPVwStatusControl("Grating Angle", "READ", "WRITE", "STATUS", "STOP", this);
    gratingTranslationPV_= new AMPVwStatusControl("Grating Translation", "BL1611-ID-1:AddOns:grating","BL1611-ID-1:AddOns:grating", "SMTR16114I1016:state", "SMTR16114I1016:emergStop", this);
    undulatorHarmonicPV_ = new AMPVwStatusControl("Grating Angle", "READ", "WRITE", "STATUS", "STOP", this);
    undulatorPositionPV_ = new AMPVwStatusControl("Grating Angle", "READ", "WRITE", "STATUS", "STOP", this);
    undulatorOffsetPV_ = new AMPVwStatusControl("Grating Angle", "READ", "WRITE", "STATUS", "STOP", this);
    exitSlitPositionPV_ = new AMPVwStatusControl("Grating Angle", "READ", "WRITE", "STATUS", "STOP", this);

    addChildControl(gratingAnglePV_);
    addChildControl(gratingTranslationPV_);
    addChildControl(undulatorHarmonicPV_);
    addChildControl(undulatorPositionPV_);
    addChildControl(undulatorOffsetPV_);
    addChildControl(exitSlitPositionPV_);

    connect(gratingAnglePV_, SIGNAL(valueChanged(double)), this, SLOT(onGratingAnglePVValueChanged(double)));
    connect(gratingTranslationPV_, SIGNAL(valueChanged(double)), this, SLOT(onGratingTranslationPVValueChanged(double)));
    connect(undulatorHarmonicPV_, SIGNAL(valueChanged(double)), this, SLOT(onUndulatorHarmonicPVValueChanged(double)));
    connect(undulatorPositionPV_, SIGNAL(valueChanged(double)), this, SLOT(onUndulatorPositionPVValueChanged(double)));
    connect(undulatorOffsetPV_, SIGNAL(valueChanged(double)), this, SLOT(onUndulatorOffsetPVValueChanged(double)));
    connect(exitSlitPositionPV_, SIGNAL(valueChanged(double)), this, SLOT(onExitSlitPositionPVValueChanged(double)));


    energyPositionController_ = 0;
}

bool SGMEnergyControl::isUndulatorTracking() const
{
    if(energyPositionController_) {
        return energyPositionController_->isUndulatorTracking();
    }

    return false;
}

void SGMEnergyControl::setUndulatorTracking(bool isTracking)
{
    if(energyPositionController_) {
        energyPositionController_->setUndulatorTracking(isTracking);
    }
}

bool SGMEnergyControl::isExitSlitTracking() const
{
    if(energyPositionController_) {
        return energyPositionController_->isExitSlitPositionTracking();
    }

    return false;
}

void SGMEnergyControl::setExitSlitTracking(bool isTracking)
{
    if(energyPositionController_) {
        energyPositionController_->setExitSlitPositionTracking(isTracking);
    }
}

void SGMEnergyControl::onEnergyPositionGratingAngleChanged(double gratingAngle)
{
    if(!gratingAnglePV_->withinTolerance(gratingAngle)) {

        gratingAnglePV_->move(gratingAngle);
    }
}

void SGMEnergyControl::onEnergyPositionGratingTranslationChanged(SGMGratingSupport::GratingTranslation gratingTranslation)
{
    if(gratingTranslation != SGMGratingSupport::UnknownGrating) {

        double gratingTranslationValue = double(int(gratingTranslation));

        if(!gratingTranslationPV_->withinTolerance(gratingTranslationValue)) {

            gratingTranslationPV_->move(gratingTranslationValue);
        }
    }
}

void SGMEnergyControl::onEnergyPositionUndulatorHamonicChanged(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic)
{

    if(undulatorHarmonic != SGMUndulatorSupport::UnknownUndulatorHarmonic) {

        double undulatorHarmonicValue = double(int(undulatorHarmonic));

        if(!undulatorHarmonicPV_->withinTolerance(undulatorHarmonicValue)) {

            undulatorHarmonicPV_->move(undulatorHarmonicValue);
        }
    }
}

void SGMEnergyControl::onEnergyPositionUndulatorPositionChanged(double undulatorPosition)
{
    if(!undulatorPositionPV_->withinTolerance(undulatorPosition)) {

        undulatorPositionPV_->move(undulatorPosition);
    }
}

void SGMEnergyControl::onEnergyPositionUndulatorOffsetChanged(double undulatorOffset)
{
    if(!undulatorOffsetPV_->withinTolerance(undulatorOffset)) {

        undulatorOffsetPV_->move(undulatorOffset);
    }
}

void SGMEnergyControl::onEnergyPositionExitSlitPositionChanged(double exitSlitPosition)
{

    if(!exitSlitPositionPV_->withinTolerance(exitSlitPosition)) {

        exitSlitPositionPV_->move(exitSlitPosition);
    }
}

void SGMEnergyControl::onEnergyPositionResultantEnergyChanged(double energy)
{
    setValue(energy);
}

void SGMEnergyControl::onGratingAnglePVValueChanged(double value)
{
    if(energyPositionController_) {
        energyPositionController_->setGratingAngle(value);
    }
}

void SGMEnergyControl::onGratingTranslationPVValueChanged(double value)
{
    SGMGratingSupport::GratingTranslation gratingTranslation = SGMGratingSupport::UnknownGrating;

    if(qAbs(value) < gratingTranslationPV_->tolerance()) {

        gratingTranslation = SGMGratingSupport::LowGrating;
    } else if(qAbs(value - 1) < gratingTranslationPV_->tolerance()) {

        gratingTranslation = SGMGratingSupport::MediumGrating;
    } else if(qAbs(value - 2) < gratingTranslationPV_->tolerance()) {

        gratingTranslation = SGMGratingSupport::HighGrating;
    }

    if(gratingTranslation != SGMGratingSupport::UnknownGrating &&
            energyPositionController_) {

        energyPositionController_->setGratingTranslation(gratingTranslation);
    }
}

void SGMEnergyControl::onUndulatorHarmonicPVValueChanged(double value)
{

    if(energyPositionController_) {
        SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic = SGMUndulatorSupport::UnknownUndulatorHarmonic;

        if(qAbs(value - 1) < undulatorHarmonicPV_->tolerance()) {

            undulatorHarmonic = SGMUndulatorSupport::FirstHarmonic;
        } else if(qAbs(value - 2) < undulatorHarmonicPV_->tolerance()) {

            undulatorHarmonic = SGMUndulatorSupport::ThirdHarmonic;
        }

        if(undulatorHarmonic != SGMUndulatorSupport::UnknownUndulatorHarmonic) {

            energyPositionController_->setUndulatorHarmonic(undulatorHarmonic);
        }
    }
}

void SGMEnergyControl::onUndulatorPositionPVValueChanged(double value)
{
    if(energyPositionController_) {
        energyPositionController_->setUndulatorPosition(value);
    }
}

void SGMEnergyControl::onUndulatorOffsetPVValueChanged(double value)
{
    if(energyPositionController_) {
        energyPositionController_->setUndulatorOffset(value);
    }
}

void SGMEnergyControl::onExitSlitPositionPVValueChanged(double value)
{
    if(energyPositionController_) {
        energyPositionController_->setExitSlitPosition(value);
    }
}

void SGMEnergyControl::updateConnected()
{
    setConnected(gratingAnglePV_->isConnected() &&
                 gratingTranslationPV_->isConnected() &&
                 undulatorHarmonicPV_->isConnected() &&
                 undulatorPositionPV_->isConnected() &&
                 undulatorOffsetPV_->isConnected() &&
                 exitSlitPositionPV_->isConnected());

    if(energyPositionController_ == 0 && isConnected()) {
        initializeEnergyPosition();
    }
}

void SGMEnergyControl::updateValue()
{
    /*
     * Not Used. Our value (which represents the energy) is updated when the energy
     * position class emits the energyChanged signal (see onMonoInfoEnergyChanged)
     */
}

void SGMEnergyControl::updateMoving()
{
    setIsMoving(gratingAnglePV_->isMoving() ||
                gratingTranslationPV_->isMoving() ||
                undulatorHarmonicPV_->isMoving() ||
                undulatorPositionPV_->isMoving() ||
                undulatorOffsetPV_->isMoving() ||
                exitSlitPositionPV_->isMoving());
}

AMAction3 *SGMEnergyControl::createMoveAction(double setpoint)
{
    AMListAction3* moveAction = 0;

    if(energyPositionController_) {
        // Create a cloned mono info to use in the calculation for our new component
        // positions:
        SGMEnergyPosition newMonochromatorEnergyInfo(value(), energyPositionController_->gratingTranslation());
        newMonochromatorEnergyInfo.setUndulatorTracking(energyPositionController_->isUndulatorTracking());
        newMonochromatorEnergyInfo.setExitSlitPositionTracking(energyPositionController_->isExitSlitPositionTracking());

        // Move our new mono info to the setpoint energy position:
        newMonochromatorEnergyInfo.requestEnergy(setpoint);


        if(!newMonochromatorEnergyInfo.hasErrors()) {

            moveAction = new AMListAction3(new AMListActionInfo("Moving to Energy",
                                                                "Performing coordinated move to energy"),
                                           AMListAction3::Sequential);

            // If we're at a valid energy, build a list of move actions which set
            // our component positions:

            AMListAction3* coordinatedMoveActionList = new AMListAction3(new AMListActionInfo3("Moving mono components",
                                                                                               "Moving mono components"),
                                                                         AMListAction3::Parallel);


            AMAction3* gratingAngleMoveAction = AMActionSupport::buildControlMoveAction(gratingAnglePV_, newMonochromatorEnergyInfo.gratingAngle(), false);
            AMAction3* undulatorPositionMoveAction = AMActionSupport::buildControlMoveAction(undulatorPositionPV_, newMonochromatorEnergyInfo.undulatorPosition(), false);
            AMAction3* exitSlitPositionMoveAction = AMActionSupport::buildControlMoveAction(exitSlitPositionPV_, newMonochromatorEnergyInfo.exitSlitPosition(), false);

            coordinatedMoveActionList->addSubAction(gratingAngleMoveAction);
            coordinatedMoveActionList->addSubAction(undulatorPositionMoveAction);
            coordinatedMoveActionList->addSubAction(exitSlitPositionMoveAction);

            // Build the wait actions to ensure all our moves are complete:
            AMListAction3* coordinatedWaitActionList = new AMListAction3(new AMListActionInfo3("Waiting for mono components",
                                                                                               "Waiting for mono components"),
                                                                         AMListAction3::Parallel);

            AMAction3* gratingAngleWaitAction = AMActionSupport::buildControlWaitAction(gratingAnglePV_,
                                                                                        newMonochromatorEnergyInfo.gratingAngle(),
                                                                                        120,
                                                                                        AMControlWaitActionInfo::MatchWithinTolerance);

            AMAction3* undulatorPositionWaitAction = AMActionSupport::buildControlWaitAction(undulatorPositionPV_,
                                                                                             newMonochromatorEnergyInfo.undulatorPosition(),
                                                                                             120,
                                                                                             AMControlWaitActionInfo::MatchWithinTolerance);

            AMAction3* exitSlitPositionWaitAction = AMActionSupport::buildControlWaitAction(exitSlitPositionPV_,
                                                                                            newMonochromatorEnergyInfo.exitSlitPosition(),
                                                                                            120,
                                                                                            AMControlWaitActionInfo::MatchWithinTolerance);

            coordinatedWaitActionList->addSubAction(gratingAngleWaitAction);
            coordinatedWaitActionList->addSubAction(undulatorPositionWaitAction);
            coordinatedWaitActionList->addSubAction(exitSlitPositionWaitAction);


            moveAction->addSubAction(coordinatedMoveActionList);
            moveAction->addSubAction(coordinatedWaitActionList);
        }
    }
    return moveAction;
}

void SGMEnergyControl::initializeEnergyPosition()
{
    // Initialize the mono info controller's data
    SGMGratingSupport::GratingTranslation gratingTranslation = SGMGratingSupport::UnknownGrating;
    SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic = SGMUndulatorSupport::UnknownUndulatorHarmonic;

    if(gratingTranslationPV_->withinTolerance(0)) {

        gratingTranslation = SGMGratingSupport::LowGrating;
    } else if(gratingTranslationPV_->withinTolerance(1)) {

        gratingTranslation = SGMGratingSupport::MediumGrating;
    } else if (gratingTranslationPV_->withinTolerance(2)) {

        gratingTranslation = SGMGratingSupport::HighGrating;
    }

    if(undulatorHarmonicPV_->withinTolerance(0)) {

        undulatorHarmonic = SGMUndulatorSupport::FirstHarmonic;
    } else if( undulatorHarmonicPV_->withinTolerance(1)) {

        undulatorHarmonic = SGMUndulatorSupport::ThirdHarmonic;
    }

    energyPositionController_ = new SGMEnergyPosition(gratingTranslation,
                                                   gratingAnglePV_->value(),
                                                   undulatorHarmonic,
                                                   undulatorPositionPV_->value(),
                                                   undulatorOffsetPV_->value(),
                                                   exitSlitPositionPV_->value());

    energyPositionController_->setUndulatorTracking(undulatorTrackingPV_->withinTolerance(1));
    energyPositionController_->setExitSlitPositionTracking(exitSlitTrackingPV_->withinTolerance(1));

    setValue(energyPositionController_->resultantEnergy());

    // Connect the mono info controller's signals
    connect(energyPositionController_, SIGNAL(gratingAngleChanged(double)), this, SLOT(onEnergyPositionGratingAngleChanged(double)));
    connect(energyPositionController_, SIGNAL(gratingTranslationChanged(SGMGratingSupport::GratingTranslation)), this, SLOT(onEnergyPositionGratingTranslationChanged(SGMGratingSupport::GratingTranslation)));
    connect(energyPositionController_, SIGNAL(undulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic)), this, SLOT(onEnergyPositionUndulatorHamonicChanged(SGMUndulatorSupport::UndulatorHarmonic)));
    connect(energyPositionController_, SIGNAL(undulatorTrackingChanged(bool)), this, SIGNAL(undulatorTrackingChanged(bool)));
    connect(energyPositionController_, SIGNAL(undulatorPositionChanged(double)), this, SLOT(onEnergyPositionUndulatorPositionChanged(double)));
    connect(energyPositionController_, SIGNAL(undulatorOffsetChanged(double)), this, SLOT(onEnergyPositionUndulatorOffsetChanged(double)));
    connect(energyPositionController_, SIGNAL(exitSlitPositionChanged(double)), this, SLOT(onEnergyPositionExitSlitPositionChanged(double)));
    connect(energyPositionController_, SIGNAL(exitSlitTrackingChanged(bool)), this, SIGNAL(exitSlitTrackingChanged(bool)));

    connect(energyPositionController_, SIGNAL(energyChanged(double)), this, SLOT(onEnergyPositionResultantEnergyChanged(double)));
}

