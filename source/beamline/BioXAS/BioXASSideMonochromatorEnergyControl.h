#ifndef BIOXASSIDEMONOCHROMATORENERGYCONTROL_H
#define BIOXASSIDEMONOCHROMATORENERGYCONTROL_H

#include "beamline/BioXAS/BioXASSSRLMonochromatorEnergyControl.h"

class BioXASSideMonochromatorEnergyControl : public BioXASSSRLMonochromatorEnergyControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideMonochromatorEnergyControl(QObject* parent = 0);
	/// Destructor.
	virtual ~BioXASSideMonochromatorEnergyControl();

};

#endif // BIOXASSIDEMONOCHROMATORENERGYCONTROL_H
