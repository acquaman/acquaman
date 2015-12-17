#include "BioXASMainMonochromator.h"

BioXASMainMonochromator::BioXASMainMonochromator(QObject *parent) :
	BioXASSSRLMonochromator("MainMono", parent)
{
	setUpperSlit(new CLSMAXvMotor(QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), QString("SMTR1607-5-I21-09"), QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), true, 0.1, 2.0, this));
	setLowerSlit(new CLSMAXvMotor(QString("SMTR1607-5-I21-10 VERT LOWER BLADE"), QString("SMTR1607-5-I21-10"), QString("SMTR1607-5-I21-10 VERT LOWER BLADE"), true, 0.1, 2.0, this));
	setSlitsStatus(new AMReadOnlyPVControl(QString("SlitsStatus"), QString("BL1607-5-I21:Mono:SlitsClosed"), this));
	setPaddle(new CLSMAXvMotor(QString("SMTR1607-5-I21-11 PHOSPHOR PADDLE"), QString("SMTR1607-5-I21-11"), QString("SMTR1607-5-I21-11 PHOSPHOR PADDLE"), false, 0.1, 2.0, this));
	setPaddleStatus(new AMReadOnlyPVControl(QString("PaddleStatus"), QString("BL1607-5-I21:Mono:PaddleExtracted"), this));
	setKeyStatus(new AMReadOnlyPVControl(QString("KeyStatus"), QString("BL1607-5-I21:Mono:KeyStatus"), this));
	setBrakeStatus(new AMReadOnlyPVControl(QString("BrakeStatus"), QString("BL1607-5-I21:Mono:BrakeOff"), this));
	setBraggAtCrystalChangePositionStatus(new AMReadOnlyPVControl("AtCrystalChangePosition", "BL1607-5-I21:Mono:XtalChangePos", this));
	setCrystalChange(new CLSMAXvMotor(QString("SMTR1607-5-I21-22 XTAL XCHAGE"), QString("SMTR1607-5-I21-22"), QString("SMTR1607-5-I21-22 XTAL XCHAGE"), true, 0.05, 2.0, this));
	setRegionAStatus(new AMReadOnlyPVControl("RegionAStatus", "BL1607-5-I21:Mono:Region:A", this));
	setRegionBStatus(new AMReadOnlyPVControl("RegionBStatus", "BL1607-5-I21:Mono:Region:B", this));
	setVertical(new CLSMAXvMotor(QString("SMTR1607-5-I21-13 VERTICAL"), QString("SMTR1607-5-I21-13"), QString("SMTR1607-5-I21-13 VERTICAL"), true, 0.05, 2.0, this));
	setLateral(new CLSMAXvMotor(QString("SMTR1607-5-I21-14 LATERAL"), QString("SMTR1607-5-I21-14"), QString("SMTR1607-5-I21-14 LATERAL"), true, 0.05, 2.0, this));
	setCrystal1Pitch(new CLSMAXvMotor(QString("SMTR1607-5-I21-23 XTAL 1 PITCH"), QString("SMTR1607-5-I21-23"), QString("SMTR1607-5-I21-23 XTAL 1 PITCH"), true, 0.05, 2.0, this, QString(":V")));
	setCrystal1Roll(new CLSMAXvMotor(QString("SMTR1607-5-I21-24 XTAL 1 ROLL"), QString("SMTR1607-5-I21-24"), QString("SMTR1607-5-I21-24 XTAL 1 ROLL"),   true, 0.05, 2.0, this, QString(":V")));
	setCrystal2Pitch(new CLSMAXvMotor(QString("SMTR1607-5-I21-25 XTAL 2 PITCH"), QString("SMTR1607-5-I21-25"), QString("SMTR1607-5-I21-25 XTAL 2 PITCH"), true, 0.05, 2.0, this, QString(":V")));
	setCrystal2Roll(new CLSMAXvMotor(QString("SMTR1607-5-I21-26 XTAL 2 ROLL"), QString("SMTR1607-5-I21-26"), QString("SMTR1607-5-I21-26 XTAL 2 ROLL"), true, 0.05, 2.0, this, QString(":V")));

	setStepBragg(new CLSMAXvMotor(QString("SMTR1607-5-I21-12 BRAGG"), QString("SMTR1607-5-I21-12"), QString("SMTR1607-5-I21-12 BRAGG"), false, 0.05, 2.0, this, QString(":deg")));
	setEncoderBragg(new CLSMAXvMotor(QString("SMTR1607-5-I21-12 BRAGG"), QString("SMTR1607-5-I21-12"), QString("SMTR1607-5-I21-12 BRAGG"), true, 0.05, 2.0, this, QString(":deg")));

	setStepEnergy(new BioXASSSRLMonochromatorEnergyControl(name()+"StepEnergyControl", this));
	setEncoderEnergy(new BioXASSSRLMonochromatorEnergyControl(name()+"EncoderEnergyControl", this));

	setRegion(new BioXASSSRLMonochromatorRegionControl(name()+"RegionControl", this));

	encoderStepsDiff_ = new AMReadOnlyPVControl(name()+"EncoderStepsDiffControlDeg", "BL1607-7-I21:Mono:fbk:diff", this);
}

BioXASMainMonochromator::~BioXASMainMonochromator()
{

}
