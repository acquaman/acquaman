#ifndef BIOXASSIDECRYOSTAT_H
#define BIOXASSIDECRYOSTAT_H

#include <QObject>

#include "beamline/BioXAS/BioXASCryostat.h"

class BioXASSideCryostat : public BioXASCryostat
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASSideCryostat(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideCryostat();
};

#endif // BIOXASSIDECRYOSTAT_H
