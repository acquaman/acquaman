#ifndef BIOXASMAINVALVES_H
#define BIOXASMAINVALVES_H

#include "beamline/BioXAS/BioXASValves.h"

class BioXASMainValves : public BioXASValves
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASMainValves(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainValves();
};

#endif // BIOXASMAINVALVES_H
