#include "CLSFlowSwitch.h"

CLSFlowSwitch::CLSFlowSwitch(const QString &name, const QString &baseName, QObject *parent) :
	CLSBeamlineControl(name, "", parent)
{
	setStatusControl(new AMReadOnlyPVControl(baseName, baseName, this), Good, Bad);
}

CLSFlowSwitch::~CLSFlowSwitch()
{

}
