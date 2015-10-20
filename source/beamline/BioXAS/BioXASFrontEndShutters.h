#ifndef BIOXASFRONTENDSHUTTERS_H
#define BIOXASFRONTENDSHUTTERS_H

#include "beamline/BioXAS/BioXASFrontEndShuttersControl.h"

class BioXASFrontEndShutters : public BioXASFrontEndShuttersControl
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASFrontEndShutters(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndShutters();
};

#endif // BIOXASFRONTENDSHUTTERS_H
