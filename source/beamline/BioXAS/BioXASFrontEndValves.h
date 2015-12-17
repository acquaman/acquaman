#ifndef BIOXASFRONTENDVALVES_H
#define BIOXASFRONTENDVALVES_H

#include "beamline/BioXAS/BioXASValves.h"

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
