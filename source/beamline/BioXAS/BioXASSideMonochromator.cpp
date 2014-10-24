#include "BioXASSideMonochromator.h"
#include <QDebug>

BioXASSideMonochromator::BioXASSideMonochromator(QObject *parent) :
    QObject(parent)
{
    connected_ = false;
    region_ = None;

    phosphorPaddleMotor_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), QString("SMTR1607-5-I22-11"), QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), QString(":mm"),  true, 0.05, 2.0, this);
    braggMotor_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("SMTR1607-5-I22-12 BRAGG"), QString("SMTR1607-5-I22-12"), QString("SMTR1607-5-I22-12 BRAGG"), QString(":deg"), true, 0.05, 2.0, this);
    verticalMotor_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("SMTR1607-5-I22-13 VERTICAL"), QString("SMTR1607-5-I22-13"), QString("SMTR1607-5-I22-13 VERTICAL"), QString(":mm"),  true, 0.05, 2.0, this);
    lateralMotor_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("SMTR1607-5-I22-14 LATERAL"), QString("SMTR1607-5-I22-14"), QString("SMTR1607-5-I22-14 LATERAL"), QString(":mm"),  true, 0.05, 2.0, this);
    crystalExchangeMotor_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("SMTR1607-5-I22-22 XTAL XCHAGE"), QString("SMTR1607-5-I22-22"), QString("SMTR1607-5-I22-22 XTAL XCHAGE"), QString(":mm"),  true, 0.05, 2.0, this);
    crystal1PitchMotor_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), QString("SMTR1607-5-I22-23"), QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), QString(":V"),   true, 0.05, 2.0, this);
    crystal1RollMotor_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("SMTR1607-5-I22-24 XTAL 1 ROLL"), QString("SMTR1607-5-I22-24"), QString("SMTR1607-5-I22-24 XTAL 1 ROLL"), QString(":V"),   true, 0.05, 2.0, this);
    crystal2PitchMotor_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), QString("SMTR1607-5-I22-25"), QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), QString(":V"),   true, 0.05, 2.0, this);
    crystal2RollMotor_ = new BioXASCLSMAXvMotor(BioXASBeamlineDef::MonoMotor, QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), QString("SMTR1607-5-I22-26"), QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), QString(":V"),   true, 0.05, 2.0, this);

    braggMotorPower_ = new AMPVControl("BraggMotorPower", "SMTR1607-5-I22-12:power", "SMTR1607-5-I22-12:power", QString(), this);
    slitsClosed_ = new AMPVControl("SlitsClosed", "BL1607-5-I21:SlitsClosed", "BL1607-5-I21:SlitsOprCloseCmd", QString(), this);
    paddleOut_ = new AMPVControl("PaddleOut", "BL1607-5-I21:PaddleExtracted", "BL1607-I21:PaddleOprOutCmd", QString(), this);
    crystalChangeEnabled_ = new AMReadOnlyPVControl("CrystalStageMotorDisabled", "BL1607-5-I21:KeyStatus", this);
    stageAtCrystalChangePosition_ = new AMReadOnlyPVControl("AtCrystalChangePosition", "BL1607-5-I21:XtalChangePos", this);
    crystalChangeBrakeEnabled_ = new AMReadOnlyPVControl("BrakeEnabled", "BL1607-5-I21:BrakeOff", this);
    stageMotorAbs_ = new AMPVControl("StageMotorAbsolute", "SMTR1607-5-I21-12:deg:fbk", "SMTR1607-5-I21-12:deg", QString(), this);
    crystalChangeMotorRel_ = new AMPVControl("CrystalChangeMotorRelative", "SMTR1607-5-I21-22:step:rel", "SMTR1607-5-I21-22:step:fbk", QString(), this);
    crystalChangeMotorCWLimit_ = new AMReadOnlyPVControl("CrystalChangeMotorCWLimit", "SMTR1607-5-I21-22:cw", this);
    crystalChangeMotorCCWLimit_ = new AMReadOnlyPVControl("CrystalChangeMotorCCWLimit", "SMTR1607-5-I21-22:ccw", this);
    regionAStatus_ = new AMReadOnlyPVControl("RegionAStatus", "BL1607-5-I21:Region:A", this);
    regionBStatus_ = new AMReadOnlyPVControl("RegionBStatus", "BL1607-5-I21:Region:B", this);

    energy_ = new BioXASSideMonochromatorControl("EnergyEV", "BL1607-5-I21:Energy:EV:fbk", "BL1607-5-I21:Energy:EV", "BL1607-5-I21:Energy:status", QString(), this);

    connect( slitsClosed_, SIGNAL(valueChanged(double)), this, SLOT(onSlitsClosedChanged(double)) );
    connect( paddleOut_, SIGNAL(valueChanged(double)), this, SLOT(onPaddleOutChanged(double)) );
    connect( crystalChangeEnabled_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangeEnabled(double)) );
    connect( stageAtCrystalChangePosition_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangePositionStatusChanged(double)) );
    connect( crystalChangeBrakeEnabled_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangeBrakeEnabledChanged(double)) );
    connect( crystalChangeMotorCCWLimit_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangeMotorCCWLimitStatusChanged(double)) );
    connect( crystalChangeMotorCWLimit_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangeMotorCWLimitStatusChanged(double)) );
    connect( regionAStatus_, SIGNAL(valueChanged(double)), this, SLOT(onRegionChanged()) );
    connect( regionBStatus_, SIGNAL(valueChanged(double)), this, SLOT(onRegionChanged()) );

    connect( slitsClosed_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( paddleOut_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalChangeEnabled_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( stageAtCrystalChangePosition_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalChangeBrakeEnabled_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( stageMotorAbs_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalChangeMotorRel_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalChangeMotorCWLimit_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalChangeMotorCCWLimit_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
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

AMAction3* BioXASSideMonochromator::createRemovePaddleAction()
{
    if (!paddleOut_->isConnected())
        return 0;

    AMControlInfo setpoint = paddleOut_->toInfo();
    setpoint.setValue(1);
    AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, paddleOut_);

    return action;
}

AMAction3* BioXASSideMonochromator::createWaitForCrystalChangeEnabledAction()
{
    if (!crystalChangeEnabled_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalChangeEnabled_->toInfo();
    setpoint.setValue(1);
    AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(setpoint);
    AMControlWaitAction *action = new AMControlWaitAction(actionInfo, crystalChangeEnabled_);

    return action;
}

AMAction3* BioXASSideMonochromator::createMoveToCrystalChangePositionAction()
{
    return createMoveStageAction(55);
}

AMAction3* BioXASSideMonochromator::createWaitForBrakeDisabledAction()
{
    if (!crystalChangeBrakeEnabled_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalChangeBrakeEnabled_->toInfo();
    setpoint.setValue(1);
    AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(setpoint);
    AMControlWaitAction *action = new AMControlWaitAction(actionInfo, crystalChangeBrakeEnabled_);

    return action;
}

AMAction3* BioXASSideMonochromator::createMoveCrystalChangeMotorAction(int relDestination)
{
    if (!crystalChangeMotorRel_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalChangeMotorRel_->toInfo();
    setpoint.setValue(relDestination);
    AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, crystalChangeMotorRel_);

    return action;
}

AMAction3* BioXASSideMonochromator::createWaitForBrakeEnabledAction()
{
    if (!crystalChangeBrakeEnabled_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalChangeBrakeEnabled_->toInfo();
    setpoint.setValue(0);
    AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(setpoint);
    AMControlWaitAction *action = new AMControlWaitAction(actionInfo, crystalChangeBrakeEnabled_);

    return action;
}

AMAction3* BioXASSideMonochromator::createMoveStageAction(double degDestination)
{
    if (!stageMotorAbs_->isConnected())
        return 0;

    AMControlInfo setpoint = stageMotorAbs_->toInfo();
    setpoint.setValue(degDestination);
    AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, stageMotorAbs_);

    return action;
}


AMAction3* BioXASSideMonochromator::createWaitForCrystalChangeDisabledAction()
{
    if (!crystalChangeEnabled_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalChangeEnabled_->toInfo();
    setpoint.setValue(0);
    AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(setpoint);
    AMControlWaitAction *action = new AMControlWaitAction(actionInfo, crystalChangeEnabled_);

    return action;
}


AMAction3* BioXASSideMonochromator::createCrystalChangeAction()
{
    if (!connected_)
        return 0;

    double crystalChangeMotorDestination;
    double newRegionDestination;

    if (region_ == A) {
        crystalChangeMotorDestination = -12,000;
        newRegionDestination = 350;
    } else if (region_ == B) {
        crystalChangeMotorDestination = 12,000;
        newRegionDestination = 140;
    } else {
        return 0;
    }

    AMAction3 *closeSlits = createCloseSlitsAction();
    AMAction3 *removePaddle = createRemovePaddleAction();
    AMAction3 *turnKeyCCW = createWaitForCrystalChangeEnabledAction();
    AMAction3 *toCrystalChangePosition = createMoveToCrystalChangePositionAction();
    AMAction3 *disableBrake = createWaitForBrakeDisabledAction();
    AMAction3 *fromCrystalChangePosition = createMoveCrystalChangeMotorAction(crystalChangeMotorDestination);
    AMAction3 *enableBrake = createWaitForBrakeEnabledAction();
    AMAction3 *toNewRegion = createMoveStageAction(newRegionDestination);
    AMAction3 *turnKeyCW = createWaitForCrystalChangeDisabledAction();


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

AMAction3* BioXASSideMonochromator::createSetBraggMotorPowerOnAction()
{
    AMControlInfo setpoint = braggMotorPower_->toInfo();
    setpoint.setValue(1);
    AMControlMoveActionInfo3 *info = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(info, braggMotorPower_);

    return action;
}

AMAction3* BioXASSideMonochromator::createSetBraggMotorPowerAutoAction()
{
    AMControlInfo setpoint = braggMotorPower_->toInfo();
    setpoint.setValue(3);
    AMControlMoveActionInfo3 *info = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(info, braggMotorPower_);

    return action;
}

void BioXASSideMonochromator::onConnectedChanged()
{
    bool currentState = (slitsClosed_->isConnected() && paddleOut_->isConnected() &&
                         crystalChangeEnabled_->isConnected() && stageAtCrystalChangePosition_->isConnected() &&
                         crystalChangeBrakeEnabled_->isConnected() && stageMotorAbs_->isConnected() &&
                         crystalChangeMotorRel_->isConnected() && crystalChangeMotorCWLimit_->isConnected() &&
                         crystalChangeMotorCCWLimit_->isConnected() && regionAStatus_->isConnected() &&
                         regionBStatus_->isConnected() && energy_->isConnected());

    if (connected_ != currentState) {
        connected_ = currentState;
        emit connected(connected_);
    }
}

void BioXASSideMonochromator::onRegionChanged()
{
    Region newRegion;
    int regionAVal = (int)regionAStatus_->value();
    int regionBVal = (int)regionBStatus_->value();

    if (regionAVal == 0 && regionBVal == 0)
        newRegion = None;
    else if (regionAVal == 0 && regionBVal == 1)
        newRegion = B;
    else if (regionAVal == 1 && regionBVal == 0)
        newRegion = A;
    else if (regionAVal == 1 && regionBVal == 1)
        newRegion = None;
    else
        newRegion = None;

    if (region_ != newRegion) {
        region_ = newRegion;
        emit regionChanged(region_);
    }

}
