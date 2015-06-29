#include "BioXASSideMonochromator.h"

BioXASSideMonochromator::BioXASSideMonochromator(QObject *parent) :
	BioXASSSRLMonochromator("SideMono", parent)
{
	// Create components.

	upperSlit_ = new AMPVwStatusControl("UpperSlitBlade", "SMTR1607-5-I22-09:mm:fbk", "SMTR1607-5-I22-09:mm", "SMTR1607-5-I22-09:status", "SMTR1607-5-I22-09:stop", this);
	lowerSlit_ = new AMPVwStatusControl("LowerSlitBlade", "SMTR1607-5-I22-10:mm:fbk", "SMTR1607-5-I22-10:mm", "SMTR1607-5-I22-10:status", "SMTR1607-5-I22-10:stop", this);
	slitsStatus_ = new AMReadOnlyPVControl("SlitsStatus", "BL1607-5-I22:Mono:SlitsClosed", this);
	paddle_ = new AMPVwStatusControl("Paddle", "SMTR1607-5-I22-11:mm:fbk", "SMTR1607-5-I22-11:mm", "SMTR1607-5-I22-11:status", "SMTR1607-5-I22-11:stop", this);
	paddleStatus_ = new AMReadOnlyPVControl("PaddleStatus", "BL1607-5-I22:Mono:PaddleExtracted", this);
	keyStatus_ = new AMReadOnlyPVControl("KeyStatus", "BL1607-5-I22:Mono:KeyStatus", this);
	brakeStatus_ = new AMReadOnlyPVControl("BrakeStatus", "BL1607-5-I22:Mono:BrakeOff", this);
	bragg_ = new AMPVwStatusControl("Bragg", "SMTR1607-5-I22-12:deg:fbk", "SMTR1607-5-I22-12:deg", "SMTR1607-5-I22-12:status", "SMTR1607-5-I22-12:stop", this, 0.05);
	stepBragg_ = new AMPVwStatusControl("StepBragg", "SMTR1607-5-I22-12:deg:sp", "SMTR1607-5-I22-12:deg", "SMTR1607-5-I22-12:status", "SMTR1607-5-I22-12:stop", this, 0.05);
	braggAtCrystalChangePositionStatus_ = new AMReadOnlyPVControl("AtCrystalChangePosition", "BL1607-5-I22:Mono:XtalChangePos", this);
	crystalChange_ = new AMPVwStatusControl("CrystalChange", "SMTR1607-5-I22-22:mm:fbk", "SMTR1607-5-I22-22:mm", "SMTR1607-5-I22-22:status", "SMTR1607-5-I22-22:stop", this);
	crystalChangeCWLimitStatus_ = new AMReadOnlyPVControl("CrystalChangeCWStatus", "SMTR1607-5-I22-22:cw", this);
	crystalChangeCCWLimitStatus_ = new AMReadOnlyPVControl("CrystalChangeCCWStatus", "SMTR1607-5-I22-22:ccw", this);
	regionAStatus_ = new AMReadOnlyPVControl("RegionAStatus", "BL1607-5-I22:Mono:Region:A", this);
	regionBStatus_ = new AMReadOnlyPVControl("RegionBStatus", "BL1607-5-I22:Mono:Region:B", this);

	braggSetPosition_ = new AMSinglePVControl("BraggSetPositionControl", "SMTR1607-5-I22-12:deg:setPosn", this);

	upperSlitMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), QString("SMTR1607-5-I22-09"), QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), true, 0.1, 2.0, this);
	lowerSlitMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), QString("SMTR1607-5-I22-10"), QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), true, 0.1, 2.0, this);
	paddleMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), QString("SMTR1607-5-I22-11"), QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), true, 0.05, 2.0, this);
	braggMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-12 BRAGG"), QString("SMTR1607-5-I22-12"), QString("SMTR1607-5-I22-12 BRAGG"), true, 0.05, 2.0, this, QString(":deg"));
	verticalMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-13 VERTICAL"), QString("SMTR1607-5-I22-13"), QString("SMTR1607-5-I22-13 VERTICAL"), true, 0.05, 2.0, this);
	lateralMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-14 LATERAL"), QString("SMTR1607-5-I22-14"), QString("SMTR1607-5-I22-14 LATERAL"), true, 0.05, 2.0, this);
	crystalChangeMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-22 XTAL XCHAGE"), QString("SMTR1607-5-I22-22"), QString("SMTR1607-5-I22-22 XTAL XCHAGE"), true, 0.05, 2.0, this);
	crystal1PitchMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), QString("SMTR1607-5-I22-23"), QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), true, 0.05, 2.0, this, QString(":V"));
	crystal1RollMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-24 XTAL 1 ROLL"), QString("SMTR1607-5-I22-24"), QString("SMTR1607-5-I22-24 XTAL 1 ROLL"),   true, 0.05, 2.0, this, QString(":V"));
	crystal2PitchMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), QString("SMTR1607-5-I22-25"), QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), true, 0.05, 2.0, this, QString(":V"));
	crystal2RollMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), QString("SMTR1607-5-I22-26"), QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), true, 0.05, 2.0, this, QString(":V"));

	// Create region control.

	region_ = new BioXASSSRLMonochromatorRegionControl(name_+"RegionControl", this);
	region_->setUpperSlitControl(upperSlit_);
	region_->setLowerSlitControl(lowerSlit_);
	region_->setSlitsStatusControl(slitsStatus_);
	region_->setPaddleControl(paddle_);
	region_->setPaddleStatusControl(paddleStatus_);
	region_->setKeyStatusControl(keyStatus_);
	region_->setBrakeStatusControl(brakeStatus_);
	region_->setBraggControl(bragg_);
	region_->setBraggAtCrystalChangePositionStatusControl(braggAtCrystalChangePositionStatus_);
	region_->setCrystalChangeControl(crystalChange_);
	region_->setCrystalChangeCWLimitStatusControl(crystalChangeCWLimitStatus_);
	region_->setCrystalChangeCCWLimitStatusControl(crystalChangeCCWLimitStatus_);
	region_->setRegionAStatusControl(regionAStatus_);
	region_->setRegionBStatusControl(regionBStatus_);

	// Create energy control.

	encoderEnergy_ = new BioXASSSRLMonochromatorEnergyControl(name_+"EncoderEnergyControl", this);
	encoderEnergy_->setBraggControl(bragg_);
	encoderEnergy_->setBraggSetPositionControl(braggSetPosition_);
	encoderEnergy_->setRegionControl(region_);
	encoderEnergy_->setM1MirrorPitchControl(m1Pitch_);

	stepEnergy_ = new BioXASSSRLMonochromatorEnergyControl(name_+"StepEnergyControl", this);
	stepEnergy_->setBraggControl(stepBragg_);
	stepEnergy_->setBraggSetPositionControl(braggSetPosition_);
	stepEnergy_->setRegionControl(region_);
	stepEnergy_->setM1MirrorPitchControl(m1Pitch_);

	// Listen to connection states.

	connect( upperSlit_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( lowerSlit_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( slitsStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( paddle_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( paddleStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( keyStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( brakeStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( bragg_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( stepBragg_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( braggAtCrystalChangePositionStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( crystalChange_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( crystalChangeCWLimitStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( crystalChangeCCWLimitStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( regionAStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( regionBStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	connect( upperSlitMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( lowerSlitMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( paddleMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( braggMotor_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
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

	// Current settings.

	updateConnected();
}

BioXASSideMonochromator::~BioXASSideMonochromator()
{

}
