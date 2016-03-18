#include "CLSTemperatureMonitor.h"

CLSTemperatureMonitor::CLSTemperatureMonitor(const QString &name, const QString &baseName, QObject *parent) :
	AMTemperatureMonitor(name, "C", parent)
{
	setStatusControl(new AMReadOnlyPVControl(QString("%1:trip").arg(baseName), QString("%1:trip").arg(baseName), this), 1, 0);
	setValueControl(new AMReadOnlyPVControl(baseName, baseName, this));
}

CLSTemperatureMonitor::~CLSTemperatureMonitor()
{

}

