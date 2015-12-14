#include "BioXASBeamStatus.h"
#include "beamline/BioXAS/BioXASMasterValves.h"

BioXASBeamStatus::BioXASBeamStatus(const QString &name, QObject *parent) :
	BioXASBiStateGroup(name, "", parent)
{
	// Initialize class variables.

	valves_ = 0;

	// Setup the basic value options.

	addOption(On, "On");
	addOption(Off, "Off");
}

BioXASBeamStatus::~BioXASBeamStatus()
{

}

void BioXASBeamStatus::setValves(BioXASMasterValves *newValves)
{
	if (valves_ != newValves) {

		if (valves_)
			removeBiStateControl(valves_);

		valves_ = newValves;

		if (valves_)
			addBiStateControl(valves_, BioXASValves::Open, BioXASValves::None); // We don't want to close valves in order to turn off the beam.

		emit valvesChanged(valves_);
	}
}

int BioXASBeamStatus::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isOn())
		result = On;
	else if (isOff())
		result = Off;

	return result;
}
