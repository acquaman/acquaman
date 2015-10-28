#include "BioXASBeamStatus.h"

BioXASBeamStatus::BioXASBeamStatus(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize class variables.

	frontEndBeamStatus_ = new BioXASFrontEndBeamStatusControl(this);

	sideBeamStatus_ = new BioXASSideBeamStatusControl(this);
	sideBeamStatus_->setFrontEndBeamStatusControl(frontEndBeamStatus_);

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
