#include "BioXASSideMonochromator.h"
#include <QDebug>

BioXASSideMonochromator::BioXASSideMonochromator(QObject *parent) :
	BioXASSSRLMonochromator("SideMono", parent)
{
	// Create components.

//	upperSlit_ = new AMPVwStatusControl("UpperSlitBlade", "SMTR1607-5-I22-09:mm:fbk", "SMTR1607-5-I22-09:mm", "SMTR1607-5-I22-09:status", "SMTR1607-5-I22-09:stop", this);
//	lowerSlit_ = new AMPVwStatusControl("LowerSlitBlade", "SMTR1607-5-I22-10:mm:fbk", "SMTR1607-5-I22-10:mm", "SMTR1607-5-I22-10:status", "SMTR1607-5-I22-10:stop", this);
	slitsStatus_ = new AMReadOnlyPVControl("SlitsStatus", "BL1607-5-I22:Mono:SlitsClosed", this);
//	paddle_ = new AMPVwStatusControl("Paddle", "SMTR1607-5-I22-11:mm:fbk", "SMTR1607-5-I22-11:mm", "SMTR1607-5-I22-11:status", "SMTR1607-5-I22-11:stop", this);
	paddleStatus_ = new AMReadOnlyPVControl("PaddleStatus", "BL1607-5-I22:Mono:PaddleExtracted", this);
	keyStatus_ = new AMReadOnlyPVControl("KeyStatus", "BL1607-5-I22:Mono:KeyStatus", this);
	brakeStatus_ = new AMReadOnlyPVControl("BrakeStatus", "BL1607-5-I22:Mono:BrakeOff", this);
	braggAtCrystalChangePositionStatus_ = new AMReadOnlyPVControl("AtCrystalChangePosition", "BL1607-5-I22:Mono:XtalChangePos", this);
//	crystalChange_ = new AMPVwStatusControl("CrystalChange", "SMTR1607-5-I22-22:mm:fbk", "SMTR1607-5-I22-22:mm", "SMTR1607-5-I22-22:status", "SMTR1607-5-I22-22:stop", this);
//	crystalChangeCWLimitStatus_ = new AMReadOnlyPVControl("CrystalChangeCWStatus", "SMTR1607-5-I22-22:cw", this);
//	crystalChangeCCWLimitStatus_ = new AMReadOnlyPVControl("CrystalChangeCCWStatus", "SMTR1607-5-I22-22:ccw", this);
	regionAStatus_ = new AMReadOnlyPVControl("RegionAStatus", "BL1607-5-I22:Mono:Region:A", this);
	regionBStatus_ = new AMReadOnlyPVControl("RegionBStatus", "BL1607-5-I22:Mono:Region:B", this);

	setUpperSlit(new CLSMAXvMotor(QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), QString("SMTR1607-5-I22-09"), QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), true, 0.1, 2.0, this));
	setLowerSlit(new CLSMAXvMotor(QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), QString("SMTR1607-5-I22-10"), QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), true, 0.1, 2.0, this);
	setPaddle(new CLSMAXvMotor(QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), QString("SMTR1607-5-I22-11"), QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), true, 0.05, 2.0, this));
	setEncoderBragg(new CLSMAXvMotor(QString("SMTR1607-5-I22-12 BRAGG"), QString("SMTR1607-5-I22-12"), QString("SMTR1607-5-I22-12 BRAGG"), true, 0.001, 2.0, this, QString(":deg"));
	addChildControl(encoderBraggMotor_);

	stepsBraggMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-12 BRAGG"), QString("SMTR1607-5-I22-12"), QString("SMTR1607-5-I22-12 BRAGG"), false, 0.001, 2.0, this, QString(":deg"));
	addChildControl(stepsBraggMotor_);

	setVertical(new CLSMAXvMotor(QString("SMTR1607-5-I22-13 VERTICAL"), QString("SMTR1607-5-I22-13"), QString("SMTR1607-5-I22-13 VERTICAL"), true, 0.05, 2.0, this));
	setLateral(new CLSMAXvMotor(QString("SMTR1607-5-I22-14 LATERAL"), QString("SMTR1607-5-I22-14"), QString("SMTR1607-5-I22-14 LATERAL"), true, 0.05, 2.0, this));
	setCrystalChange(new CLSMAXvMotor(QString("SMTR1607-5-I22-22 XTAL XCHAGE"), QString("SMTR1607-5-I22-22"), QString("SMTR1607-5-I22-22 XTAL XCHAGE"), true, 0.05, 2.0, this));
	setCrystal1Pitch(new CLSMAXvMotor(QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), QString("SMTR1607-5-I22-23"), QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), true, 0.05, 2.0, this, QString(":V"));
	setCrystal1Roll(new CLSMAXvMotor(QString("SMTR1607-5-I22-24 XTAL 1 ROLL"), QString("SMTR1607-5-I22-24"), QString("SMTR1607-5-I22-24 XTAL 1 ROLL"),   true, 0.05, 2.0, this, QString(":V"));
	crystal2PitchMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), QString("SMTR1607-5-I22-25"), QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), true, 0.05, 2.0, this, QString(":V"));
	crystal2RollMotor_ = new CLSMAXvMotor(QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), QString("SMTR1607-5-I22-26"), QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), true, 0.05, 2.0, this, QString(":V"));



	// Create energy control.

	setStepEnergy(new BioXASSSRLMonochromatorEnergyControl(name()+"StepEnergyControl", this));
	setEncoderEnergy(new BioXASSSRLMonochromatorEnergyControl(name()+"EncoderEnergyControl", this));

	// Create region control.

	setRegion(new BioXASSSRLMonochromatorRegionControl(name()+"RegionControl", this));
}

BioXASSideMonochromator::~BioXASSideMonochromator()
{

}

//bool BioXASSideMonochromator::stop() const
//{
//	bool stopped = false;

//	if (lowerSlitMotor_->stop())
//		stopped = true;

//	qDebug() << "\n\nLower slit stopped:" << stopped;

//	return stopped;
//}
