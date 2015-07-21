#include "BioXASSideCarbonFilterFarmControl.h"
#include "beamline/BioXAS/BioXASSideCarbonFilterFarm.h"

BioXASSideCarbonFilterFarmControl::BioXASSideCarbonFilterFarmControl(BioXASCarbonFilterFarmActuatorControl *upstreamActuatorControl, BioXASCarbonFilterFarmActuatorControl *downstreamActuatorControl, QObject *parent) :
	BioXASCarbonFilterFarmControl(upstreamActuatorControl, downstreamActuatorControl, parent)
{
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
