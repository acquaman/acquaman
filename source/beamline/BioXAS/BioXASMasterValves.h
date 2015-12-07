#ifndef BIOXASMASTERVALVES_H
#define BIOXASMASTERVALVES_H

#include "beamline/BioXAS/BioXASValves.h"

class BioXASMasterValves : public BioXASValves
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASMasterValves(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMasterValves();

};

#endif // BIOXASMASTERVALVES_H
