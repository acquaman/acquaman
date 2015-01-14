#include "BioXASMainMonochromator.h"
#include <QDebug>

BioXASMainMonochromator::BioXASMainMonochromator(QObject *parent) :
    QObject(parent)
{
    connected_ = false;
    region_ = None;

    upperSlitBladeMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), QString("SMTR1607-5-I21-09"), QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), true, 0.1, 2.0, this);
    lowerSlitBladeMotor_ = new CLSMAXvMotor(QString("SMTR1607-I21-10 VERT LOWER BLADE"), QString("SMTR1607-5-I21-10"), QString("SMTR1607-5-I21-10 VERT LOWER BLADE"), true, 0.1, 2.0, this);
    phosphorPaddleMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-11 PHOSPHOR PADDLE"), QString("SMTR1607-5-I21-11"), QString("SMTR1607-5-I21-11 PHOSPHOR PADDLE"), false, 0.1, 2.0, this);
    braggMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-12 BRAGG"), QString("SMTR1607-5-I21-12"), QString("SMTR1607-5-I21-12 BRAGG"), true, 0.05, 2.0, this, QString(":deg"));
    verticalMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-13 VERTICAL"), QString("SMTR1607-5-I21-13"), QString("SMTR1607-5-I21-13 VERTICAL"), true, 0.05, 2.0, this);
    lateralMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-14 LATERAL"), QString("SMTR1607-5-I21-14"), QString("SMTR1607-5-I21-14 LATERAL"), true, 0.05, 2.0, this);
    crystalExchangeMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-22 XTAL XCHAGE"), QString("SMTR1607-5-I21-22"), QString("SMTR1607-5-I21-22 XTAL XCHAGE"), true, 0.05, 2.0, this);
    crystal1PitchMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-23 XTAL 1 PITCH"), QString("SMTR1607-5-I21-23"), QString("SMTR1607-5-I21-23 XTAL 1 PITCH"), true, 0.05, 2.0, this, QString(":V"));
    crystal1RollMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-24 XTAL 1 ROLL"), QString("SMTR1607-5-I21-24"), QString("SMTR1607-5-I21-24 XTAL 1 ROLL"),   true, 0.05, 2.0, this, QString(":V"));
    crystal2PitchMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-25 XTAL 2 PITCH"), QString("SMTR1607-5-I21-25"), QString("SMTR1607-5-I21-25 XTAL 2 PITCH"), true, 0.05, 2.0, this, QString(":V"));
    crystal2RollMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-26 XTAL 2 ROLL"), QString("SMTR1607-5-I21-26"), QString("SMTR1607-5-I21-26 XTAL 2 ROLL"), true, 0.05, 2.0, this, QString(":V"));

    slitsClosed_ = new AMReadOnlyPVControl(QString("SlitsClosed"), QString("BL1607-5-I21:Mono:SlitsClosed"), this);
    phosphorPaddleOut_ = new AMReadOnlyPVControl("PhosphorPaddleOut", "BL1607-5-I21:Mono:PaddleExtracted", this);
    braggMotorPower_ = new AMPVControl("BraggMotorPower", "SMTR1607-5-I21-12:power", "SMTR1607-5-I21-12:power", QString(), this);
    crystalChangeEnabled_ = new AMReadOnlyPVControl("CrystalChangeEnabled", "BL1607-5-I21:Mono:KeyStatus", this);
    atCrystalChangePosition_ = new AMReadOnlyPVControl("AtCrystalChangePosition", "BL1607-5-I21:Mono:XtalChangePos", this);
    crystalChangeBrakeEnabled_ = new AMReadOnlyPVControl("BrakeEnabled", "BL1607-5-I21:Mono:BrakeOff", this);
    stageMotorAbs_ = new AMPVControl("StageMotorAbsolute", "SMTR1607-5-I21-12:deg:fbk", "SMTR1607-5-I21-12:deg", QString(), this);
    crystalChangeMotorRel_ = new AMPVControl("CrystalChangeMotorRelative", "SMTR1607-5-I21-22:step:rel", "SMTR1607-5-I21-22:step:sp", QString(), this);
    crystalChangeMotorCWLimit_ = new AMReadOnlyPVControl("CrystalChangeMotorCWLimit", "SMTR1607-5-I21-22:cw", this);
    crystalChangeMotorCCWLimit_ = new AMReadOnlyPVControl("CrystalChangeMotorCCWLimit", "SMTR1607-5-I21-22:ccw", this);
    regionAStatus_ = new AMReadOnlyPVControl("RegionAStatus", "BL1607-5-I21:Mono:Region:A", this);
    regionBStatus_ = new AMReadOnlyPVControl("RegionBStatus", "BL1607-5-I21:Mono:Region:B", this);

    energy_ = new BioXASMainMonochromatorControl("EnergyEV", "BL1607-5-I21:Energy:EV:fbk", "BL1607-5-I21:Energy:EV", "BL1607-5-I21:Energy:status", QString("BL1607-5-I21:Energy:stop"), this);

    // Listen to controls value changes.
    connect( braggMotorPower_, SIGNAL(valueChanged(double)), this, SLOT(onBraggMotorPowerChanged(double)) );
    connect( crystalChangeEnabled_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangeEnabled(double)) );
    connect( atCrystalChangePosition_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangePositionStatusChanged(double)) );
    connect( crystalChangeBrakeEnabled_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangeBrakeEnabledChanged(double)) );
    connect( crystalChangeMotorCCWLimit_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangeMotorCCWLimitStatusChanged(double)) );
    connect( crystalChangeMotorCWLimit_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangeMotorCWLimitStatusChanged(double)) );
    connect( regionAStatus_, SIGNAL(valueChanged(double)), this, SLOT(onRegionChanged()) );
    connect( regionBStatus_, SIGNAL(valueChanged(double)), this, SLOT(onRegionChanged()) );

    // Listen to motors connection state.
    connect( upperSlitBladeMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( lowerSlitBladeMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( phosphorPaddleMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( braggMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( verticalMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( lateralMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalExchangeMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystal1PitchMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystal1RollMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystal2PitchMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystal2RollMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

    // Listen to controls connection state.
    connect( crystalChangeEnabled_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( atCrystalChangePosition_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
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

BioXASMainMonochromator::~BioXASMainMonochromator()
{

}

AMAction3* BioXASMainMonochromator::createCloseLowerSlitAction()
{
    if (!lowerSlitBladeMotor_->isConnected())
        return 0;

    AMControlInfo setpoint = lowerSlitBladeMotor_->toInfo();
    setpoint.setValue(0);

    AMControlMoveAction3 *action = new AMControlMoveAction3(new AMControlMoveActionInfo3(setpoint), lowerSlitBladeMotor_);

    return action;
}

AMAction3* BioXASMainMonochromator::createCloseUpperSlitAction()
{
    if (!upperSlitBladeMotor_->isConnected())
        return 0;

    AMControlInfo setpoint = upperSlitBladeMotor_->toInfo();
    setpoint.setValue(0);

    AMControlMoveAction3 *action = new AMControlMoveAction3(new AMControlMoveActionInfo3(setpoint), upperSlitBladeMotor_);

    return action;
}

AMAction3* BioXASMainMonochromator::createCloseSlitsAction()
{
    AMAction3 *closeLower = createCloseLowerSlitAction();
    AMAction3 *closeUpper = createCloseUpperSlitAction();

    AMListAction3 *closeSlits = new AMListAction3(new AMListActionInfo3("Closing mono slits", "Closing mono slits"), AMListAction3::Parallel);
    if(lowerSlitBladeMotor_->atLimit() != CLSMAXvMotor::LimitCW)
        closeSlits->addSubAction(closeLower);
    if(upperSlitBladeMotor_->atLimit() != CLSMAXvMotor::LimitCW)
        closeSlits->addSubAction(closeUpper);

    closeSlits->info()->setShortDescription("Closing slits...");
    closeSlits->info()->setLongDescription("");

    return closeSlits;
}

AMAction3* BioXASMainMonochromator::createWaitForSlitsClosedAction()
{
    if (!slitsClosed_->isConnected())
        return 0;

    AMControlInfo setpoint = slitsClosed_->toInfo();
    setpoint.setValue(1);

    AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(setpoint, 20, AMControlWaitActionInfo::MatchEqual);
    actionInfo->setShortDescription("Waiting for slits closed...");
    actionInfo->setLongDescription("");

    AMControlWaitAction *action = new AMControlWaitAction(actionInfo, slitsClosed_);

    return action;
}

AMAction3* BioXASMainMonochromator::createRemovePaddleAction()
{
    AMAction3 *action = 0;

    qDebug() << "Phosphor paddle at CW Limit: " << (phosphorPaddleMotor_->atLimit() == CLSMAXvMotor::LimitCW);
    qDebug() << "Phosphor paddle at CCW Limit: " << (phosphorPaddleMotor_->atLimit() == CLSMAXvMotor::LimitCCW);

    if (phosphorPaddleMotor_->isConnected() && phosphorPaddleMotor_->atLimit() != CLSMAXvMotor::LimitCW) {

        AMControlInfo setpoint = phosphorPaddleMotor_->toInfo();
        setpoint.setValue(0);

        AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
        actionInfo->setShortDescription("Removing paddle...");
        actionInfo->setLongDescription("");

        action = new AMControlMoveAction3(actionInfo, phosphorPaddleMotor_);
    }

    return action;
}

AMAction3* BioXASMainMonochromator::createWaitForPaddleRemovedAction()
{
    if (!phosphorPaddleOut_->isConnected())
        return 0;

    AMControlInfo setpoint = phosphorPaddleOut_->toInfo();
    setpoint.setValue(1);

    AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(setpoint, 30, AMControlWaitActionInfo::MatchEqual);
    actionInfo->setShortDescription("Waiting for paddle removed...");
    actionInfo->setLongDescription("");

    AMControlWaitAction *action = new AMControlWaitAction(actionInfo, phosphorPaddleOut_);

    return action;

    return 0;
}

AMAction3* BioXASMainMonochromator::createWaitForCrystalChangeEnabledAction()
{
    if (!crystalChangeEnabled_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalChangeEnabled_->toInfo();
    setpoint.setValue(1);

    AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(setpoint, 100, AMControlWaitActionInfo::MatchEqual);
    actionInfo->setShortDescription("Enable crystal change.");
    actionInfo->setLongDescription("Turn the crystal change status key to 'Enabled', counter-clockwise.");

    AMControlWaitAction *action = new AMControlWaitAction(actionInfo, crystalChangeEnabled_);

    return action;
}

AMAction3* BioXASMainMonochromator::createMoveToCrystalChangePositionAction()
{
    AMAction3 *action = createMoveStageAction(55);

    if (action) {
        action->info()->setShortDescription("Moving to crystal change position...");
        action->info()->setLongDescription("");
    }

    return action;
}

AMAction3* BioXASMainMonochromator::createWaitForAtCrystalChangePositionAction()
{
    if (!atCrystalChangePosition_->isConnected())
        return 0;

    AMControlInfo setpoint = atCrystalChangePosition_->toInfo();
    setpoint.setValue(1);

    AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(setpoint, 60, AMControlWaitActionInfo::MatchEqual);
    actionInfo->setShortDescription("Waiting for move to crystal change position...");
    actionInfo->setLongDescription("");

    AMControlWaitAction *action = new AMControlWaitAction(actionInfo, atCrystalChangePosition_);

    return action;
}

AMAction3* BioXASMainMonochromator::createWaitForBrakeDisabledAction()
{
    if (!crystalChangeBrakeEnabled_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalChangeBrakeEnabled_->toInfo();
    setpoint.setValue(1);

    AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(setpoint, 10, AMControlWaitActionInfo::MatchEqual);
    actionInfo->setShortDescription("Disable brake.");
    actionInfo->setLongDescription("Flip the brake switch to 'Disabled'.");

    AMControlWaitAction *action = new AMControlWaitAction(actionInfo, crystalChangeBrakeEnabled_);

    return action;
}

AMAction3* BioXASMainMonochromator::createMoveCrystalChangeMotorAction(int relDestination)
{
    if (!crystalChangeMotorRel_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalChangeMotorRel_->toInfo();
    setpoint.setValue(relDestination);

    AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
    actionInfo->setShortDescription("Moving crystal change motor by " + QString::number(relDestination) + " degrees...");
    actionInfo->setLongDescription("");

    AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, crystalChangeMotorRel_);

    return action;
}

AMAction3* BioXASMainMonochromator::createWaitForBrakeEnabledAction()
{
    if (!crystalChangeBrakeEnabled_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalChangeBrakeEnabled_->toInfo();
    setpoint.setValue(0);

    AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(setpoint, 10, AMControlWaitActionInfo::MatchEqual);
    actionInfo->setShortDescription("Enable brake.");
    actionInfo->setLongDescription("Flip the brake switch to 'Enabled'.");\

    AMControlWaitAction *action = new AMControlWaitAction(actionInfo, crystalChangeBrakeEnabled_);

    return action;
}

AMAction3* BioXASMainMonochromator::createMoveStageAction(double degDestination)
{
    if (!stageMotorAbs_->isConnected())
        return 0;

    AMControlInfo setpoint = stageMotorAbs_->toInfo();
    setpoint.setValue(degDestination);

    AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
    actionInfo->setShortDescription("Moving stage to " + QString::number(degDestination) + " degrees...");
    actionInfo->setLongDescription("");

    AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, stageMotorAbs_);

    return action;
}


AMAction3* BioXASMainMonochromator::createWaitForCrystalChangeDisabledAction()
{
    if (!crystalChangeEnabled_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalChangeEnabled_->toInfo();
    setpoint.setValue(0);

    AMControlWaitActionInfo *actionInfo = new AMControlWaitActionInfo(setpoint);
    actionInfo->setShortDescription("Disable crystal change.");
    actionInfo->setLongDescription("Turn crystal change status key to 'Disabled', clockwise.");

    AMControlWaitAction *action = new AMControlWaitAction(actionInfo, crystalChangeEnabled_);

    return action;
}


AMAction3* BioXASMainMonochromator::createCrystalChangeAction()
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
    AMAction3 *waitForSlitsClosed = createWaitForSlitsClosedAction();
    AMAction3 *removePaddle = createRemovePaddleAction();
    AMAction3 *waitForPaddleRemoved = createWaitForPaddleRemovedAction();
    AMAction3 *turnKeyCCW = createWaitForCrystalChangeEnabledAction();
    AMAction3 *toCrystalChangePosition = createMoveToCrystalChangePositionAction();
    AMAction3 *waitForAtCrystalChangePosition = createWaitForAtCrystalChangePositionAction();
    AMAction3 *disableBrake = createWaitForBrakeDisabledAction();
    AMAction3 *fromCrystalChangePosition = createMoveCrystalChangeMotorAction(crystalChangeMotorDestination);
    AMAction3 *enableBrake = createWaitForBrakeEnabledAction();
    AMAction3 *toNewRegion = createMoveStageAction(newRegionDestination);
    AMAction3 *turnKeyCW = createWaitForCrystalChangeDisabledAction();


    AMListAction3* crystalChangeAction = new AMListAction3(new AMListActionInfo3("Crystal Change Action", "Crystal Change Action"));
    crystalChangeAction->addSubAction(closeSlits);
    crystalChangeAction->addSubAction(waitForSlitsClosed);
    crystalChangeAction->addSubAction(removePaddle);
    crystalChangeAction->addSubAction(waitForPaddleRemoved);
    crystalChangeAction->addSubAction(turnKeyCCW);
    crystalChangeAction->addSubAction(toCrystalChangePosition);
    crystalChangeAction->addSubAction(waitForAtCrystalChangePosition);
    crystalChangeAction->addSubAction(disableBrake);
    crystalChangeAction->addSubAction(fromCrystalChangePosition);
    crystalChangeAction->addSubAction(enableBrake);
    crystalChangeAction->addSubAction(toNewRegion);
    crystalChangeAction->addSubAction(turnKeyCW);

    return crystalChangeAction;
}

AMAction3* BioXASMainMonochromator::createSetEnergyAction(double newEnergy)
{
    if (!energy_->isConnected())
        return 0;

    AMControlInfo setpoint = energy_->toInfo();
    setpoint.setValue(newEnergy);
    AMControlMoveActionInfo3* actionInfo = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3* action = new AMControlMoveAction3(actionInfo, energy_);

    return action;
}

AMAction3* BioXASMainMonochromator::createSetBraggMotorPowerOnAction()
{
    AMControlInfo setpoint = braggMotorPower_->toInfo();
    setpoint.setValue(1);
    AMControlMoveActionInfo3 *info = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(info, braggMotorPower_);

    return action;
}

AMAction3* BioXASMainMonochromator::createSetBraggMotorPowerAutoAction()
{
    AMControlInfo setpoint = braggMotorPower_->toInfo();
    setpoint.setValue(3);
    AMControlMoveActionInfo3 *info = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(info, braggMotorPower_);

    return action;
}

void BioXASMainMonochromator::onConnectedChanged()
{
    bool currentState = false;

    currentState = (

                // Motors
                upperSlitBladeMotor_->isConnected() &&
                lowerSlitBladeMotor_->isConnected() &&
                phosphorPaddleMotor_->isConnected() &&
                braggMotor_->isConnected() &&
                verticalMotor_->isConnected() &&
                lateralMotor_->isConnected() &&
                crystalExchangeMotor_->isConnected() &&
                crystal1PitchMotor_->isConnected() &&
                crystal1RollMotor_->isConnected() &&
                crystal2PitchMotor_->isConnected() &&
                crystal2RollMotor_->isConnected() &&

                // Controls
                braggMotorPower_->isConnected() &&
                crystalChangeEnabled_->isConnected() &&
                atCrystalChangePosition_->isConnected() &&
                crystalChangeBrakeEnabled_->isConnected() &&
                stageMotorAbs_->isConnected() &&
                crystalChangeMotorRel_->isConnected() &&
                crystalChangeMotorCWLimit_->isConnected() &&
                crystalChangeMotorCCWLimit_->isConnected() &&
                regionAStatus_->isConnected() &&
                regionBStatus_->isConnected() &&
                energy_->isConnected()

                );

    if (connected_ != currentState) {
        connected_ = currentState;
        emit connected(connected_);
    }
}

void BioXASMainMonochromator::onRegionChanged()
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

