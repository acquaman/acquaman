#include "BioXASSideSOEBeamStatus.h"

BioXASSideSOEBeamStatus::BioXASSideSOEBeamStatus(QObject *parent) :
	BioXASBeamlineBeamStatus("BioXASSideSOEBeamStatus", parent)
{
	// Initialize class variables.

	poeBeamStatus_ = 0;
	endstationShutter_ = 0;
}

BioXASSideSOEBeamStatus::~BioXASSideSOEBeamStatus()
{

}

void BioXASSideSOEBeamStatus::setPOEBeamStatus(BioXASSidePOEBeamStatus *newControl)
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

void BioXASSideSOEBeamStatus::setEndstationShutter(CLSBiStateControl *newControl)
{
	if (endstationShutter_ != newControl) {

		if (endstationShutter_)
			removeBiStateControl(endstationShutter_);

		endstationShutter_ = newControl;

		if (endstationShutter_)
			addBiStateControl(endstationShutter_, CLSBiStateControl::Open, CLSBiStateControl::Closed);

		emit endstationShutterChanged(endstationShutter_);
	}
}

void BioXASSideSOEBeamStatus::updateConnected()
{
	bool connected = (
				BioXASBeamlineBeamStatus::isConnected() &&
				poeBeamStatus_ && poeBeamStatus_->isConnected() &&
				endstationShutter_ && endstationShutter_->isConnected()
				);

	setConnected(connected);
}
