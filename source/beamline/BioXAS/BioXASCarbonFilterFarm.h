#ifndef BIOXASCARBONFILTERFARM_H
#define BIOXASCARBONFILTERFARM_H

#include "beamline/BioXAS/BioXASCarbonFilterFarmControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmFilterControl.h"

class BioXASCarbonFilterFarm : public BioXASCarbonFilterFarmControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarm(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarm();
};

#endif // BIOXASCARBONFILTERFARM_H
