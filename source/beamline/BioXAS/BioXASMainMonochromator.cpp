#include "BioXASMainMonochromator.h"

BioXASMainMonochromator::BioXASMainMonochromator(QObject *parent) :
	BioXASSSRLMonochromator(parent)
{
	// Initialize variables.

	connected_ = false;

	// Create components.

	upperSlit_ = new AMPVControl("UpperSlitBlade", "SMTR1607-5-I21-09:mm:fbk", "SMTR1607-5-I21-09:mm", QString(), this);
	lowerSlit_ = new AMPVControl("LowerSlitBlade", "SMTR1607-5-I21-10:mm:fbk", "SMTR1607-5-I21-10:mm", QString(), this);
	slitsStatus_ = new AMReadOnlyPVControl(QString("SlitsStatus"), QString("BL1607-5-I21:Mono:SlitsClosed"), this);
	paddle_ = new AMPVControl("Paddle", "SMTR1607-5-I21-11:mm:fbk", "SMTR1607-5-I21-11:mm", QString(), this);
	paddleStatus_ = new AMReadOnlyPVControl(QString("PaddleStatus"), QString("BL1607-5-I21:Mono:PaddleExtracted"), this);
	keyStatus_ = new AMReadOnlyPVControl(QString("KeyStatus"), QString("BL1607-5-I21:Mono:KeyStatus"), this);
	brakeStatus_ = new AMReadOnlyPVControl(QString("BrakeStatus"), QString("BL1607-5-I21:Mono:BrakeOff"), this);
	bragg_ = new AMPVControl("Bragg", "SMTR1607-5-I21-12:deg:fbk", "SMTR1607-5-I21-12:deg", QString(), this);
	braggAtCrystalChangePositionStatus_ = new AMReadOnlyPVControl("AtCrystalChangePosition", "BL1607-5-I21:Mono:XtalChangePos", this);
	crystalChange_ = new AMPVControl("CrystalChange", "SMTR1607-5-I21-22:mm:fbk", "SMTR1607-5-I21-22:mm", QString(), this);
	crystalChangeCWLimitStatus_ = new AMReadOnlyPVControl("CrystalChangeCWStatus", "SMTR1607-5-I21-22:cw", this);
	crystalChangeCCWLimitStatus_ = new AMReadOnlyPVControl("CrystalChangeCCWStatus", "SMTR1607-5-I21-22:ccw", this);
	regionAStatus_ = new AMReadOnlyPVControl("RegionAStatus", "BL1607-5-I21:Mono:Region:A", this);
	regionBStatus_ = new AMReadOnlyPVControl("RegionBStatus", "BL1607-5-I21:Mono:Region:B", this);

	braggSetPosition_ = new AMSinglePVControl("BraggSetPositionControl", "SMTR1607-5-I21-12:deg:setPosn", this);
	m1MirrorOffset_ = new AMSinglePVControl("M1MirrorOffset", "BL1607-5-I21:Energy:EV:fbk:tr.H", this);

	upperSlitMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), QString("SMTR1607-5-I21-09"), QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), true, 0.1, 2.0, this);
	lowerSlitMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-10 VERT LOWER BLADE"), QString("SMTR1607-5-I21-10"), QString("SMTR1607-5-I21-10 VERT LOWER BLADE"), true, 0.1, 2.0, this);
	paddleMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-11 PHOSPHOR PADDLE"), QString("SMTR1607-5-I21-11"), QString("SMTR1607-5-I21-11 PHOSPHOR PADDLE"), false, 0.1, 2.0, this);
	braggMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-12 BRAGG"), QString("SMTR1607-5-I21-12"), QString("SMTR1607-5-I21-12 BRAGG"), true, 0.05, 2.0, this, QString(":deg"));
	verticalMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-13 VERTICAL"), QString("SMTR1607-5-I21-13"), QString("SMTR1607-5-I21-13 VERTICAL"), true, 0.05, 2.0, this);
	lateralMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-14 LATERAL"), QString("SMTR1607-5-I21-14"), QString("SMTR1607-5-I21-14 LATERAL"), true, 0.05, 2.0, this);
	crystalChangeMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-22 XTAL XCHAGE"), QString("SMTR1607-5-I21-22"), QString("SMTR1607-5-I21-22 XTAL XCHAGE"), true, 0.05, 2.0, this);
	crystal1PitchMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-23 XTAL 1 PITCH"), QString("SMTR1607-5-I21-23"), QString("SMTR1607-5-I21-23 XTAL 1 PITCH"), true, 0.05, 2.0, this, QString(":V"));
	crystal1RollMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-24 XTAL 1 ROLL"), QString("SMTR1607-5-I21-24"), QString("SMTR1607-5-I21-24 XTAL 1 ROLL"),   true, 0.05, 2.0, this, QString(":V"));
	crystal2PitchMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-25 XTAL 2 PITCH"), QString("SMTR1607-5-I21-25"), QString("SMTR1607-5-I21-25 XTAL 2 PITCH"), true, 0.05, 2.0, this, QString(":V"));
	crystal2RollMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I21-26 XTAL 2 ROLL"), QString("SMTR1607-5-I21-26"), QString("SMTR1607-5-I21-26 XTAL 2 ROLL"), true, 0.05, 2.0, this, QString(":V"));

	// Create region control.

	region_ = new BioXASSSRLMonochromatorRegionControl(this);
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

	energy_ = new BioXASSSRLMonochromatorEnergyControl("MainEnergy", this);
	energy_->setBraggControl(bragg_);
	energy_->setBraggSetPositionControl(braggSetPosition_);
	energy_->setRegionControl(region_);
	energy_->setM1MirrorControl(m1MirrorOffset_);

	// Listen to connection states.

	connect( upperSlit_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( lowerSlit_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( slitsStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( paddle_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( paddleStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( keyStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( brakeStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( bragg_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
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
	connect( energy_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Current settings.

	updateConnected();
}

BioXASMainMonochromator::~BioXASMainMonochromator()
{

}

