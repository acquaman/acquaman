#ifndef BIOXASSIDEMONOCHROMATORREGIONCONTROL_H
#define BIOXASSIDEMONOCHROMATORREGIONCONTROL_H

#include <QObject>

#include "beamline/AMPVControl.h"

#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorRegionControl.h"

class BioXASSideMonochromatorRegionControl : public QObject
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideMonochromatorRegionControl(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideMonochromatorRegionControl();
};

#endif // BIOXASSIDEMONOCHROMATORREGIONCONTROL_H
