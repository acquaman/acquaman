#include "BioXASSideCarbonFilterFarmControl.h"
#include "beamline/BioXAS/BioXASSideCarbonFilterFarm.h"

BioXASSideCarbonFilterFarmControl::BioXASSideCarbonFilterFarmControl(BioXASCarbonFilterFarmActuatorControl *upstreamActuatorControl, BioXASCarbonFilterFarmActuatorControl *downstreamActuatorControl, QObject *parent) :
	BioXASCarbonFilterFarmControl(upstreamActuatorControl, downstreamActuatorControl, parent)
{
	// Make connections.
	// Emit enumChanged signals when connected, value changes, and setpoint changes. This is to make sure the control is viewed as an enum.

	connect( this, SIGNAL(connected(bool)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(valueChanged(double)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(setpointChanged(double)), this, SIGNAL(enumChanged()) );

	// Set the appropriate enum and move enum states.

	setEnumStates(QStringList() << filterToString(BioXASCarbonFilterFarm::Filter::Invalid) << filterToString(BioXASCarbonFilterFarm::Filter::None) << filterToString(BioXASCarbonFilterFarm::Filter::Fifty) << filterToString(BioXASCarbonFilterFarm::Filter::SevenHundred) << filterToString(BioXASCarbonFilterFarm::Filter::SevenHundredFifty));
	setMoveEnumStates(QStringList() << filterToString(BioXASCarbonFilterFarm::Filter::None) << filterToString(BioXASCarbonFilterFarm::Filter::Fifty) << filterToString(BioXASCarbonFilterFarm::Filter::SevenHundred) << filterToString(BioXASCarbonFilterFarm::Filter::SevenHundredFifty));
}

BioXASSideCarbonFilterFarmControl::~BioXASSideCarbonFilterFarmControl()
{

}

bool BioXASSideCarbonFilterFarmControl::validFilter(double value)
{
	bool result = false;

	switch ((int)value) {
	case BioXASCarbonFilterFarm::Filter::Invalid:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Filter::None:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Filter::Fifty:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Filter::SevenHundred:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Filter::SevenHundredFifty:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

bool BioXASSideCarbonFilterFarmControl::validFilterSetpoint(double value)
{
	bool result = false;

	switch ((int)value) {
	case BioXASCarbonFilterFarm::Filter::None:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Filter::Fifty:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Filter::SevenHundred:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Filter::SevenHundredFifty:
		result = true;
		break;
	default:
		break;
	}

	return result;
}
