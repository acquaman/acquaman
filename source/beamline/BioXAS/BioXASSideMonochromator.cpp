#include "BioXASSideMonochromator.h"
#include <QDebug>

BioXASSideMonochromator::BioXASSideMonochromator(QObject *parent) :
    QObject(parent)
{
    connected_ = false;

    tankClosed_ = new AMSinglePVControl("BL1607-I22:TankClosed", "BL1607-I22:TankClosed", this);
    brakeOff_ = new AMSinglePVControl("BL1607-I22:BrakeOff", "BL1607-I22:BrakeOff", this);
    keyStatus_ = new AMSinglePVControl("BL1607-I22:KeyStatus", "BL1607-I22:KeyStatus", this);
    paddleExtracted_ = new AMSinglePVControl("BL1607-I22:PaddleExtracted", "BL1607-I22:PaddleExtracted", this);
    crystalChangePosition_ = new AMSinglePVControl("BL1607-I22:XtalChangePos", "BL1607-I22:XtalChangePos", this);
    upperSlitClosed_ = new AMSinglePVControl("BL1607-I22:UpperSlitClosed", "BL1607-I22:UpperSlitClosed", this);
    lowerSlitClosed_ = new AMSinglePVControl("BL1607-I22:LowerSlitClosed", "BL1607-I22:LowerSlitClosed", this);
    slitsClosed_ = new AMSinglePVControl("BL1607-I22:SlitsClosed", "BL1607-I22:SlitsClosed", this);
    regionAThetaCWLimit_ = new AMSinglePVControl("BL1607-I22:Theta:A:CWLIM", "BL1607-I22:Theta:A:CWLIM", this);
    regionAThetaCCWLimit_ = new AMSinglePVControl("BL1607-I22:Theta:A:CCWLIM", "BL1607-I22:Theta:A:CCWLIM", this);
    regionBThetaCWLimit_ = new AMSinglePVControl("BL1607-I22:Theta:B:CWLIM", "BL1607-I22:Theta:B:CWLIM", this);
    regionBThetaCCWLimit_ = new AMSinglePVControl("BL1607-I22:Theta:B:CCWLIM", "BL1607-I22:Theta:B:CCWLIM", this);
    regionA_ = new AMSinglePVControl("BL1607-I22:Region:A", "BL1607-I22:Region:A", this);
    regionB_ = new AMSinglePVControl("BL1607-I22:Region:B", "BL1607-I22:Region:B", this);
    regionATranslateLimit_ = new AMSinglePVControl("BL1607-I22:Translate:A:Limit", "BL1607-I22:Translate:A:Limit", this);
    regionBTranslateLimit_ = new AMSinglePVControl("BL1607-I22:Translate:B:Limit", "BL1607-I22:Translate:B:Limit", this);
    translatePermitted_ = new AMSinglePVControl("BL1607-I22:Translate:Permited", "BL1607-I22:Translate:Permited", this);
    thetaPaddlePermitted_ = new AMSinglePVControl("BL1607-I22:ThetaPaddle:Permited", "BL1607-I22:ThetaPaddle:Permited", this);
    slitsCloseCmd_ = new AMSinglePVControl("BL1607-I22:SlitsCloseCmd", "BL1607-I22:SlitsCloseCmd", this);
    slitsEnabled_ = new AMSinglePVControl("BL1607-I22:SlitsEnabled", "BL1607-I22:SlitsEnabled", this);

    energy_ = new BioXASSideMonochromatorControl("Energy Setpoint", "BL1607-5-I22:Energy:EV:fbk", "BL1607-5-I22:Energy:EV", "BL1607-5-I22:Energy:status", QString(), this);

    connect( tankClosed_, SIGNAL(valueChanged(double)), this, SIGNAL(tankClosedChanged(double)) );
    connect( brakeOff_, SIGNAL(valueChanged(double)), this, SIGNAL(brakeStatusChanged(double)) );
    connect( keyStatus_, SIGNAL(valueChanged(double)), this, SIGNAL(keyStatusChanged(double)) );
    connect( paddleExtracted_, SIGNAL(valueChanged(double)), this, SIGNAL(paddleExtractedChanged(double)) );
    connect( crystalChangePosition_, SIGNAL(valueChanged(double)), this, SIGNAL(crystalChangePosition(double)) );
    connect( upperSlitClosed_, SIGNAL(valueChanged(double)), this, SIGNAL(upperSlitClosedChanged(double)) );
    connect( lowerSlitClosed_, SIGNAL(valueChanged(double)), this, SIGNAL(lowerSlitClosedChanged(double)) );
    connect( slitsClosed_, SIGNAL(valueChanged(double)), this, SIGNAL(slitsClosedChanged(double)) );
    connect( regionAThetaCWLimit_, SIGNAL(valueChanged(double)), this, SIGNAL(regionAThetaCWLimit(double)) );
    connect( regionAThetaCCWLimit_, SIGNAL(valueChanged(double)), this, SIGNAL(regionAThetaCCWLimit(double)) );
    connect( regionBThetaCWLimit_, SIGNAL(valueChanged(double)), this, SIGNAL(regionBThetaCWLimit(double)) );
    connect( regionBThetaCCWLimit_, SIGNAL(valueChanged(double)), this, SIGNAL(regionBThetaCCWLimit(double)) );
    connect( regionA_, SIGNAL(valueChanged(double)), this, SIGNAL(regionA(double)) );
    connect( regionB_, SIGNAL(valueChanged(double)), this, SIGNAL(regionB(double)) );
    connect( regionATranslateLimit_, SIGNAL(valueChanged(double)), this, SIGNAL(regionATranslateLimit(double)) );
    connect( regionBTranslateLimit_, SIGNAL(valueChanged(double)), this, SIGNAL(regionBTranslateLimit(double)) );
    connect( translatePermitted_, SIGNAL(valueChanged(double)), this, SIGNAL(translatePermitted(double)) );
    connect( thetaPaddlePermitted_, SIGNAL(valueChanged(double)), this, SIGNAL(thetaPaddlePermitted(double)) );
    connect( slitsEnabled_, SIGNAL(valueChanged(double)), this, SIGNAL(slitsEnabled(double)) );
    connect( energy_, SIGNAL(setpointChanged(double)), this, SIGNAL(energySetpointChanged(double)) );
    connect( energy_, SIGNAL(valueChanged(double)), this, SIGNAL(energyFeedbackChanged(double)) );

    connect( tankClosed_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( brakeOff_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( keyStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( paddleExtracted_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalChangePosition_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( upperSlitClosed_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( lowerSlitClosed_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( slitsClosed_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( regionAThetaCWLimit_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( regionAThetaCCWLimit_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( regionBThetaCWLimit_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( regionBThetaCCWLimit_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( regionA_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( regionB_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( regionATranslateLimit_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( regionBTranslateLimit_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( translatePermitted_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( thetaPaddlePermitted_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( slitsEnabled_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( energy_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

    onConnectedChanged();
}

BioXASSideMonochromator::~BioXASSideMonochromator()
{

}

AMAction3* BioXASSideMonochromator::createCloseSlitsAction()
{
    if (!slitsCloseCmd_->isConnected())
        return 0;

    AMControlInfo setpoint = slitsCloseCmd_->toInfo();
    setpoint.setValue(0);
    AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, slitsCloseCmd_);

    return action;
}

AMAction3* BioXASSideMonochromator::createWaitForBrakeOffAction()
{
    if (!brakeOff_->isConnected())
        return 0;

    AMControlInfo setpoint = brakeOff_->toInfo();
    setpoint.setValue(1);
    AMControlWaitActionInfo* actionInfo = new AMControlWaitActionInfo(setpoint);
    AMAction3* action = new AMControlWaitAction(actionInfo, brakeOff_);

    return action;
}

AMAction3* BioXASSideMonochromator::createWaitForBrakeOnAction()
{
    if (!brakeOff_->isConnected())
        return 0;

    AMControlInfo setpoint = brakeOff_->toInfo();
    setpoint.setValue(0);
    AMControlWaitActionInfo* actionInfo = new AMControlWaitActionInfo(setpoint);
    AMAction3* action = new AMControlWaitAction(actionInfo, brakeOff_);

    return action;
}

AMAction3* BioXASSideMonochromator::createWaitForMoveToCrystalChangePositionAction()
{
    if (!crystalChangePosition_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalChangePosition_->toInfo();
    setpoint.setValue(0);
    AMControlWaitActionInfo* actionInfo = new AMControlWaitActionInfo(setpoint);
    AMAction3* action = new AMControlWaitAction(actionInfo, crystalChangePosition_);

    return action;
}

AMAction3* BioXASSideMonochromator::createWaitForMoveOffCrystalChangePositionAction()
{
    if (!crystalChangePosition_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalChangePosition_->toInfo();
    setpoint.setValue(1);
    AMControlWaitActionInfo* actionInfo = new AMControlWaitActionInfo(setpoint);
    AMAction3* action = new AMControlWaitAction(actionInfo, crystalChangePosition_);

    return action;
}

AMAction3* BioXASSideMonochromator::createWaitForKeyStatusCWAction()
{
    if (!keyStatus_->isConnected())
        return 0;

    AMControlInfo setpoint = keyStatus_->toInfo();
    setpoint.setValue(1);
    AMControlWaitActionInfo* actionInfo = new AMControlWaitActionInfo(setpoint);
    AMAction3* action = new AMControlWaitAction(actionInfo, keyStatus_);

    return action;
}

AMAction3* BioXASSideMonochromator::createWaitForKeyStatusCCWAction()
{
    if (!keyStatus_->isConnected())
        return 0;

    AMControlInfo setpoint = keyStatus_->toInfo();
    setpoint.setValue(0);
    AMControlWaitActionInfo* actionInfo = new AMControlWaitActionInfo(setpoint);
    AMAction3* action = new AMControlWaitAction(actionInfo, keyStatus_);

    return action;
}

AMAction3* BioXASSideMonochromator::createWaitForCrystalChangeAction()
{
    AMAction3* brakeOn = createWaitForBrakeOnAction();
    if (!brakeOn)
        return 0;

    AMAction3* toCrystalChangePosition = createWaitForMoveToCrystalChangePositionAction();
    if (!toCrystalChangePosition)
        return 0;

    AMAction3* keyTurn = createWaitForKeyStatusCWAction();
    if (!keyTurn)
        return 0;

    AMAction3* brakeOff = createWaitForBrakeOffAction();
    if (!brakeOff)
        return 0;

    AMListAction3* crystalChangeAction = new AMListAction3(new AMListActionInfo3("Crystal Change Action", "Crystal Change Action"));
    crystalChangeAction->addSubAction(brakeOn);
    crystalChangeAction->addSubAction(toCrystalChangePosition);
    crystalChangeAction->addSubAction(keyTurn);
    crystalChangeAction->addSubAction(brakeOff);

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
    bool currentState = (tankClosed_->isConnected() && brakeOff_->isConnected() &&
                         keyStatus_->isConnected() && paddleExtracted_->isConnected() &&
                         crystalChangePosition_->isConnected() && upperSlitClosed_->isConnected() &&
                         lowerSlitClosed_->isConnected() && slitsClosed_->isConnected() &&
                         regionAThetaCWLimit_->isConnected() && regionAThetaCCWLimit_->isConnected() &&
                         regionBThetaCWLimit_->isConnected() && regionBThetaCCWLimit_->isConnected() &&
                         regionA_->isConnected() && regionB_->isConnected() &&
                         regionATranslateLimit_->isConnected() && regionBTranslateLimit_->isConnected() &&
                         translatePermitted_->isConnected() && thetaPaddlePermitted_->isConnected() &&
                         slitsCloseCmd_->isConnected() && slitsEnabled_->isConnected() &&
                         energy_->isConnected());

    if (connected_ != currentState) {
        connected_ = currentState;
        emit connected(connected_);
    }
}
