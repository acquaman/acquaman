#ifndef BIOXASSIDECARBONFILTERFARM_H
#define BIOXASSIDECARBONFILTERFARM_H

#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"
#include "beamline/BioXAS/BioXASSideCarbonFilterFarmControl.h"

// Valid actuator positions.
#define BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT 18.996
#define BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM -3.5
#define BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP -15.1
#define BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT 19.15
#define BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM -3.5
#define BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP -15.2

class BioXASSideCarbonFilterFarm : public BioXASCarbonFilterFarm
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideCarbonFilterFarm(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideCarbonFilterFarm();
};

#endif // BIOXASSIDECARBONFILTERFARM_H
