#include "BioXASSSRLMonochromatorMask.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"

BioXASSSRLMonochromatorMask::BioXASSSRLMonochromatorMask(const QString &deviceName, QObject *parent) :
	BioXASBeamlineComponent(deviceName, parent)
{
	// Initialize class variables.



	// Current settings.

}

BioXASSSRLMonochromatorMask::~BioXASSSRLMonochromatorMask()
{

}

bool BioXASSSRLMonochromatorMask::isConnected() const
{
	return false;
}
