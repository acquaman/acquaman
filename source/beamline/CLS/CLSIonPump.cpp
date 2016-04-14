#include "CLSIonPump.h"

CLSIonPump::CLSIonPump(const QString &name, const QString &baseName, QObject *parent) :
	CLSBeamlineControl(name, "", parent)
{
	setStatusControl(new AMReadOnlyPVControl(baseName, baseName, this), Good, Bad);
}

CLSIonPump::~CLSIonPump()
{

}
