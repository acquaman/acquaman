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

	/// Creates and returns a new action that calibrates the energy control s.t. the current energy is the given energy.
	virtual AMAction3* createSetEnergyAction(double newEnergy) = 0;

public slots:
	/// Calibrates the energy control s.t. the current energy is the given energy.
	virtual void setEnergy(double newEnergy) = 0;
};

#endif // BIOXASMONOCHROMATORENERGYCONTROL_H
