#include "BioXASSideMonochromatorMask.h"
#include "beamline/CLS/CLSMAXvMotor.h"

BioXASSideMonochromatorMask::BioXASSideMonochromatorMask(QObject *parent) :
	BioXASSSRLMonochromatorMaskControl("BioXASSideMonochromatorMask", parent)
{
	setUpperBlade(new CLSMAXvMotor(QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), QString("SMTR1607-5-I22-09"), QString("SMTR1607-5-I22-09 VERT UPPER BLADE"), true, 0.1, 2.0, this));
	setLowerBlade(new CLSMAXvMotor(QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), QString("SMTR1607-5-I22-10"), QString("SMTR1607-5-I22-10 VERT LOWER BLADE"), true, 0.1, 2.0, this));
	setStatus(new AMReadOnlyPVControl(name()+"Status", "BL1607-5-I22:Mono:SlitsClosed", this));
}

BioXASSideMonochromatorMask::~BioXASSideMonochromatorMask()
{

}
