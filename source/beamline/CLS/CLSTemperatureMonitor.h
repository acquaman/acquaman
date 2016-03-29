#ifndef CLSTEMPERATUREMONITOR_H
#define CLSTEMPERATUREMONITOR_H

#include "beamline/AMBeamlineControl.h"

class CLSTemperatureMonitor : public AMBeamlineControl
{
	Q_OBJECT

public:
	/// An enumeration of the possible status states.
	enum Status { Bad = 0, Good = 1 };

	/// Constructor.
	CLSTemperatureMonitor(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSTemperatureMonitor();
};

#endif // CLSTEMPERATUREMONITOR_H
