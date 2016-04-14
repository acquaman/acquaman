#include "CLSFlowSwitch.h"
#include "beamline/AMPVControl.h"

CLSFlowSwitch::CLSFlowSwitch(const QString &name, const QString &baseName, QObject *parent) :
	AMBeamlineControl(name, "", parent)
{
	setStatusControl(new AMReadOnlyPVControl(baseName, baseName, this), Good, Bad);
}

CLSFlowSwitch::~CLSFlowSwitch()
{

}
