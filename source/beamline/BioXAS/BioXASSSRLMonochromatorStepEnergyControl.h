#ifndef BIOXASSSRLMONOCHROMATORSTEPENERGYCONTROL_H
#define BIOXASSSRLMONOCHROMATORSTEPENERGYCONTROL_H

#include "beamline/BioXAS/BioXASSSRLMonochromatorEnergyControl.h"

class BioXASSSRLMonochromatorStepEnergyControl : public BioXASSSRLMonochromatorEnergyControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorStepEnergyControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorStepEnergyControl();

protected slots:
	/// Updates the current value.
	void updateValue();
};

#endif // BIOXASSSRLMONOCHROMATORSTEPENERGYCONTROL_H
