#ifndef BIOXASIMAGINGVALVES_H
#define BIOXASIMAGINGVALVES_H

#include "beamline/BioXAS/BioXASValves.h"

class BioXASImagingValves : public BioXASValves
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASImagingValves(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASImagingValves();
};

#endif // BIOXASIMAGINGVALVES_H
