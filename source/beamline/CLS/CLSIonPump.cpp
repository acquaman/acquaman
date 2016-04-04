#include "CLSIonPump.h"
#include "beamline/AMPVControl.h"

CLSIonPump::CLSIonPump(const QString &name, const QString &baseName, QObject *parent) :
	AMBeamlineControl(name, "", parent)
{
	setStatusControl(new AMReadOnlyPVControl(baseName, baseName, this), Good, Bad);
}

CLSIonPump::~CLSIonPump()
{

}
