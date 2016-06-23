#ifndef CLSTEMPERATUREMONITOR_H
#define CLSTEMPERATUREMONITOR_H

#include "beamline/CLS/CLSBeamlineControl.h"

class CLSTemperatureMonitor : public CLSBeamlineControl
{
	Q_OBJECT

public:
	/// Constructor.
	CLSTemperatureMonitor(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSTemperatureMonitor();
};

#endif // CLSTEMPERATUREMONITOR_H
