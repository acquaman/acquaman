#ifndef BIOXASBEAMSTATUS_H
#define BIOXASBEAMSTATUS_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"

class BioXASBeamStatus : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamStatus(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatus();
};

#endif // BIOXASBEAMSTATUS_H
