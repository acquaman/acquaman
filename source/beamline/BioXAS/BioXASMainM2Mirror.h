#ifndef BIOXASMAINM2MIRROR_H
#define BIOXASMAINM2MIRROR_H

#include "beamline/BioXAS/BioXASM2Mirror.h"

class BioXASMainM2Mirror : public BioXASM2Mirror
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainM2Mirror(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainM2Mirror();
};

#endif // BIOXASMAINM2MIRROR_H
