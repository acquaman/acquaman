#include "BioXASSideBeamStatus.h"
#include "beamline/BioXAS/BioXASFrontEndValves.h"
#include "beamline/CLS/CLSBiStateControl.h"

BioXASSideBeamStatus::BioXASSideBeamStatus(BioXASFrontEndShuttersControl *shutters, BioXASValvesControl *valves, AMControl *mirrorMask, BioXASSSRLMonochromatorMaskControl *monoMask, CLSBiStateControl *endstationShutter, QObject *parent) :
	BioXASBeamStatus("BioXASSideBeamStatus", parent)
{
	// Create and set beam status controls.

	setFrontEndBeamStatus(new BioXASFrontEndBeamStatusControl("BioXASFrontEndBeamStatus", this));
	setPOEBeamStatus(new BioXASSidePOEBeamStatusControl("BioXASSidePOEBeamStatus", this));
	setSOEBeamStatus(new BioXASSideSOEBeamStatusControl("BioXASSideSOEBeamStatus", this));

	// Make connections.

	connect( frontEndStatus_, SIGNAL(shuttersChanged(AMControl*)), this, SIGNAL(shuttersChanged(AMControl*)) );
	connect( frontEndStatus_, SIGNAL(valvesChanged(AMControl*)), this, SIGNAL(valvesChanged(AMControl*)) );
	connect( poeBeamStatus_, SIGNAL(mirrorMaskChanged(AMControl*)), this, SIGNAL(mirrorMaskChanged(AMControl*)) );
	connect( poeBeamStatus_, SIGNAL(monoMaskChanged(AMControl*)), this, SIGNAL(monoMaskChanged(AMControl*)) );
	connect( soeBeamStatus_, SIGNAL(endstationShutterChanged(AMControl*)), this, SIGNAL(endstationShutterChanged(AMControl*)) );

	// Set beam status subcontrols.

	setShutters(shutters);
	setValves(valves);
	setMirrorMask(mirrorMask);
	setMonoMask(monoMask);
	setEndstationShutter(endstationShutter);
}

BioXASSideBeamStatus::~BioXASSideBeamStatus()
{

}

void BioXASSideBeamStatus::setShutters(BioXASFrontEndShuttersControl *newControl)
{
	frontEndStatus_->setShutters(newControl);
}

void BioXASSideBeamStatus::setValves(BioXASValvesControl *newControl)
{
	frontEndStatus_->setValves(newControl);
}

void BioXASSideBeamStatus::setMirrorMask(AMControl *newControl)
{
	poeBeamStatus_->setMirrorMask(newControl);
}

void BioXASSideBeamStatus::setMonoMask(BioXASSSRLMonochromatorMaskControl *newControl)
{
	poeBeamStatus_->setMonoMask(newControl);
}

void BioXASSideBeamStatus::setEndstationShutter(CLSBiStateControl *newControl)
{
	soeBeamStatus_->setEndstationShutter(newControl);
}
