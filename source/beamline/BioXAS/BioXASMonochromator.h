#ifndef BIOXASMONOCHROMATOR_H
#define BIOXASMONOCHROMATOR_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/AMControl.h"
#include "actions3/AMAction3.h"

class BioXASMonochromator : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMonochromator(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMonochromator();

	/// Returns the energy control.
	virtual AMControl* energyControl() const = 0;
};

#endif // BIOXASMONOCHROMATOR_H
