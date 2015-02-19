#include "BioXASMainMonochromator.h"
#include "BioXASMainMonochromatorCrystalChangeControl.h"
#include <QDebug>

BioXASMainMonochromator::BioXASMainMonochromator(QObject *parent) :
    QObject(parent)
{
    // Initialize variables.

    connected_ = false;
    region_ = None;
	braggMotorCrystalChangePosition_ = 135.0;
	braggMotorRegionADestination_ = SETPOINT_BRAGG_MOTOR_REGION_A_DESTINATION + (braggMotorCrystalChangePosition_ - SETPOINT_BRAGG_MOTOR_CRYSTAL_CHANGE_POSITION);
	braggMotorRegionBDestination_ = SETPOINT_BRAGG_MOTOR_REGION_B_DESTINATION + (braggMotorCrystalChangePosition_ - SETPOINT_BRAGG_MOTOR_CRYSTAL_CHANGE_POSITION);

	// Components.

    upperSlitBladeMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), QString("SMTR1607-5-I21-09"), QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), true, 0.1, 2.0, this);
    lowerSlitBladeMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-10 VERT LOWER BLADE"), QString("SMTR1607-5-I21-10"), QString("SMTR1607-5-I21-10 VERT LOWER BLADE"), true, 0.1, 2.0, this);
    braggMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-12 BRAGG"), QString("SMTR1607-5-I21-12"), QString("SMTR1607-5-I21-12 BRAGG"), true, 0.05, 2.0, this, QString(":deg"));
    verticalMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-13 VERTICAL"), QString("SMTR1607-5-I21-13"), QString("SMTR1607-5-I21-13 VERTICAL"), true, 0.05, 2.0, this);
    lateralMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-14 LATERAL"), QString("SMTR1607-5-I21-14"), QString("SMTR1607-5-I21-14 LATERAL"), true, 0.05, 2.0, this);
    crystalExchangeMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-22 XTAL XCHAGE"), QString("SMTR1607-5-I21-22"), QString("SMTR1607-5-I21-22 XTAL XCHAGE"), true, 0.05, 2.0, this);
    crystal1PitchMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-23 XTAL 1 PITCH"), QString("SMTR1607-5-I21-23"), QString("SMTR1607-5-I21-23 XTAL 1 PITCH"), true, 0.05, 2.0, this, QString(":V"));
    crystal1RollMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-24 XTAL 1 ROLL"), QString("SMTR1607-5-I21-24"), QString("SMTR1607-5-I21-24 XTAL 1 ROLL"),   true, 0.05, 2.0, this, QString(":V"));
    crystal2PitchMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-25 XTAL 2 PITCH"), QString("SMTR1607-5-I21-25"), QString("SMTR1607-5-I21-25 XTAL 2 PITCH"), true, 0.05, 2.0, this, QString(":V"));
    crystal2RollMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-26 XTAL 2 ROLL"), QString("SMTR1607-5-I21-26"), QString("SMTR1607-5-I21-26 XTAL 2 ROLL"), true, 0.05, 2.0, this, QString(":V"));

    slitsClosed_ = new AMReadOnlyPVControl(QString("SlitsClosed"), QString("BL1607-5-I21:Mono:SlitsClosed"), this);
    phosphorPaddle_ = new AMPVControl(QString("PhosphorPaddle"), QString("SMTR1607-5-I21-11:mm:sp"), QString("SMTR1607-5-I21-11:mm:rel"), QString(), this);
    phosphorPaddleOut_ = new AMReadOnlyPVControl("PhosphorPaddleOut", "BL1607-5-I21:Mono:PaddleExtracted", this);
    braggMotorPower_ = new AMPVControl("BraggMotorPower", "SMTR1607-5-I21-12:power", "SMTR1607-5-I21-12:power", QString(), this);
    crystalChangeEnabled_ = new AMReadOnlyPVControl("CrystalChangeEnabled", "BL1607-5-I21:Mono:KeyStatus", this);
    atCrystalChangePosition_ = new AMReadOnlyPVControl("AtCrystalChangePosition", "BL1607-5-I21:Mono:XtalChangePos", this);
    crystalChangeBrakeEnabled_ = new AMReadOnlyPVControl("BrakeEnabled", "BL1607-5-I21:Mono:BrakeOff", this);
    stageMotorAbs_ = new AMPVControl("StageMotorAbsolute", "SMTR1607-5-I21-12:deg:fbk", "SMTR1607-5-I21-12:deg", QString(), this);
    crystalChangeMotorRel_ = new AMPVControl("CrystalChangeMotorRelative", "SMTR1607-5-I21-22:step:sp", "SMTR1607-5-I21-22:step:rel", QString(), this);
    crystalChangeMotorCWLimit_ = new AMReadOnlyPVControl("CrystalChangeMotorCWLimit", "SMTR1607-5-I21-22:cw", this);
    crystalChangeMotorCCWLimit_ = new AMReadOnlyPVControl("CrystalChangeMotorCCWLimit", "SMTR1607-5-I21-22:ccw", this);
    braggMotorCWLimit_ = new AMReadOnlyPVControl("BraggMotorCWLimit", "SMTR1607-5-I21-12:cw", this);
    braggMotorCCWLimit_ = new AMReadOnlyPVControl("BraggMotorCCWLimit", "SMTR1607-5-I21-12:ccw", this);
    regionAStatus_ = new AMReadOnlyPVControl("RegionAStatus", "BL1607-5-I21:Mono:Region:A", this);
    regionBStatus_ = new AMReadOnlyPVControl("RegionBStatus", "BL1607-5-I21:Mono:Region:B", this);

    energy_ = new BioXASMainMonochromatorControl("EnergyEV", "BL1607-5-I21:Energy:EV:fbk", "BL1607-5-I21:Energy:EV", "BL1607-5-I21:Energy:status", QString("BL1607-5-I21:Energy:stop"), this);

    crystalChangeControl_ = new BioXASMainMonochromatorCrystalChangeControl(this, this);

	// Listen to component connection states.

    connect( upperSlitBladeMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( lowerSlitBladeMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( braggMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( verticalMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( lateralMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalExchangeMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystal1PitchMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystal1RollMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystal2PitchMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystal2RollMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

    connect( slitsClosed_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( phosphorPaddle_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( phosphorPaddleOut_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( braggMotorPower_, SIGNAL(valueChanged(double)), this, SLOT(onBraggMotorPowerChanged(double)) );
    connect( crystalChangeEnabled_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( atCrystalChangePosition_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalChangeBrakeEnabled_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( stageMotorAbs_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalChangeMotorRel_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalChangeMotorCWLimit_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalChangeMotorCCWLimit_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( regionAStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( regionBStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
    connect( crystalChangeMotorRel_, SIGNAL(valueChanged(double)), this, SIGNAL(crystalChangeMotorPositionChanged(double)) );

    connect( energy_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

	// Listen to component value changes.

    connect( braggMotor_, SIGNAL(valueChanged(double)), this, SIGNAL(braggMotorPositionChanged(double)) );

    connect( slitsClosed_, SIGNAL(valueChanged(double)), this, SLOT(onSlitsClosedStatusChanged(double)) );
    connect( crystalChangeEnabled_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangeEnabled(double)) );
    connect( atCrystalChangePosition_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangePositionStatusChanged(double)) );
    connect( crystalChangeBrakeEnabled_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangeBrakeEnabledChanged(double)) );
    connect( crystalChangeMotorCCWLimit_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangeMotorCCWLimitStatusChanged(double)) );
    connect( crystalChangeMotorCWLimit_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangeMotorCWLimitStatusChanged(double)) );
    connect( regionAStatus_, SIGNAL(valueChanged(double)), this, SLOT(onRegionChanged()) );
    connect( regionBStatus_, SIGNAL(valueChanged(double)), this, SLOT(onRegionChanged()) );

    // Current settings.

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

    AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_SLITS_CLOSED, AMControlWaitActionInfo::MatchEqual), slitsClosed_);
    action->info()->setShortDescription("Waiting for slits closed...");
    action->info()->setLongDescription("");

    return action;
}

AMAction3* BioXASMainMonochromator::createRemovePaddleAction()
{    
    if (!phosphorPaddle_->isConnected())
        return 0;

    AMControlInfo setpoint = phosphorPaddle_->toInfo();
    setpoint.setValue(SETPOINT_PADDLE_OUT);

    AMControlMoveAction3 *action = new AMControlMoveAction3(new AMControlMoveActionInfo3(setpoint), phosphorPaddle_);
    action->info()->setShortDescription("Removing paddle...");
    action->info()->setLongDescription("");

    return action;
}

AMAction3* BioXASMainMonochromator::createWaitForPaddleRemovedAction()
{
    if (!phosphorPaddleOut_->isConnected())
        return 0;

    AMControlInfo setpoint = phosphorPaddleOut_->toInfo();
    setpoint.setValue(1);

    AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_PADDLE_OUT, AMControlWaitActionInfo::MatchEqual), phosphorPaddleOut_);
    action->info()->setShortDescription("Waiting for paddle removed...");
    action->info()->setLongDescription("");

    return action;
}

AMAction3* BioXASMainMonochromator::createWaitForCrystalChangeEnabledAction()
{
    if (!crystalChangeEnabled_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalChangeEnabled_->toInfo();
    setpoint.setValue(1);

    AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_KEY_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), crystalChangeEnabled_);
    action->info()->setShortDescription("Enable crystal change.");
    action->info()->setLongDescription("Turn the crystal change status key to 'Enabled', counter-clockwise.");

    return action;
}

AMAction3* BioXASMainMonochromator::createMoveToCrystalChangePositionAction()
{
    AMAction3 *action = createMoveBraggMotorAction(SETPOINT_BRAGG_MOTOR_CRYSTAL_CHANGE_POSITION);

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

    AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_POSITION_REACHED, AMControlWaitActionInfo::MatchEqual), atCrystalChangePosition_);
    action->info()->setShortDescription("Waiting for move to crystal change position...");
    action->info()->setLongDescription("");

    return action;
}

AMAction3* BioXASMainMonochromator::createWaitForBrakeDisabledAction()
{
    if (!crystalChangeBrakeEnabled_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalChangeBrakeEnabled_->toInfo();
    setpoint.setValue(1);

    AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAKE_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), crystalChangeBrakeEnabled_);
    action->info()->setShortDescription("Disable brake.");
    action->info()->setLongDescription("Flip the brake switch to 'Disabled'. There is a mandatory 30s wait after the switch is flipped.");

    return action;
}

