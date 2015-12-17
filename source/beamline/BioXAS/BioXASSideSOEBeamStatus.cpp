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
