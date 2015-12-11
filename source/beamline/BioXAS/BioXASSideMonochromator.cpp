#include "BioXASSideMonochromator.h"

BioXASSideMonochromator::BioXASSideMonochromator(QObject *parent) :
	BioXASSSRLMonochromator("SideMono", parent)
{	
	setUpperSlit(new CLSMAXvMotor(QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), QString("SMTR1607-5-I22-09"), QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), true, 0.1, 2.0, this));
	setLowerSlit(new CLSMAXvMotor(QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), QString("SMTR1607-5-I22-10"), QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), true, 0.1, 2.0, this));
	setSlitsStatus(new AMReadOnlyPVControl("SlitsStatus", "BL1607-5-I22:Mono:SlitsClosed", this));
	setPaddle(new CLSMAXvMotor(QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), QString("SMTR1607-5-I22-11"), QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), true, 0.05, 2.0, this));
	setPaddleStatus(new AMReadOnlyPVControl("PaddleStatus", "BL1607-5-I22:Mono:PaddleExtracted", this));
	setKeyStatus(new AMReadOnlyPVControl("KeyStatus", "BL1607-5-I22:Mono:KeyStatus", this));
	setBrakeStatus(new AMReadOnlyPVControl("BrakeStatus", "BL1607-5-I22:Mono:BrakeOff", this));
	setBraggAtCrystalChangePositionStatus(new AMReadOnlyPVControl("AtCrystalChangePosition", "BL1607-5-I22:Mono:XtalChangePos", this));
	setCrystalChange(new CLSMAXvMotor(QString("SMTR1607-5-I22-22 XTAL XCHAGE"), QString("SMTR1607-5-I22-22"), QString("SMTR1607-5-I22-22 XTAL XCHAGE"), true, 0.05, 2.0, this));
	setRegionAStatus(new AMReadOnlyPVControl("RegionAStatus", "BL1607-5-I22:Mono:Region:A", this));
	setRegionBStatus(new AMReadOnlyPVControl("RegionBStatus", "BL1607-5-I22:Mono:Region:B", this));
	setVertical(new CLSMAXvMotor(QString("SMTR1607-5-I22-13 VERTICAL"), QString("SMTR1607-5-I22-13"), QString("SMTR1607-5-I22-13 VERTICAL"), true, 0.05, 2.0, this));
	setLateral(new CLSMAXvMotor(QString("SMTR1607-5-I22-14 LATERAL"), QString("SMTR1607-5-I22-14"), QString("SMTR1607-5-I22-14 LATERAL"), true, 0.05, 2.0, this));
	setCrystal1Pitch(new CLSMAXvMotor(QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), QString("SMTR1607-5-I22-23"), QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), true, 0.05, 2.0, this, QString(":V")));
	setCrystal1Roll(new CLSMAXvMotor(QString("SMTR1607-5-I22-24 XTAL 1 ROLL"), QString("SMTR1607-5-I22-24"), QString("SMTR1607-5-I22-24 XTAL 1 ROLL"),   true, 0.05, 2.0, this, QString(":V")));
	setCrystal2Pitch(new CLSMAXvMotor(QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), QString("SMTR1607-5-I22-25"), QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), true, 0.05, 2.0, this, QString(":V")));
	setCrystal2Roll(new CLSMAXvMotor(QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), QString("SMTR1607-5-I22-26"), QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), true, 0.05, 2.0, this, QString(":V")));

	setStepBragg(new CLSMAXvMotor(QString("SMTR1607-5-I22-12 BRAGG"), QString("SMTR1607-5-I22-12"), QString("SMTR1607-5-I22-12 BRAGG"), false, 0.001, 2.0, this, QString(":deg")));
	setEncoderBragg(new CLSMAXvMotor(QString("SMTR1607-5-I22-12 BRAGG"), QString("SMTR1607-5-I22-12"), QString("SMTR1607-5-I22-12 BRAGG"), true, 0.001, 2.0, this, QString(":deg")));

	setStepEnergy(new BioXASSSRLMonochromatorEnergyControl(name()+"StepEnergyControl", this));
	setEncoderEnergy(new BioXASSSRLMonochromatorEnergyControl(name()+"EncoderEnergyControl", this));

	setRegion(new BioXASSSRLMonochromatorRegionControl(name()+"RegionControl", this));
}

BioXASSideMonochromator::~BioXASSideMonochromator()
{

}
