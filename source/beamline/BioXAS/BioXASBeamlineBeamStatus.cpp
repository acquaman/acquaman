#include "BioXASBeamlineBeamStatus.h"

BioXASBeamlineBeamStatus::BioXASBeamlineBeamStatus(const QString &name, QObject *parent) :
	BioXASBeamStatus(name, parent)
{
	// Initialize class variables.

	frontEndStatus_ = 0;
}

BioXASBeamlineBeamStatus::~BioXASBeamlineBeamStatus()
{

}

void BioXASBeamlineBeamStatus::setFrontEndBeamStatus(BioXASFrontEndBeamStatus *newControl)
{
	if (frontEndStatus_ != newControl) {

		if (frontEndStatus_)
			removeBiStateControl(frontEndStatus_);

		frontEndStatus_ = newControl;

		if (frontEndStatus_)
			addBiStateControl(frontEndStatus_, BioXASBeamStatus::On, BioXASBeamStatus::Off);

		emit frontEndBeamStatusChanged(frontEndStatus_);
	}
}

void BioXASBeamlineBeamStatus::updateConnected()
{
	bool connected = (
				frontEndStatus_ && frontEndStatus_->isConnected()
				);

	setConnected(connected);
}
