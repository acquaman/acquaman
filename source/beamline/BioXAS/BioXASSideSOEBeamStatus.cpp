#include "BioXASSideSOEBeamStatus.h"
#include "beamline/CLS/CLSBiStateControl.h"

BioXASSideSOEBeamStatus::BioXASSideSOEBeamStatus(const QString &name, QObject *parent) :
	BioXASSidePOEBeamStatus(name, parent)
{
	// Initialize class variables.

	endstationShutter_ = 0;
}

BioXASSideSOEBeamStatus::~BioXASSideSOEBeamStatus()
{

}

bool BioXASSideSOEBeamStatus::isConnected() const
{
	bool connected = (
				BioXASSidePOEBeamStatus::isConnected() &&
				endstationShutter_ && endstationShutter_->isConnected()
				);

	return connected;
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
