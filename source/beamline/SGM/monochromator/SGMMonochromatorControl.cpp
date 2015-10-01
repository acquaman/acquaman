#include "SGMMonochromatorControl.h"
#include "SGMMonochromatorInfo.h"

#include "beamline/AMPVControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
SGMMonochromatorControl::SGMMonochromatorControl(QObject *parent)
    : AMPseudoMotorControl("Energy", "eV", parent, "SGM Monochromator Energy")
{
    gratingAnglePV_ = new AMPVwStatusControl("Grating Angle", "READ", "WRITE", "STATUS", "STOP", this);
    gratingTranslationPV_= new AMPVwStatusControl("Grating Angle", "READ", "WRITE", "STATUS", "STOP", this);
    undulatorHarmonicPV_ = new AMPVwStatusControl("Grating Angle", "READ", "WRITE", "STATUS", "STOP", this);
    undulatorTrackingPV_ = new AMSinglePVControl("Undulator Tracking", "READ", this, 0.5);
    undulatorPositionPV_ = new AMPVwStatusControl("Grating Angle", "READ", "WRITE", "STATUS", "STOP", this);
    undulatorOffsetPV_ = new AMPVwStatusControl("Grating Angle", "READ", "WRITE", "STATUS", "STOP", this);
    exitSlitTrackingPV_ = new AMSinglePVControl("Exit Slit Tracking", "READ", this, 0.5);
    exitSlitPositionPV_ = new AMPVwStatusControl("Grating Angle", "READ", "WRITE", "STATUS", "STOP", this);

    addChildControl(gratingAnglePV_);
    addChildControl(gratingTranslationPV_);
    addChildControl(undulatorHarmonicPV_);
    addChildControl(undulatorTrackingPV_);
    addChildControl(undulatorPositionPV_);
    addChildControl(undulatorOffsetPV_);
    addChildControl(exitSlitTrackingPV_);
    addChildControl(exitSlitPositionPV_);

    connect(gratingAnglePV_, SIGNAL(valueChanged(double)), this, SLOT(onGratingAnglePVValueChanged(double)));
    connect(gratingTranslationPV_, SIGNAL(valueChanged(double)), this, SLOT(onGratingTranslationPVValueChanged(double)));
    connect(undulatorHarmonicPV_, SIGNAL(valueChanged(double)), this, SLOT(onUndulatorHarmonicPVValueChanged(double)));
    connect(undulatorTrackingPV_, SIGNAL(valueChanged(double)), this, SLOT(onUndulatorTrackingPVValueChanged(double)));
    connect(undulatorPositionPV_, SIGNAL(valueChanged(double)), this, SLOT(onUndulatorPositionPVValueChanged(double)));
    connect(undulatorOffsetPV_, SIGNAL(valueChanged(double)), this, SLOT(onUndulatorOffsetPVValueChanged(double)));
    connect(exitSlitTrackingPV_, SIGNAL(valueChanged(double)), this, SLOT(onExitSlitTrackingPVValueChanged(double)));
    connect(exitSlitPositionPV_, SIGNAL(valueChanged(double)), this, SLOT(onExitSlitPositionPVValueChanged(double)));


    monoInfoController_ = 0;
}

void SGMMonochromatorControl::onMonoInfoGratingAngleChanged(double gratingAngle)
{
    if(!gratingAnglePV_->withinTolerance(gratingAngle)) {

        gratingAnglePV_->move(gratingAngle);
    }
}

void SGMMonochromatorControl::onMonoInfoGratingTranslationChanged(SGMGratingSupport::GratingTranslation gratingTranslation)
{
    if(gratingTranslation != SGMGratingSupport::UnknownGrating) {

        double gratingTranslationValue = double(int(gratingTranslation));

        if(!gratingTranslationPV_->withinTolerance(gratingTranslationValue)) {

            gratingTranslationPV_->move(gratingTranslationValue);
        }
    }
}

void SGMMonochromatorControl::onMonoInfoUndulatorHamonicChanged(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic)
{

    if(undulatorHarmonic != SGMUndulatorSupport::UnknownUndulatorHarmonic) {

        double undulatorHarmonicValue = double(int(undulatorHarmonic));

        if(!undulatorHarmonicPV_->withinTolerance(undulatorHarmonicValue)) {

            undulatorHarmonicPV_->move(undulatorHarmonicValue);
        }
    }
}

