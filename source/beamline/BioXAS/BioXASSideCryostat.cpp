#include "BioXASSideCryostat.h"
#include "beamline/AMPVControl.h"

BioXASSideCryostat::BioXASSideCryostat(const QString &name, QObject *parent) :
	BioXASCryostat(name, parent)
{
	setPressure(new AMReadOnlyPVControl("TMP1607-6-01:Pressure", "TMP1607-6-01:Pressure", this));
	setTemperature(new AMReadOnlyPVControl("TC1607-6-01:READ_TEMP_SIG_TEMP:1", "TC1607-6-01:READ_TEMP_SIG_TEMP:1", this));
}

BioXASSideCryostat::~BioXASSideCryostat()
{

}
