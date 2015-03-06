#include "BioXASSideMonochromator.h"

#include <QDebug>

BioXASSideMonochromator::BioXASSideMonochromator(QObject *parent) :
	BioXASSSRLMonochromator(parent)
{
	connected_ = false;
	region_ = Region::None;

	phosphorPaddleMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), QString("SMTR1607-5-I22-11"), QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), true, 0.05, 2.0, this);
	braggMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-12 BRAGG"), QString("SMTR1607-5-I22-12"), QString("SMTR1607-5-I22-12 BRAGG"), true, 0.05, 2.0, this, QString(":deg"));
	verticalMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-13 VERTICAL"), QString("SMTR1607-5-I22-13"), QString("SMTR1607-5-I22-13 VERTICAL"), true, 0.05, 2.0, this);
	lateralMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-14 LATERAL"), QString("SMTR1607-5-I22-14"), QString("SMTR1607-5-I22-14 LATERAL"), true, 0.05, 2.0, this);
	crystalChangeMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-22 XTAL XCHAGE"), QString("SMTR1607-5-I22-22"), QString("SMTR1607-5-I22-22 XTAL XCHAGE"), true, 0.05, 2.0, this);
	crystal1PitchMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), QString("SMTR1607-5-I22-23"), QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), true, 0.05, 2.0, this, QString(":V"));
	crystal1RollMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-24 XTAL 1 ROLL"), QString("SMTR1607-5-I22-24"), QString("SMTR1607-5-I22-24 XTAL 1 ROLL"),   true, 0.05, 2.0, this, QString(":V"));
	crystal2PitchMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), QString("SMTR1607-5-I22-25"), QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), true, 0.05, 2.0, this, QString(":V"));
	crystal2RollMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), QString("SMTR1607-5-I22-26"), QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), true, 0.05, 2.0, this, QString(":V"));

	braggMotorPower_ = new AMPVControl("BraggMotorPower", "SMTR1607-5-I22-12:power", "SMTR1607-5-I22-12:power", QString(), this);
	slitsStatus_ = new AMPVControl("SlitsStatus", "BL1607-5-I22:Mono:SlitsClosed", "BL1607-5-I22:Mono:SlitsOprCloseCmd", QString(), this);
	paddleStatus_ = new AMPVControl("PaddleStatus", "BL1607-5-I22:Mono:PaddleExtracted", "BL1607-5-I22:Mono:PaddleOprOutCmd", QString(), this);
	keyStatus_ = new AMReadOnlyPVControl("KeyStatus", "BL1607-5-I22:Mono:KeyStatus", this);
	braggAtCrystalChangePositionStatus_ = new AMReadOnlyPVControl("AtCrystalChangePosition", "BL1607-5-I22:Mono:XtalChangePos", this);
	brakeStatus_ = new AMReadOnlyPVControl("BrakeStatus", "BL1607-5-I22:Mono:BrakeOff", this);
	braggAbsolutePosition_ = new AMPVControl("BraggMotorAbsolutePosition", "SMTR1607-5-I22-12:deg:fbk", "SMTR1607-5-I22-12:deg", QString(), this);
	crystalChangeMotorRel_ = new AMPVControl("CrystalChangeMotorRelativePosition", "SMTR1607-5-I22-22:step:rel", "SMTR1607-5-I22-22:step:sp", QString(), this);
	crystalChangeMotorCWLimitStatus_ = new AMReadOnlyPVControl("CrystalChangeMotorCWLimitStatus", "SMTR1607-5-I22-22:cw", this);
	crystalChangeMotorCCWLimitStatus_ = new AMReadOnlyPVControl("CrystalChangeMotorCCWLimitStatus", "SMTR1607-5-I22-22:ccw", this);
	regionAStatus_ = new AMReadOnlyPVControl("RegionAStatus", "BL1607-5-I22:Mono:Region:A", this);
	regionBStatus_ = new AMReadOnlyPVControl("RegionBStatus", "BL1607-5-I22:Mono:Region:B", this);

	energy_ = new BioXASSideMonochromatorControl("EnergyEV", "BL1607-5-I22:Energy:EV:fbk", "BL1607-5-I22:Energy:EV", "BL1607-5-I22:Energy:status", QString("BL1607-5-I22:Energy:stop"), this);

	// Controls value changes.
	connect( braggMotorPower_, SIGNAL(valueChanged(double)), this, SLOT(onBraggMotorPowerChanged(double)) );
	connect( slitsStatus_, SIGNAL(valueChanged(double)), this, SLOT(onSlitsClosedChanged(double)) );
	connect( paddleStatus_, SIGNAL(valueChanged(double)), this, SLOT(onPaddleOutChanged(double)) );
	connect( keyStatus_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangeEnabled(double)) );
	connect( braggAtCrystalChangePositionStatus_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangePositionStatusChanged(double)) );
	connect( brakeStatus_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangeBrakeEnabledChanged(double)) );
	connect( crystalChangeMotorCCWLimitStatus_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangeMotorCCWLimitStatusChanged(double)) );
	connect( crystalChangeMotorCWLimitStatus_, SIGNAL(valueChanged(double)), this, SLOT(onCrystalChangeMotorCWLimitStatusChanged(double)) );
	connect( regionAStatus_, SIGNAL(valueChanged(double)), this, SLOT(onRegionChanged()) );
	connect( regionBStatus_, SIGNAL(valueChanged(double)), this, SLOT(onRegionChanged()) );

	// Motors connection state.
	connect( phosphorPaddleMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( braggMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( verticalMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( lateralMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( crystalChangeMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( crystal1PitchMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( crystal1RollMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( crystal2PitchMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( crystal2RollMotor_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

	// Controls connection state.
	connect( slitsStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( paddleStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( keyStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( braggAtCrystalChangePositionStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( brakeStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( braggAbsolutePosition_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( crystalChangeMotorRel_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( crystalChangeMotorCWLimitStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( crystalChangeMotorCCWLimitStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( regionAStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( regionBStatus_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	connect( energy_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

	onConnectedChanged();
}

BioXASSideMonochromator::~BioXASSideMonochromator()
{

}

AMAction3* BioXASSideMonochromator::createSetEnergyAction(double newEnergy)
{
	if (!energy_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(energy_, newEnergy);
}

AMAction3* BioXASSideMonochromator::createCloseSlitsAction()
{
	if (!slitsStatus_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(slitsStatus_, Slits::Closed);
}

AMAction3* BioXASSideMonochromator::createRemovePaddleAction()
{
	if (!paddleStatus_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(paddleStatus_, Paddle::Out);
}

AMAction3* BioXASSideMonochromator::createMoveBraggMotorAction(double degDestination)
{
	if (!braggAbsolutePosition_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(braggAbsolutePosition_, degDestination);
}

AMAction3* BioXASSideMonochromator::createMoveCrystalChangeMotorAction(int relDestination)
{
	if (!crystalChangeMotorRel_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(crystalChangeMotorRel_, relDestination, true);
}

AMAction3* BioXASSideMonochromator::createSetBraggMotorPowerOnAction()
{
	return AMActionSupport::buildControlMoveAction(braggMotorPower_, CLSMAXvMotor::PowerOn);
}

AMAction3* BioXASSideMonochromator::createSetBraggMotorPowerAutoAction()
{
	return AMActionSupport::buildControlMoveAction(braggMotorPower_, CLSMAXvMotor::PowerAutoSoftware);
}

void BioXASSideMonochromator::setEnergy(double newEnergy)
{
	if (energy_->isConnected())
		energy_->move(newEnergy);
}

void BioXASSideMonochromator::setRegion(Region::State newRegion)
{
	AMAction3 *crystalChangeAction = createCrystalChangeAction(newRegion);

	if (crystalChangeAction && connected_)
		crystalChangeAction->start();
}

void BioXASSideMonochromator::setSlitsClosed()
{
	if (slitsStatus_->isConnected())
		slitsStatus_->move(Slits::Closed);
}

void BioXASSideMonochromator::setPaddleOut()
{
	if (paddleStatus_->isConnected())
		paddleStatus_->move(Paddle::Out);
}

void BioXASSideMonochromator::setBraggMotorPosition(double degDestination)
{
	if (braggMotor_->isConnected())
		braggMotor_->move(degDestination);
}

void BioXASSideMonochromator::setCrystalChangeMotorPosition(double relDestination)
{
	if (crystalChangeMotorRel_->isConnected())
		crystalChangeMotorRel_->move(relDestination);
}

AMAction3* BioXASSideMonochromator::createWaitForKeyEnabledAction()
{
	if (!keyStatus_->isConnected())
		return 0;

	return AMActionSupport::buildControlWaitAction(keyStatus_, Key::Enabled);
}

AMAction3* BioXASSideMonochromator::createMoveToCrystalChangePositionAction()
{
	return createMoveBraggMotorAction(BRAGG_MOTOR_CRYSTAL_CHANGE_POSITION);
}

AMAction3* BioXASSideMonochromator::createWaitForBrakeDisabledAction()
{
	if (!brakeStatus_->isConnected())
		return 0;

	return AMActionSupport::buildControlWaitAction(brakeStatus_, Brake::Disabled);
}

AMAction3* BioXASSideMonochromator::createWaitForBrakeEnabledAction()
{
	if (!brakeStatus_->isConnected())
		return 0;

	return AMActionSupport::buildControlWaitAction(brakeStatus_, Brake::Enabled);
}

AMAction3* BioXASSideMonochromator::createWaitForKeyDisabledAction()
{
	if (!keyStatus_->isConnected())
		return 0;

	return AMActionSupport::buildControlWaitAction(keyStatus_, Key::Disabled);
}


AMAction3* BioXASSideMonochromator::createCrystalChangeAction(Region::State newRegion)
{
	if (!connected_ || region_ == newRegion || newRegion == Region::None)
		return 0;

	double crystalChangeMotorDestination;
	double newRegionDestination;

	if (newRegion == Region::A) {
		crystalChangeMotorDestination = -12,000;
		newRegionDestination = 350;

	} else {
		crystalChangeMotorDestination = 12,000;
		newRegionDestination = 140;
	}

	AMAction3 *closeSlits = createCloseSlitsAction();
	AMAction3 *removePaddle = createRemovePaddleAction();
	AMAction3 *turnKeyCCW = createWaitForKeyEnabledAction();
	AMAction3 *toCrystalChangePosition = createMoveToCrystalChangePositionAction();
	AMAction3 *disableBrake = createWaitForBrakeDisabledAction();
	AMAction3 *fromCrystalChangePosition = createMoveCrystalChangeMotorAction(crystalChangeMotorDestination);
	AMAction3 *enableBrake = createWaitForBrakeEnabledAction();
	AMAction3 *toNewRegion = createMoveBraggMotorAction(newRegionDestination);
	AMAction3 *turnKeyCW = createWaitForKeyDisabledAction();


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

void BioXASSideMonochromator::onConnectedChanged()
{
	bool currentState = (
				// Motors
				phosphorPaddleMotor_->isConnected() &&
				braggMotor_->isConnected() &&
				verticalMotor_->isConnected() &&
				lateralMotor_->isConnected() &&
				crystalChangeMotor_->isConnected() &&
				crystal1PitchMotor_->isConnected() &&
				crystal1RollMotor_->isConnected() &&
				crystal2PitchMotor_->isConnected() &&
				crystal2RollMotor_->isConnected() &&

				// Controls
				//braggMotorPower_->isConnected() && slitsClosed_->isConnected() && paddleOut_->isConnected() &&
				braggMotorPower_->isConnected() &&
				keyStatus_->isConnected() &&
				braggAtCrystalChangePositionStatus_->isConnected() &&
				brakeStatus_->isConnected() &&
				braggAbsolutePosition_->isConnected() &&
				crystalChangeMotorRel_->isConnected() &&
				crystalChangeMotorCWLimitStatus_->isConnected() &&
				crystalChangeMotorCCWLimitStatus_->isConnected() &&
				regionAStatus_->isConnected() &&
				regionBStatus_->isConnected() &&
				energy_->isConnected()
				);

	if (connected_ != currentState) {
		connected_ = currentState;
		emit connected(connected_);
	}
}

void BioXASSideMonochromator::onRegionChanged()
{
	Region::State newRegion;
	int regionAVal = (int)regionAStatus_->value();
	int regionBVal = (int)regionBStatus_->value();

	if (regionAVal == 0 && regionBVal == 0)
		newRegion = Region::None;
	else if (regionAVal == 0 && regionBVal == 1)
		newRegion = Region::B;
	else if (regionAVal == 1 && regionBVal == 0)
		newRegion = Region::A;
	else if (regionAVal == 1 && regionBVal == 1)
		newRegion = Region::None;
	else
		newRegion = Region::None;

	if (region_ != newRegion) {
		region_ = newRegion;
		emit regionChanged(region_);
	}

}
