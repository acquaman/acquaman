#include "BioXASBeamStatus.h"

BioXASBeamStatus::BioXASBeamStatus(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize class variables.

	frontEndBeamStatus_ = new BioXASFrontEndBeamStatusControl(this);

}

BioXASBeamStatus::~BioXASBeamStatus()
{

}

bool BioXASBeamStatus::isConnected() const
{
	bool connected = (
				frontEndBeamStatus_ && frontEndBeamStatus_->isConnected()
				);

	return connected;
}
