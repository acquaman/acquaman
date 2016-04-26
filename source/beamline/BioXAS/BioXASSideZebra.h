#ifndef BIOXASSIDEZEBRA_H
#define BIOXASSIDEZEBRA_H

#include "beamline/BioXAS/BioXASZebra.h"

class BioXASSideZebra : public BioXASZebra
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASSideZebra(const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideZebra();
};

#endif // BIOXASSIDEZEBRA_H
