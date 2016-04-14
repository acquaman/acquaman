#include "CLSFlowTransducer.h"

CLSFlowTransducer::CLSFlowTransducer(const QString &name, const QString &baseName, QObject *parent) :
	CLSBeamlineControl(name, "L/min", parent)
{
	setStatusControl(new AMReadOnlyPVControl(QString("%1:lowflow").arg(baseName), QString("%1:lowflow").arg(baseName), this), Good, Bad);
	setValueControl(new AMReadOnlyPVControl(baseName, baseName, this));
}

CLSFlowTransducer::~CLSFlowTransducer()
{

}
