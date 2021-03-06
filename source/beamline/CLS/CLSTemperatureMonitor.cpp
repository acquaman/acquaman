#include "CLSTemperatureMonitor.h"

CLSTemperatureMonitor::CLSTemperatureMonitor(const QString &name, const QString &baseName, QObject *parent) :
	CLSBeamlineControl(name, "C", parent)
{
	setStatusControl(new AMReadOnlyPVControl(QString("%1:trip").arg(baseName), QString("%1:trip").arg(baseName), this), Good, Bad);
	setValueControl(new AMReadOnlyPVControl(baseName, baseName, this));
}

CLSTemperatureMonitor::~CLSTemperatureMonitor()
{

}

