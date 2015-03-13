#ifndef BIOXASMAINMONOCHROMATORREGIONCONTROL_H
#define BIOXASMAINMONOCHROMATORREGIONCONTROL_H

#include <QObject>

#include "beamline/AMPVControl.h"

#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorRegionControl.h"

class BioXASMainMonochromatorRegionControl : public BioXASSSRLMonochromatorRegionControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainMonochromatorRegionControl(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainMonochromatorRegionControl();
};

#endif // BIOXASMAINMONOCHROMATORREGIONCONTROL_H
