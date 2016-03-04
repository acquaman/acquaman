#include "BioXASSideCryostat.h"
#include "beamline/AMPVControl.h"

BioXASSideCryostat::BioXASSideCryostat(const QString &name, QObject *parent) :
	BioXASCryostat(name, parent)
{
	setPressure(new AMReadOnlyPVControl("TC1607-6-01:READ_TEMP_SIG_TEMP:1", "TC1607-6-01:READ_TEMP_SIG_TEMP:1", this));
	setTemperature(new AMReadOnlyPVControl("TMP1607-6-01:pressure", "TMP1607-6-01:pressure", this));
}

BioXASSideCryostat::~BioXASSideCryostat()
{

}
