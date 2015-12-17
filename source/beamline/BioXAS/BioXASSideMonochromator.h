#ifndef BIOXASSIDEMONOCHROMATOR_H
#define BIOXASSIDEMONOCHROMATOR_H

#include "beamline/BioXAS/BioXASSSRLMonochromator.h"

class BioXASSideMonochromator : public BioXASSSRLMonochromator
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideMonochromator(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideMonochromator();
};

#endif // BIOXASSIDEMONOCHROMATOR_H
