#ifndef BIOXASSIDEVALVES_H
#define BIOXASSIDEVALVES_H

#include "beamline/BioXAS/BioXASValves.h"

class BioXASSideValves : public BioXASValves
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASSideValves(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideValves();
};

#endif // BIOXASSIDEVALVES_H
