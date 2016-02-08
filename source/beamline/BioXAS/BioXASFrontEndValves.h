#ifndef BIOXASFRONTENDVALVES_H
#define BIOXASFRONTENDVALVES_H

#include "beamline/BioXAS/BioXASValves.h"

#define BIOXASFRONTENDVALVES_VALVE_OPEN 1
#define BIOXASFRONTENDVALVES_VALVE_CLOSED 4
#define BIOXASFRONTENDVALVES_FASTVALVE_OPEN 1
#define BIOXASFRONTENDVALVES_FASTVALVE_CLOSED 4

class BioXASFrontEndValves : public BioXASValves
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASFrontEndValves(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndValves();
};

#endif // BIOXASFRONTENDVALVES_H
