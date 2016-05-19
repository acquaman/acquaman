#include "BioXASSideMonochromatorMask.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"

BioXASSideMonochromatorMask::BioXASSideMonochromatorMask(const QString &deviceName, QObject *parent) :
	BioXASSSRLMonochromatorMask(deviceName, parent)
{
	BioXASMAXvMotor *upperBlade = new BioXASMAXvMotor(QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), QString("SMTR1607-5-I22-09"), QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), true, 0.1, 2.0, this);
	upperBlade->setMinimumValueOverride(0);
	upperBlade->setMaximumValueOverride(1);
	upperBlade->setLimitSetpoint(CLSMAXvMotor::LimitCW, -10);
	upperBlade->setLimitSetpoint(CLSMAXvMotor::LimitCCW, 10);
	setUpperBlade(upperBlade);

	BioXASMAXvMotor *lowerBlade = new BioXASMAXvMotor(QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), QString("SMTR1607-5-I22-10"), QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), true, 0.1, 2.0, this);
	lowerBlade->setMinimumValueOverride(-4);
	lowerBlade->setMaximumValueOverride(0);
	lowerBlade->setLimitSetpoint(CLSMAXvMotor::LimitCW, 10);
	lowerBlade->setLimitSetpoint(CLSMAXvMotor::LimitCCW, -10);
	setLowerBlade(lowerBlade);

	setBladesState(new AMReadOnlyPVControl("BL1607-5-I22:Mono:SlitsClosed", "BL1607-5-I22:Mono:SlitsClosed", this));
}

BioXASSideMonochromatorMask::~BioXASSideMonochromatorMask()
{

}