AMAction3* BioXASMainMonochromator::createMoveCrystalChangeMotorAction(int relDestination)
{
    if (!crystalChangeMotorRel_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalChangeMotorRel_->toInfo();
    setpoint.setValue(relDestination);

    AMControlMoveAction3 *action = new AMControlMoveAction3(new AMControlMoveActionInfo3(setpoint), crystalChangeMotorRel_);
    action->info()->setShortDescription("Moving crystal change motor by " + QString::number(relDestination) + " degrees...");
    action->info()->setLongDescription("");

    return action;
}

AMAction3* BioXASMainMonochromator::createWaitForCrystalChangeMotorLimitReached(bool cwLimit)
{
    AMControlWaitAction *action = 0;

    if (cwLimit && crystalChangeMotorCWLimit_->isConnected()) {
        AMControlInfo setpoint = crystalChangeMotorCWLimit_->toInfo();
        setpoint.setValue(1);

        action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), crystalChangeMotorCWLimit_);
        action->info()->setShortDescription("Waiting for move to crystal change motor CW limit...");
        action->info()->setLongDescription("");

    } else if (!cwLimit && crystalChangeMotorCCWLimit_->isConnected()){
        AMControlInfo setpoint = crystalChangeMotorCCWLimit_->toInfo();
        setpoint.setValue(1);

        action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), crystalChangeMotorCCWLimit_);
        action->info()->setShortDescription("Waiting for move to crystal change motor CCW limit...");
        action->info()->setLongDescription("");
    }

    return action;
}

