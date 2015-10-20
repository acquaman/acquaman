#ifndef BIOXASFRONTENDBEAMSTATUS_H
#define BIOXASFRONTENDBEAMSTATUS_H

#include "beamline/BioXAS/BioXASFrontEndBeamStatusControl.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASFrontEndShutters.h"

class BioXASFrontEndBeamStatus : public BioXASFrontEndBeamStatusControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFrontEndBeamStatus(BioXASFrontEndShutters *shutters, BioXASValves *valves, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndBeamStatus();
};

#endif // BIOXASFRONTENDBEAMSTATUS_H
