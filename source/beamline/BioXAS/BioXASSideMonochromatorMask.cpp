#include "BioXASSideMonochromatorMask.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"

BioXASSideMonochromatorMask::BioXASSideMonochromatorMask(const QString &deviceName, QObject *parent) :
	BioXASSSRLMonochromatorMask(deviceName, parent)
{
	CLSMAXvMotor *upperBlade = new CLSMAXvMotor(QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), QString("SMTR1607-5-I22-09"), QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), true, 0.1, 2.0, this);
	upperBlade->setMinimumValue(0);
	upperBlade->setMaximumValue(1);
	setUpperBlade(upperBlade);

	CLSMAXvMotor *lowerBlade = new CLSMAXvMotor(QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), QString("SMTR1607-5-I22-10"), QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), true, 0.1, 2.0, this);
	lowerBlade->setMinimumValue(-4);
	lowerBlade->setMaximumValue(0);
	setLowerBlade(lowerBlade);

	setBladesState(new AMReadOnlyPVControl("BL1607-5-I22:Mono:SlitsClosed", "BL1607-5-I22:Mono:SlitsClosed", this));
}

BioXASSideMonochromatorMask::~BioXASSideMonochromatorMask()
{

}
