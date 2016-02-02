#ifndef BIOXASMAINM1MIRROR_H
#define BIOXASMAINM1MIRROR_H

#include "beamline/BioXAS/BioXASM1Mirror.h"

class BioXASMainM1Mirror : public BioXASM1Mirror
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASMainM1Mirror(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainM1Mirror();
};

#endif // BIOXASMAINM1MIRROR_H
