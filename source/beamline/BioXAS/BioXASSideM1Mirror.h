#ifndef BIOXASSIDEM1MIRROR_H
#define BIOXASSIDEM1MIRROR_H

#include "beamline/BioXAS/BioXASM1Mirror.h"

class BioXASSideM1Mirror : public BioXASM1Mirror
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideM1Mirror(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideM1Mirror();
};

#endif // BIOXASSIDEM1MIRROR_H
