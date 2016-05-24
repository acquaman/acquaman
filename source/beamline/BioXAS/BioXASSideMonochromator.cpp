#include "BioXASSideMonochromator.h"

BioXASSideMonochromator::BioXASSideMonochromator(const QString &deviceName, QObject *parent) :
	BioXASSSRLMonochromator(deviceName, parent)
{	
	// Mask controls.

	CLSMAXvMotor *upperBlade = new CLSMAXvMotor(QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), QString("SMTR1607-5-I22-09"), QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), true, 0.1, 2.0, this);
	upperBlade->setMinimumValueOverride(0);
	upperBlade->setMaximumValueOverride(1);
	setUpperBlade(upperBlade);

	CLSMAXvMotor *lowerBlade = new CLSMAXvMotor(QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), QString("SMTR1607-5-I22-10"), QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), true, 0.1, 2.0, this);
	lowerBlade->setMinimumValueOverride(-4);
	lowerBlade->setMaximumValueOverride(0);
	setLowerBlade(lowerBlade);

	setBladesState(new AMReadOnlyPVControl("BL1607-5-I22:Mono:SlitsClosed", "BL1607-5-I22:Mono:SlitsClosed", this));

	setPaddle(new CLSMAXvMotor(QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), QString("SMTR1607-5-I22-11"), QString("SMTR1607-5-I22-11 PHOSPHOR PADDLE"), false, 1, 2.0, this));
	setPaddleStatus(new AMReadOnlyPVControl(name()+"PaddleStatus", "BL1607-5-I22:Mono:PaddleExtracted", this));

	setKeyStatus(new AMReadOnlyPVControl(name()+"KeyStatus", "BL1607-5-I22:Mono:KeyStatus", this));
	setBrakeStatus(new AMReadOnlyPVControl(name()+"BrakeStatus", "BL1607-5-I22:Mono:BrakeOff", this));
	setBraggAtCrystalChangePositionStatus(new AMReadOnlyPVControl(name()+"AtCrystalChangePosition", "BL1607-5-I22:Mono:XtalChangePos", this));
	setCrystalChange(new CLSMAXvMotor(QString("SMTR1607-5-I22-22 XTAL XCHANGE"), QString("SMTR1607-5-I22-22"), QString("SMTR1607-5-I22-22 XTAL XCHAGE"), false, 0.05, 2.0, this));
	setRegionAStatus(new AMReadOnlyPVControl(name()+"RegionAStatus", "BL1607-5-I22:Mono:Region:A", this));
	setRegionBStatus(new AMReadOnlyPVControl(name()+"RegionBStatus", "BL1607-5-I22:Mono:Region:B", this));

	CLSMAXvMotor *verticalMotor = new CLSMAXvMotor(QString("SMTR1607-5-I22-13 VERTICAL"), QString("SMTR1607-5-I22-13"), QString("SMTR1607-5-I22-13 VERTICAL"), true, 0.05, 2.0, this);
	verticalMotor->setMinimumValueOverride(1400);
	verticalMotor->setMaximumValueOverride(1420);
	setVertical(verticalMotor);

	setLateral(new CLSMAXvMotor(QString("SMTR1607-5-I22-14 LATERAL"), QString("SMTR1607-5-I22-14"), QString("SMTR1607-5-I22-14 LATERAL"), true, 0.05, 2.0, this));
	setCrystal1Pitch(new CLSMAXvMotor(QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), QString("SMTR1607-5-I22-23"), QString("SMTR1607-5-I22-23 XTAL 1 PITCH"), true, 0.05, 2.0, this, QString(":V")));
	setCrystal1Roll(new CLSMAXvMotor(QString("SMTR1607-5-I22-24 XTAL 1 ROLL"), QString("SMTR1607-5-I22-24"), QString("SMTR1607-5-I22-24 XTAL 1 ROLL"),   true, 0.05, 2.0, this, QString(":V")));
	setCrystal2Pitch(new CLSMAXvMotor(QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), QString("SMTR1607-5-I22-25"), QString("SMTR1607-5-I22-25 XTAL 2 PITCH"), true, 0.05, 2.0, this, QString(":V")));
	setCrystal2Roll(new CLSMAXvMotor(QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), QString("SMTR1607-5-I22-26"), QString("SMTR1607-5-I22-26 XTAL 2 ROLL"), true, 0.05, 2.0, this, QString(":V")));

	setStepBragg(new CLSMAXvMotor(QString("SMTR1607-5-I22-12 BRAGG"), QString("SMTR1607-5-I22-12"), QString("SMTR1607-5-I22-12 BRAGG"), false, 0.001, 2.0, this, QString(":deg")));
	setEncoderBragg(new CLSMAXvMotor(QString("SMTR1607-5-I22-12 BRAGG"), QString("SMTR1607-5-I22-12"), QString("SMTR1607-5-I22-12 BRAGG"), true, 0.001, 2.0, this, QString(":deg")));

	setStepEnergy(new BioXASSSRLMonochromatorEnergyControl(name()+"StepEnergy", this));
	setEncoderEnergy(new BioXASSSRLMonochromatorEnergyControl(name()+"EncoderEnergy", this));

	setRegion(new BioXASSSRLMonochromatorRegionControl(name()+"Region", this));
}

BioXASSideMonochromator::~BioXASSideMonochromator()
{

}