void SGMMonochromatorControl::onMonoInfoUndulatorTrackingChanged(bool isTracking)
{
    double trackingValue;
    isTracking ? trackingValue = 1 : trackingValue = 0;

    if(!undulatorTrackingPV_->withinTolerance(trackingValue)) {

        undulatorTrackingPV_->move(trackingValue);
    }
}

void SGMMonochromatorControl::onMonoInfoUndulatorPositionChanged(double undulatorPosition)
{
    if(!undulatorPositionPV_->withinTolerance(undulatorPosition)) {

        undulatorPositionPV_->move(undulatorPosition);
    }
}

void SGMMonochromatorControl::onMonoInfoUndulatorOffsetChanged(double undulatorOffset)
{
    if(!undulatorOffsetPV_->withinTolerance(undulatorOffset)) {

        undulatorOffsetPV_->move(undulatorOffset);
    }
}

void SGMMonochromatorControl::onMonoInfoExitSlitTrackingChanged(bool exitSlitTracking)
{

    double exitSlitTrackingValue;

    exitSlitTracking ? exitSlitTrackingValue = 1 : exitSlitTrackingValue = 0;

    if(!exitSlitTrackingPV_->withinTolerance(exitSlitTrackingValue)) {

        exitSlitTrackingPV_->move(exitSlitTrackingValue);
    }
}

void SGMMonochromatorControl::onMonoInfoExitSlitPositionChanged(double exitSlitPosition)
{

    if(!exitSlitPositionPV_->withinTolerance(exitSlitPosition)) {

        exitSlitPositionPV_->move(exitSlitPosition);
    }
}

void SGMMonochromatorControl::onMonoInfoEnergyChanged(double energy)
{
    setValue(energy);
}

void SGMMonochromatorControl::onGratingAnglePVValueChanged(double value)
{
    monoInfoController_->setGratingAngle(value);
}

void SGMMonochromatorControl::onGratingTranslationPVValueChanged(double value)
{
    SGMGratingSupport::GratingTranslation gratingTranslation = SGMGratingSupport::UnknownGrating;

    if(qAbs(value) < gratingTranslationPV_->tolerance()) {

        gratingTranslation = SGMGratingSupport::LowGrating;
    } else if(qAbs(value - 1) < gratingTranslationPV_->tolerance()) {

        gratingTranslation = SGMGratingSupport::MediumGrating;
    } else if(qAbs(value - 2) < gratingTranslationPV_->tolerance()) {

        gratingTranslation = SGMGratingSupport::HighGrating;
    }

    if(gratingTranslation != SGMGratingSupport::UnknownGrating) {

        monoInfoController_->setGratingTranslation(gratingTranslation);
    }
}

void SGMMonochromatorControl::onUndulatorHarmonicPVValueChanged(double value)
{
    SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic = SGMUndulatorSupport::UnknownUndulatorHarmonic;

    if(qAbs(value - 1) < undulatorHarmonicPV_->tolerance()) {

        undulatorHarmonic = SGMUndulatorSupport::FirstHarmonic;
    } else if(qAbs(value - 2) < undulatorHarmonicPV_->tolerance()) {

        undulatorHarmonic = SGMUndulatorSupport::ThirdHarmonic;
    }

    if(undulatorHarmonic != SGMUndulatorSupport::UnknownUndulatorHarmonic) {

        monoInfoController_->setUndulatorHarmonic(undulatorHarmonic);
    }
}

void SGMMonochromatorControl::onUndulatorTrackingPVValueChanged(double value)
{
    bool isTracking = true;

    if(qAbs(value) < undulatorTrackingPV_->tolerance()) {

        isTracking = false;
    }

    monoInfoController_->setUndulatorTracking(isTracking);
}

void SGMMonochromatorControl::onUndulatorPositionPVValueChanged(double value)
{
    monoInfoController_->setUndulatorPosition(value);
}

void SGMMonochromatorControl::onUndulatorOffsetPVValueChanged(double value)
{
    monoInfoController_->setUndulatorOffset(value);
}

void SGMMonochromatorControl::onExitSlitTrackingPVValueChanged(double value)
{
    bool isTracking = true;

    if(qAbs(value) < exitSlitTrackingPV_->tolerance()) {

        isTracking = false;
    }

    monoInfoController_->setExitSlitPositionTracking(isTracking);
}

