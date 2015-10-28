#include "BioXASSideMonochromator.h"
#include "beamline/BioXAS/BioXASSideMonochromatorMask.h"

BioXASSideMonochromator::BioXASSideMonochromator(QObject *parent) :
	BioXASSSRLMonochromator("SideMono", parent)
{
	// Create components.

	paddle_ = new AMPVwStatusControl("Paddle", "SMTR1607-5-I22-11:mm:fbk", "SMTR1607-5-I22-11:mm", "SMTR1607-5-I22-11:status", "SMTR1607-5-I22-11:stop", this);
	paddleStatus_ = new AMReadOnlyPVControl("PaddleStatus", "BL1607-5-I22:Mono:PaddleExtracted", this);
	keyStatus_ = new AMReadOnlyPVControl("KeyStatus", "BL1607-5-I22:Mono:KeyStatus", this);
	brakeStatus_ = new AMReadOnlyPVControl("BrakeStatus", "BL1607-5-I22:Mono:BrakeOff", this);
	braggAtCrystalChangePositionStatus_ = new AMReadOnlyPVControl("AtCrystalChangePosition", "BL1607-5-I22:Mono:XtalChangePos", this);
	crystalChange_ = new AMPVwStatusControl("CrystalChange", "SMTR1607-5-I22-22:mm:fbk", "SMTR1607-5-I22-22:mm", "SMTR1607-5-I22-22:status", "SMTR1607-5-I22-22:stop", this);
	crystalChangeCWLimitStatus_ = new AMReadOnlyPVControl("CrystalChangeCWStatus", "SMTR1607-5-I22-22:cw", this);
	crystalChangeCCWLimitStatus_ = new AMReadOnlyPVControl("CrystalChangeCCWStatus", "SMTR1607-5-I22-22:ccw", this);
	regionAStatus_ = new AMReadOnlyPVControl("RegionAStatus", "BL1607-5-I22:Mono:Region:A", this);
	regionBStatus_ = new AMReadOnlyPVControl("RegionBStatus", "BL1607-5-I22:Mono:Region:B", this);

	paddleMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), QString("SMTR1607-5-I22-11"), QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), true, 0.05, 2.0, this);
	encoderBraggMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-12 BRAGG"), QString("SMTR1607-5-I22-12"), QString("SMTR1607-5-I22-12 BRAGG"), true, 0.001, 2.0, this, QString(":deg"));
	stepsBraggMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-12 BRAGG"), QString("SMTR1607-5-I22-12"), QString("SMTR1607-5-I22-12 BRAGG"), false, 0.001, 2.0, this, QString(":deg"));
	verticalMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-13 VERTICAL"), QString("SMTR1607-5-I22-13"), QString("SMTR1607-5-I22-13 VERTICAL"), true, 0.05, 2.0, this);
	lateralMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-14 LATERAL"), QString("SMTR1607-5-I22-14"), QString("SMTR1607-5-I22-14 LATERAL"), true, 0.05, 2.0, this);
	crystalChangeMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-22 XTAL XCHAGE"), QString("SMTR1607-5-I22-22"), QString("SMTR1607-5-I22-22 XTAL XCHAGE"), true, 0.05, 2.0, this);
	crystal1PitchMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), QString("SMTR1607-5-I22-23"), QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), true, 0.05, 2.0, this, QString(":V"));
	crystal1RollMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-24 XTAL 1 ROLL"), QString("SMTR1607-5-I22-24"), QString("SMTR1607-5-I22-24 XTAL 1 ROLL"),   true, 0.05, 2.0, this, QString(":V"));
	crystal2PitchMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), QString("SMTR1607-5-I22-25"), QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), true, 0.05, 2.0, this, QString(":V"));
	crystal2RollMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), QString("SMTR1607-5-I22-26"), QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), true, 0.05, 2.0, this, QString(":V"));

	// Create mask control.

	mask_ = new BioXASSideMonochromatorMask(this);

	// Create region control.

	region_ = new BioXASSSRLMonochromatorRegionControl(name()+"RegionControl", this);
	region_->setMask(mask_);
	region_->setPaddleControl(paddle_);
	region_->setPaddleStatusControl(paddleStatus_);
	region_->setKeyStatusControl(keyStatus_);
	region_->setBrakeStatusControl(brakeStatus_);
	region_->setBraggControl(braggMotor());
	region_->setBraggAtCrystalChangePositionStatusControl(braggAtCrystalChangePositionStatus_);
	region_->setCrystalChangeControl(crystalChange_);
	region_->setCrystalChangeCWLimitStatusControl(crystalChangeCWLimitStatus_);
	region_->setCrystalChangeCCWLimitStatusControl(crystalChangeCCWLimitStatus_);
	region_->setRegionAStatusControl(regionAStatus_);
	region_->setRegionBStatusControl(regionBStatus_);

	// Create energy control.

	encoderEnergy_ = new BioXASSSRLMonochromatorEnergyControl(name()+"EncoderEnergyControl", this);
	encoderEnergy_->setBraggControl(encoderBraggMotor_);
	encoderEnergy_->setRegionControl(region_);
	encoderEnergy_->setM1MirrorPitchControl(m1Pitch_);

	stepEnergy_ = new BioXASSSRLMonochromatorEnergyControl(name()+"StepEnergyControl", this);
	stepEnergy_->setBraggControl(stepsBraggMotor_);
	stepEnergy_->setRegionControl(region_);
	stepEnergy_->setM1MirrorPitchControl(m1Pitch_);

	// Listen to connection states.

	connect( paddle_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( paddleStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( keyStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( brakeStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( braggAtCrystalChangePositionStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( crystalChange_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( crystalChangeCWLimitStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( crystalChangeCCWLimitStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( regionAStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( regionBStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	connect( paddleMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( encoderBraggMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( stepsBraggMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( verticalMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( lateralMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( crystalChangeMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( crystal1PitchMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( crystal1RollMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( crystal2PitchMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( crystal2RollMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	connect( region_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( encoderEnergy_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( stepEnergy_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( mask_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Current settings.

	updateConnected();
	updateMotorSettlingTime();
}

BioXASSideMonochromator::~BioXASSideMonochromator()
{

}
