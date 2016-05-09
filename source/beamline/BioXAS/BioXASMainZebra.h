#ifndef BIOXASMAINZEBRA_H
#define BIOXASMAINZEBRA_H

#include "beamline/BioXAS/BioXASZebra.h"

class BioXASMainZebra : public BioXASZebra
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASMainZebra(const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainZebra();
};

#endif // BIOXASMAINZEBRA_H
