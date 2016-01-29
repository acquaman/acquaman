#ifndef BIOXASSIDEM2MIRROR_H
#define BIOXASSIDEM2MIRROR_H

#include "beamline/BioXAS/BioXASM2Mirror.h"

class BioXASSideM2Mirror : public BioXASM2Mirror
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideM2Mirror(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideM2Mirror();

};

#endif // BIOXASSIDEM2MIRROR_H
