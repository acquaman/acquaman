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

protected slots:
	/// Adds valves to the valve set.
	void addValves(BioXASValves *newValve);
	/// Removes valves from the valve set.
	void removeValves(BioXASValves *valves);
};

#endif // BIOXASMASTERVALVES_H
