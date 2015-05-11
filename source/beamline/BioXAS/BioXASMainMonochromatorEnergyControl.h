#ifndef BIOXASMAINMONOCHROMATORENERGYCONTROL_H
#define BIOXASMAINMONOCHROMATORENERGYCONTROL_H

#include "beamline/BioXAS/BioXASSSRLMonochromatorEnergyControl.h"

class BioXASMainMonochromatorEnergyControl : public BioXASSSRLMonochromatorEnergyControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainMonochromatorEnergyControl(QObject* parent = 0);
	/// Destructor.
	virtual ~BioXASMainMonochromatorEnergyControl();
};

#endif // BIOXASMAINMONOCHROMATORENERGYCONTROL_H
