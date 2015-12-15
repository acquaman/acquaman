#include "BioXASSideBeamStatus.h"

BioXASSideBeamStatus::BioXASSideBeamStatus(QObject *parent) :
	BioXASBeamlineBeamStatus("BioXASSideBeamStatus", parent)
{
	// Initialize class variables.

	poeBeamStatus_ = 0;
	soeBeamStatus_ = 0;
}

BioXASSideBeamStatus::~BioXASSideBeamStatus()
{

}

void BioXASSideBeamStatus::setPOEBeamStatus(BioXASSidePOEBeamStatus *newControl)
{
	if (poeBeamStatus_ != newControl) {

		if (poeBeamStatus_)
			removeBiStateControl(poeBeamStatus_);

		poeBeamStatus_ = newControl;

		if (poeBeamStatus_)
			addBiStateControl(poeBeamStatus_, BioXASBeamStatus::On, BioXASBeamStatus::Off);

		emit poeBeamStatusChanged(poeBeamStatus_);
	}
}

void BioXASSideBeamStatus::setSOEBeamStatus(BioXASSideSOEBeamStatus *newControl)
{
	if (soeBeamStatus_ != newControl) {

		if (soeBeamStatus_)
			removeBiStateControl(soeBeamStatus_);

		soeBeamStatus_ = newControl;

		if (soeBeamStatus_)
			addBiStateControl(soeBeamStatus_, BioXASBeamStatus::On, BioXASBeamStatus::Off);

		emit soeBeamStatusChanged(soeBeamStatus_);
	}
}

void BioXASSideBeamStatus::updateConnected()
{
	bool connected = (
				BioXASBeamlineBeamStatus::isConnected() &&
				poeBeamStatus_ && poeBeamStatus_->isConnected() &&
				soeBeamStatus_ && soeBeamStatus_->isConnected()
				);

	setConnected(connected);
}


