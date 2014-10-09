#include "BioXASSideMonochromator.h"
#include <QDebug>

BioXASSideMonochromator::BioXASSideMonochromator(QObject *parent) :
    QObject(parent)
{
    connected_ = false;

    slitsClosed_ = new AMPVControl("SlitsClosed", "BL1607-5-I21:SlitsClosed", "BL1607-5-I21:SlitsOprCloseCmd", QString(), this);
    paddleOut_ = new AMPVControl("PaddleOut", "BL1607-5-I21:PaddleExtracted", "BL1607-I21:PaddleOprOutCmd", QString(), this);
    keyStatus_ = new AMReadOnlyPVControl("CrystalStageMotorDisabled", "BL1607-5-I21:KeyStatus", this);
    crystalChangePositionStatus_ = new AMReadOnlyPVControl("AtCrystalChangePosition", "BL1607-5-I21:XtalChangePos", this);
    brakeEnabledStatus_ = new AMReadOnlyPVControl("BrakeEnabled", "BL1607-5-I21:BrakeOff", this);
    crystalStageMotorAbs_ = new AMPVControl("CrystalStageMotorAbsolute", "SMTR1607-5-I21-12:deg:fbk", "SMTR1607-5-I21-12:deg", QString(), this);
    crystalStageMotorRel_ = new AMPVControl("CrystalStageMotorRelative", "SMTR1607-5-I21-12:step:rel", "SMTR1607-5-I21-12:step:fbk", QString(), this);
    crystalStageCWLimitStatus_ = new AMReadOnlyPVControl("CrystalStageCWLimit", "SMTR1607-5-I21-22:cw", this);
    crystalStageCCWLimitStatus_ = new AMReadOnlyPVControl("CrystalStageCCWLimit", "SMTR1607-5-I21-22:ccw", this);
    regionAStatus_ = new AMReadOnlyPVControl("RegionAStatus", "BL1607-5-I21:Region:A", this);
    regionBStatus_ = new AMReadOnlyPVControl("RegionBStatus", "BL1607-5-I21:Region:B", this);

    energy_ = new BioXASSideMonochromatorControl("EnergyEV", "BL1607-5-I21:Energy:EV:fbk", "BL1607-5-I21:Energy:EV", "BL1607-5-I21:Energy:status", QString(), this);

    connect( slitsClosed_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( paddleOut_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( keyStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalChangePositionStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( brakeEnabledStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalStageMotorAbs_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalStageMotorRel_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalStageCWLimitStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalStageCCWLimitStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( regionAStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( regionBStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( energy_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

    onConnectedChanged();
}

BioXASSideMonochromator::~BioXASSideMonochromator()
{

}

AMAction3* BioXASSideMonochromator::createCloseSlitsAction()
{
    if (!slitsClosed_->isConnected())
        return 0;

    AMControlInfo setpoint = slitsClosed_->toInfo();
    setpoint.setValue(1);
    AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, slitsClosed_);

    return action;
}

AMAction3* BioXASSideMonochromator::createPaddleOutAction()
{
    if (!paddleOut_->isConnected())
        return 0;

    AMControlInfo setpoint = paddleOut_->toInfo();
    setpoint.setValue(1);
    AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, paddleOut_);

    return action;
}

AMAction3* BioXASSideMonochromator::createWaitForKeyStatusEnabledAction()
{
    if (!keyStatus_->isConnected())
        return 0;

    AMControlInfo setpoint = keyStatus_->toInfo();
    setpoint.setValue(1);
    AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(setpoint);
    AMControlWaitAction *action = new AMControlWaitAction(actionInfo, keyStatus_);

    return action;
}

AMAction3* BioXASSideMonochromator::createMoveToCrystalChangePositionAction()
{
    if (!crystalStageMotorAbs_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalStageMotorAbs_->toInfo();
    setpoint.setValue(55);
    AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, crystalStageMotorAbs_);

    return action;
}

AMAction3* BioXASSideMonochromator::createWaitForBrakeDisabledAction()
{
    if (!brakeEnabledStatus_->isConnected())
        return 0;

    AMControlInfo setpoint = brakeEnabledStatus_->toInfo();
    setpoint.setValue(1);
    AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(setpoint);
    AMControlWaitAction *action = new AMControlWaitAction(actionInfo, brakeEnabledStatus_);

    return action;
}

AMAction3* BioXASSideMonochromator::createMoveOffCrystalChangePositionAction(int relDestination)
{
    if (!crystalStageMotorRel_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalStageMotorRel_->toInfo();
    setpoint.setValue(relDestination);
    AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, crystalStageMotorRel_);

    return action;
}

AMAction3* BioXASSideMonochromator::createWaitForBrakeEnabledAction()
{
    if (!brakeEnabledStatus_->isConnected())
        return 0;

    AMControlInfo setpoint = brakeEnabledStatus_->toInfo();
    setpoint.setValue(0);
    AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(setpoint);
    AMControlWaitAction *action = new AMControlWaitAction(actionInfo, brakeEnabledStatus_);

    return action;
}

AMAction3* BioXASSideMonochromator::createSetCrystalStageAngleAction(double degDestination)
{
    if (!crystalStageMotorAbs_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalStageMotorAbs_->toInfo();
    setpoint.setValue(degDestination);
    AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, crystalStageMotorAbs_);

    return action;
}


AMAction3* BioXASSideMonochromator::createWaitForKeyStatusDisabledAction()
{
    if (!keyStatus_->isConnected())
        return 0;

    AMControlInfo setpoint = keyStatus_->toInfo();
    setpoint.setValue(0);
    AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(setpoint);
    AMControlWaitAction *action = new AMControlWaitAction(actionInfo, keyStatus_);

    return action;
}


AMAction3* BioXASSideMonochromator::createWaitForCrystalChangeAction()
{
    bool startRegionA, endRegionA;
    double moveOffDest, moveToRegion;

    startRegionA = ((int)regionAStatus_->value() == 0);
    endRegionA = !startRegionA;

    if (endRegionA) {
        moveOffDest = 12,000;
        moveToRegion = 140;
    } else {
        moveOffDest = -12,000;
        moveToRegion = 350;
    }

    AMAction3 *closeSlits = createCloseSlitsAction();
    AMAction3 *removePaddle = createPaddleOutAction();
    AMAction3 *turnKeyCCW = createWaitForKeyStatusEnabledAction();
    AMAction3 *toCrystalChangePosition = createMoveToCrystalChangePositionAction();
    AMAction3 *disableBrake = createWaitForBrakeDisabledAction();
    AMAction3 *fromCrystalChangePosition = createMoveOffCrystalChangePositionAction(moveOffDest);
    AMAction3 *enableBrake = createWaitForBrakeEnabledAction();
    AMAction3 *toNewRegion = createSetCrystalStageAngleAction(moveToRegion);
    AMAction3 *turnKeyCW = createWaitForKeyStatusDisabledAction();


    AMListAction3* crystalChangeAction = new AMListAction3(new AMListActionInfo3("Crystal Change Action", "Crystal Change Action"));
    crystalChangeAction->addSubAction(closeSlits);
    crystalChangeAction->addSubAction(removePaddle);
    crystalChangeAction->addSubAction(turnKeyCCW);
    crystalChangeAction->addSubAction(toCrystalChangePosition);
    crystalChangeAction->addSubAction(disableBrake);
    crystalChangeAction->addSubAction(fromCrystalChangePosition);
    crystalChangeAction->addSubAction(enableBrake);
    crystalChangeAction->addSubAction(toNewRegion);
    crystalChangeAction->addSubAction(turnKeyCW);

    return crystalChangeAction;

}

AMAction3* BioXASSideMonochromator::createSetEnergyAction(double newEnergy)
{
    if (!energy_->isConnected())
        return 0;

    AMControlInfo setpoint = energy_->toInfo();
    setpoint.setValue(newEnergy);
    AMControlMoveActionInfo3* actionInfo = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3* action = new AMControlMoveAction3(actionInfo, energy_);

    return action;
}

void BioXASSideMonochromator::onConnectedChanged()
{
    bool currentState = (slitsClosed_->isConnected() && paddleOut_->isConnected() &&
                         keyStatus_->isConnected() && crystalChangePositionStatus_->isConnected() &&
                         brakeEnabledStatus_->isConnected() && crystalStageMotorAbs_->isConnected() &&
                         crystalStageMotorRel_->isConnected() && crystalStageCWLimitStatus_->isConnected() &&
                         crystalStageCCWLimitStatus_->isConnected() && regionAStatus_->isConnected() &&
                         regionBStatus_->isConnected() && energy_->isConnected());

    if (connected_ != currentState) {
        connected_ = currentState;
        emit connected(connected_);
    }
}