AMAction3* BioXASMainMonochromator::createWaitForBrakeEnabledAction()
{
    if (!crystalChangeBrakeEnabled_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalChangeBrakeEnabled_->toInfo();
    setpoint.setValue(0);

    AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAKE_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), crystalChangeBrakeEnabled_);
    action->info()->setShortDescription("Enable brake.");
    action->info()->setLongDescription("Flip the brake switch to 'Enabled'. There is a mandatory 30s wait after the switch is flipped.");

    return action;
}

AMAction3* BioXASMainMonochromator::createWaitForBraggMotorLimitReachedAction(bool cwLimit)
{
    AMControlWaitAction *action = 0;

    if (cwLimit && braggMotorCWLimit_->isConnected()) {
        AMControlInfo setpoint = braggMotorCWLimit_->toInfo();
        setpoint.setValue(0);

        action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAGG_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), braggMotorCWLimit_);
        action->info()->setShortDescription("Waiting for move to bragg motor CW limit...");
        action->info()->setLongDescription("");

    } else if (!cwLimit && braggMotorCCWLimit_->isConnected()) {
        AMControlInfo setpoint = braggMotorCCWLimit_->toInfo();
        setpoint.setValue(0);

        action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAGG_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), braggMotorCCWLimit_);
        action->info()->setShortDescription("Waiting for move to bragg motor CCW limit...");
        action->info()->setLongDescription("");
    }

    return action;
}

