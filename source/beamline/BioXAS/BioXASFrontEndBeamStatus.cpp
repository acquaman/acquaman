#include "BioXASFrontEndBeamStatus.h"
#include "beamline/BioXAS/BioXASFrontEndShutters.h"
#include "beamline/BioXAS/BioXASMasterValves.h"

BioXASFrontEndBeamStatus::BioXASFrontEndBeamStatus(const QString &name, QObject *parent) :
	BioXASBeamStatus(name, parent)
{
	// Initialize class variables.

	shutters_ = 0;
	valves_ = 0;
}

BioXASFrontEndBeamStatus::~BioXASFrontEndBeamStatus()
{

}

bool BioXASFrontEndBeamStatus::isConnected() const
{
	bool connected = (
				shutters_ && shutters_->isConnected() &&
				valves_ && valves_->isConnected()
				);

	return connected;
}

void BioXASFrontEndBeamStatus::setShutters(BioXASFrontEndShutters *newShutters)
{
	if (shutters_ != newShutters) {

		if (shutters_)
			removeBiStateControl(shutters_);

		shutters_ = newShutters;

		if (shutters_)
			addBiStateControl(shutters_, BioXASShutters::Open, BioXASShutters::Closed);

		emit shuttersChanged(shutters_);
	}
}

void BioXASFrontEndBeamStatus::setValves(BioXASMasterValves *newValves)
{
	if (valves_ != newValves) {

		if (valves_)
			removeBiStateControl(valves_);

		valves_ = newValves;

		if (valves_)
			addBiStateControl(valves_, BioXASValves::Open, BioXASValves::Closed);

		emit valvesChanged(valves_);
	}
}

