#include "BioXASSSRLMonochromatorMask.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"

BioXASSSRLMonochromatorMask::BioXASSSRLMonochromatorMask(const QString &deviceName, QObject *parent) :
	BioXASBeamlineComponent(deviceName, parent)
{
	// Initialize class variables.



	// Current settings.

	setState(new BioXASSSRLMonochromatorMaskState(name()+"State", this));
}

BioXASSSRLMonochromatorMask::~BioXASSSRLMonochromatorMask()
{

}

bool BioXASSSRLMonochromatorMask::isConnected() const
{
	bool connected = (
				upperBlade_ && upperBlade_->isConnected() &&
				lowerBlade_ && lowerBlade_->isConnected() &&
				bladesState_ && bladesState_->isConnected() &&
				state_ && state_->isConnected()
				);

	return connected;
}