AMAction3* BioXASMainMonochromator::createMoveBraggMotorAction(double degDestination)
{
    if (!stageMotorAbs_->isConnected())
        return 0;

    AMControlInfo setpoint = stageMotorAbs_->toInfo();
    setpoint.setValue(degDestination);

    AMControlMoveAction3 *action = new AMControlMoveAction3(new AMControlMoveActionInfo3(setpoint), stageMotorAbs_);
    action->info()->setShortDescription("Moving stage to " + QString::number(degDestination) + " degrees...");
    action->info()->setLongDescription("");

    return action;
}

AMAction3* BioXASMainMonochromator::createWaitForMoveToNewRegion(BioXASMainMonochromator::Region destinationRegion)
{
    AMAction3 *action = 0;

    if (regionAStatus_->isConnected() && regionBStatus_->isConnected() && region_ != destinationRegion) {

        if (destinationRegion == A) {
            AMControlInfo setpoint = regionAStatus_->toInfo();
            setpoint.setValue(1);

            action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_REGION_STATE_CHANGED, AMControlWaitActionInfo::MatchEqual), regionAStatus_);
            action->info()->setShortDescription("Waiting for move into region A...");
            action->info()->setLongDescription("");

        } else if (destinationRegion == B) {
            AMControlInfo setpoint = regionBStatus_->toInfo();
            setpoint.setValue(1);

            action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_REGION_STATE_CHANGED, AMControlWaitActionInfo::MatchEqual), regionBStatus_);
            action->info()->setShortDescription("Waiting for move into region B...");
            action->info()->setLongDescription("");
        }
    }

    return action;
}

AMAction3* BioXASMainMonochromator::createWaitForCrystalChangeDisabledAction()
{
    if (!crystalChangeEnabled_->isConnected())
        return 0;

    AMControlInfo setpoint = crystalChangeEnabled_->toInfo();
    setpoint.setValue(0);

    AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_KEY_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), crystalChangeEnabled_);
    action->info()->setShortDescription("Disable crystal change.");
    action->info()->setLongDescription("Turn crystal change status key to 'Disabled', clockwise.");

    return action;
}


