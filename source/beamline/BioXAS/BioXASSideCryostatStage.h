#ifndef BIOXASSIDECRYOSTATSTAGE_H
#define BIOXASSIDECRYOSTATSTAGE_H

#include "beamline/BioXAS/BioXASCryostatStage.h"

class BioXASSideCryostatStage : public BioXASCryostatStage
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideCryostatStage(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideCryostatStage();
};

#endif // BIOXASSIDECRYOSTATSTAGE_H
