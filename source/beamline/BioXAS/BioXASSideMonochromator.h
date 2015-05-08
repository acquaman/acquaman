#ifndef BIOXASSIDEMONOCHROMATOR_H
#define BIOXASSIDEMONOCHROMATOR_H

#include <QObject>

#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "beamline/BioXAS/BioXASSideMonochromatorEnergyControl.h"

class BioXASSideMonochromator : public BioXASSSRLMonochromator
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideMonochromator(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideMonochromator();
};

#endif // BIOXASSIDEMONOCHROMATOR_H
