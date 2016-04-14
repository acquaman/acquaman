#ifndef CLSPRESSUREMONITOR_H
#define CLSPRESSUREMONITOR_H

#include "beamline/CLS/CLSBeamlineControl.h"

class CLSPressureMonitor : public CLSBeamlineControl
{
	Q_OBJECT

public:
	/// Constructor.
	CLSPressureMonitor(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSPressureMonitor();
};

#endif // CLSPRESSUREMONITOR_H
