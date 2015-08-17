#include "BioXASMainCarbonFilterFarmControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"

BioXASMainCarbonFilterFarmControl::BioXASMainCarbonFilterFarmControl(BioXASCarbonFilterFarmActuatorControl *upstreamActuatorControl, BioXASCarbonFilterFarmActuatorControl *downstreamActuatorControl, QObject *parent) :
	BioXASCarbonFilterFarmControl(upstreamActuatorControl, downstreamActuatorControl, parent)
{
	// Set the appropriate enum and move enum states.

	setEnumStates(QStringList() << filterToString(BioXASCarbonFilterFarm::Filter::Invalid) << filterToString(BioXASCarbonFilterFarm::Filter::None) << filterToString(BioXASCarbonFilterFarm::Filter::Fifty) << filterToString(BioXASCarbonFilterFarm::Filter::SevenHundred) << filterToString(BioXASCarbonFilterFarm::Filter::SevenHundredFifty));
	setMoveEnumStates(QStringList() << filterToString(BioXASCarbonFilterFarm::Filter::None) << filterToString(BioXASCarbonFilterFarm::Filter::Fifty) << filterToString(BioXASCarbonFilterFarm::Filter::SevenHundred) << filterToString(BioXASCarbonFilterFarm::Filter::SevenHundredFifty));
}

BioXASMainCarbonFilterFarmControl::~BioXASMainCarbonFilterFarmControl()
{

}

bool BioXASMainCarbonFilterFarmControl::validFilter(double value)
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

bool BioXASMainCarbonFilterFarmControl::validFilterSetpoint(double value)
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
