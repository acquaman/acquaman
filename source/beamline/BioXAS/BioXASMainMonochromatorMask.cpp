#include "BioXASMainMonochromatorMask.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"
#include "beamline/CLS/CLSMAXvMotor.h"

BioXASMainMonochromatorMask::BioXASMainMonochromatorMask(const QString &deviceName, QObject *parent) :
	BioXASSSRLMonochromatorMask(deviceName, parent)
{
	setUpperBlade(new CLSMAXvMotor(QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), QString("SMTR1607-5-I21-09"), QString("SMTR1607-5-I21-09 VERT UPPER BLADE"), true, 0.1, 2.0, this));
	setLowerBlade(new CLSMAXvMotor(QString("SMTR1607-5-I21-10 VERT LOWER BLADE"), QString("SMTR1607-5-I21-10"), QString("SMTR1607-5-I21-10 VERT LOWER BLADE"), true, 0.1, 2.0, this));
	setBladesState(new AMReadOnlyPVControl(QString("BL1607-5-I21:Mono:SlitsClosed"), QString("BL1607-5-I21:Mono:SlitsClosed"), this));
}

BioXASMainMonochromatorMask::~BioXASMainMonochromatorMask()
{

}