AMAction3* BioXASMainMonochromator::createCrystalChangeAction()
{
    AMListAction3 *crystalChangeAction = 0;

    double crystalChangeMotorDestination;
    double newRegionDestination;

    bool regionOK = false;

    Region newRegion;

    if (region_ == A) {
        crystalChangeMotorDestination = SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_B_DESTINATION;
        newRegionDestination = SETPOINT_BRAGG_MOTOR_REGION_B_DESTINATION;
        regionOK = true;
        newRegion = B;

    } else {

//    } else if (region_ == B) {
        crystalChangeMotorDestination = SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_A_DESTINATION;
        newRegionDestination = SETPOINT_BRAGG_MOTOR_REGION_A_DESTINATION;
        regionOK = true; // not necessarily true. but here until we implement a smart way of dealing with starting a crystal change in a 'None' region.
        newRegion = A;
    }

    if (connected_ && regionOK) {

        AMAction3 *closeSlits = createCloseSlitsAction();
        AMAction3 *waitForSlitsClosed = createWaitForSlitsClosedAction();
        AMAction3 *removePaddle = createRemovePaddleAction();
        AMAction3 *waitForPaddleRemoved = createWaitForPaddleRemovedAction();
        AMAction3 *waitForKeyTurnedCCW = createWaitForCrystalChangeEnabledAction();
        AMAction3 *toCrystalChangePosition = createMoveToCrystalChangePositionAction();
        AMAction3 *waitForAtCrystalChangePosition = createWaitForAtCrystalChangePositionAction();
        AMAction3 *waitForBrakeDisabled = createWaitForBrakeDisabledAction();
        AMAction3 *fromCrystalChangePosition = createMoveCrystalChangeMotorAction(crystalChangeMotorDestination);
        AMAction3 *waitForCrystalChangeMotorLimitReached = createWaitForCrystalChangeMotorLimitReached(newRegion == A);
        AMAction3 *waitForBrakeEnabled = createWaitForBrakeEnabledAction();

        AMAction3 *waitForBraggMotorLimitReached = createWaitForBraggMotorLimitReachedAction(newRegion == A);

        AMAction3 *toNewRegion = createMoveBraggMotorAction(newRegionDestination);
        AMAction3 *waitForMoveToNewRegion = createWaitForMoveToNewRegion(newRegion);
        AMAction3 *waitForKeyTurnedCW = createWaitForCrystalChangeDisabledAction();


        crystalChangeAction = new AMListAction3(new AMListActionInfo3("Crystal Change Action", "Crystal Change Action"));
        crystalChangeAction->addSubAction(closeSlits);
        crystalChangeAction->addSubAction(waitForSlitsClosed);
        crystalChangeAction->addSubAction(removePaddle);
        crystalChangeAction->addSubAction(waitForPaddleRemoved);
        crystalChangeAction->addSubAction(waitForKeyTurnedCCW);
        crystalChangeAction->addSubAction(toCrystalChangePosition);
        crystalChangeAction->addSubAction(waitForAtCrystalChangePosition);
        crystalChangeAction->addSubAction(waitForBrakeDisabled);
        crystalChangeAction->addSubAction(fromCrystalChangePosition);
        crystalChangeAction->addSubAction(waitForCrystalChangeMotorLimitReached);
        crystalChangeAction->addSubAction(waitForBrakeEnabled);
        crystalChangeAction->addSubAction(waitForBraggMotorLimitReached);
        crystalChangeAction->addSubAction(toNewRegion);
        crystalChangeAction->addSubAction(waitForMoveToNewRegion);
        crystalChangeAction->addSubAction(waitForKeyTurnedCW);
    }

    if (crystalChangeAction) {
        qDebug() << "Mono has produced valid crystal change action.";
    } else {
        qDebug() << "Mono has produced invalid action.";
    }

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
    if (!braggMotorPower_->isConnected())
        return 0;

    AMControlInfo setpoint = braggMotorPower_->toInfo();
    setpoint.setValue(1);
    AMControlMoveActionInfo3 *info = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(info, braggMotorPower_);

    return action;
}

AMAction3* BioXASMainMonochromator::createSetBraggMotorPowerAutoAction()
{
    if (!braggMotorPower_->isConnected())
        return 0;

    AMControlInfo setpoint = braggMotorPower_->toInfo();
    setpoint.setValue(3);
    AMControlMoveActionInfo3 *info = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(info, braggMotorPower_);

    return action;
}

void BioXASMainMonochromator::onConnectedChanged()
{
    bool currentState = (

                upperSlitBladeMotor_->isConnected() &&
                lowerSlitBladeMotor_->isConnected() &&
                braggMotor_->isConnected() &&
                verticalMotor_->isConnected() &&
                lateralMotor_->isConnected() &&
                crystalExchangeMotor_->isConnected() &&
                crystal1PitchMotor_->isConnected() &&
                crystal1RollMotor_->isConnected() &&
                crystal2PitchMotor_->isConnected() &&
                crystal2RollMotor_->isConnected() &&
                slitsClosed_->isConnected() &&
                phosphorPaddle_->isConnected() &&
                phosphorPaddleOut_->isConnected() &&
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

