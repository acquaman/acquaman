#ifndef BIOXASMAINCRYOSTATSTAGE_H
#define BIOXASMAINCRYOSTATSTAGE_H

#include "beamline/BioXAS/BioXASCryostatStage.h"

class BioXASMainCryostatStage : public BioXASCryostatStage
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainCryostatStage(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainCryostatStage();
};

#endif // BIOXASMAINCRYOSTATSTAGE_H
