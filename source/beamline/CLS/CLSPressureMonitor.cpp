#include "CLSPressureMonitor.h"

CLSPressureMonitor::CLSPressureMonitor(const QString &name, const QString &baseName, QObject *parent) :
	CLSBeamlineControl(name, "Torr", parent)
{
	setStatusControl(new AMReadOnlyPVControl(QString("%1:vac").arg(baseName), QString("%1:vac").arg(baseName), this), Good, Bad);
	setValueControl(new AMReadOnlyPVControl(QString("%1:vac:p").arg(baseName), QString("%1:vac:p").arg(baseName), this));
}

CLSPressureMonitor::~CLSPressureMonitor()
{

}

