#ifndef CLSPRESSUREMONITOR_H
#define CLSPRESSUREMONITOR_H

#include "beamline/AMBeamlineControl.h"

class CLSPressureMonitor : public AMBeamlineControl
{
	Q_OBJECT

public:
	/// An enumeration of the possible status states.
	enum Status { Bad = 0, Good = 1 };

	/// Constructor.
	CLSPressureMonitor(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSPressureMonitor();
};

#endif // CLSPRESSUREMONITOR_H
