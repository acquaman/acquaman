#ifndef BIOXASMONOCHROMATOR_H
#define BIOXASMONOCHROMATOR_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASMonochromatorEnergyControl.h"

class BioXASMonochromator : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMonochromator(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMonochromator();

	/// Returns the bragg motor.
	virtual AMControl* bragg() const = 0;
	/// Returns the energy control.
	virtual BioXASMonochromatorEnergyControl* energy() const = 0;
};

#endif // BIOXASMONOCHROMATOR_H