void SGMMonochromatorControl::onExitSlitPositionPVValueChanged(double value)
{
    monoInfoController_->setExitSlitPosition(value);
}

void SGMMonochromatorControl::updateConnected()
{
    setConnected(gratingAnglePV_->isConnected() &&
                 gratingTranslationPV_->isConnected() &&
                 undulatorHarmonicPV_->isConnected() &&
                 undulatorTrackingPV_->isConnected() &&
                 undulatorPositionPV_->isConnected() &&
                 undulatorOffsetPV_->isConnected() &&
                 exitSlitTrackingPV_->isConnected() &&
                 exitSlitPositionPV_->isConnected());

    if(monoInfoController_ == 0 && isConnected()) {
        initializeMonoInfoController();
    }
}

void SGMMonochromatorControl::updateValue()
{
    /*
     * Not Used. Our value (which represents the energy) is updated when the mono
     * info class emits the energyChanged signal (see onMonoInfoEnergyChanged)
     */
}

void SGMMonochromatorControl::updateMoving()
{
    setIsMoving(gratingAnglePV_->isMoving() ||
                gratingTranslationPV_->isMoving() ||
                undulatorHarmonicPV_->isMoving() ||
                undulatorTrackingPV_->isMoving() ||
                undulatorPositionPV_->isMoving() ||
                undulatorOffsetPV_->isMoving() ||
                exitSlitTrackingPV_->isMoving() ||
                exitSlitPositionPV_->isMoving());
}

AMAction3 *SGMMonochromatorControl::createMoveAction(double setpoint)
{
    AMListAction3* moveAction = 0;

    // Create a cloned mono info to use in the calculation for our new component
    // positions:
    SGMMonochromatorInfo newMonochromatorEnergyInfo(value(), monoInfoController_->gratingTranslation());
    newMonochromatorEnergyInfo.setUndulatorTracking(monoInfoController_->isUndulatorTracking());
    newMonochromatorEnergyInfo.setExitSlitPositionTracking(monoInfoController_->isExitSlitPositionTracking());

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

    return moveAction;
}

void SGMMonochromatorControl::initializeMonoInfoController()
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

    monoInfoController_ = new SGMMonochromatorInfo(gratingTranslation,
                                                   gratingAnglePV_->value(),
                                                   undulatorHarmonic,
                                                   undulatorPositionPV_->value(),
                                                   undulatorOffsetPV_->value(),
                                                   exitSlitPositionPV_->value());

    monoInfoController_->setUndulatorTracking(undulatorTrackingPV_->withinTolerance(1));
    monoInfoController_->setExitSlitPositionTracking(exitSlitTrackingPV_->withinTolerance(1));

    setValue(monoInfoController_->resultantEnergy());

    // Connect the mono info controller's signals
    connect(monoInfoController_, SIGNAL(gratingAngleChanged(double)), this, SLOT(onMonoInfoGratingAngleChanged(double)));
    connect(monoInfoController_, SIGNAL(gratingTranslationChanged(SGMGratingSupport::GratingTranslation)), this, SLOT(onMonoInfoGratingTranslationChanged(SGMGratingSupport::GratingTranslation)));
    connect(monoInfoController_, SIGNAL(undulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic)), this, SLOT(onMonoInfoUndulatorHamonicChanged(SGMUndulatorSupport::UndulatorHarmonic)));
    connect(monoInfoController_, SIGNAL(undulatorTrackingChanged(bool)), this, SLOT(onMonoInfoUndulatorTrackingChanged(bool)));
    connect(monoInfoController_, SIGNAL(undulatorPositionChanged(double)), this, SLOT(onMonoInfoUndulatorPositionChanged(double)));
    connect(monoInfoController_, SIGNAL(undulatorOffsetChanged(double)), this, SLOT(onMonoInfoUndulatorOffsetChanged(double)));
    connect(monoInfoController_, SIGNAL(exitSlitPositionChanged(double)), this, SLOT(onMonoInfoExitSlitPositionChanged(double)));
    connect(monoInfoController_, SIGNAL(exitSlitTrackingChanged(bool)), this, SLOT(onMonoInfoExitSlitTrackingChanged(bool)));

    connect(monoInfoController_, SIGNAL(energyChanged(double)), this, SLOT(onMonoInfoEnergyChanged(double)));
}

