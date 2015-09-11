#ifndef BIOXASMONOCHROMATORENERGYCONTROL_H
#define BIOXASMONOCHROMATORENERGYCONTROL_H

#include "beamline/AMPseudoMotorControl.h"

class BioXASMonochromatorEnergyControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMonochromatorEnergyControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMonochromatorEnergyControl();
};

#endif // BIOXASMONOCHROMATORENERGYCONTROL_H
