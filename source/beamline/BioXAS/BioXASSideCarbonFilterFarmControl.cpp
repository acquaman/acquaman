#include "BioXASSideCarbonFilterFarmControl.h"

BioXASSideCarbonFilterFarmControl::BioXASSideCarbonFilterFarmControl(QObject *parent) :
    BioXASCarbonFilterFarmControl(parent)
{
	// Initialize inherited variables.

	upstreamPosition_ = new AMPVControl("CarbonFilterFarmUpstreamPosition", "SMTR1607-5-I00-01:mm:fbk", "SMTR1607-5-I00-01:mm:sp", "SMTR1607-5-I00-01:stop", this);
	upstreamStatus_ = new AMReadOnlyPVControl("CarbonFilterFarmUpstreamStatus", "SMTR1607-5-I00-01:inPosition", this);
	downstreamPosition_ = new AMPVControl("CarbonFilterFarmDownstreamPosition", "SMTR1607-5-I00-02:mm:fbk", "SMTR1607-5-I00-02:mm:sp", "SMTR1607-5-I00-02:stop", this);
	downstreamStatus_ = new AMPVControl("CarbonFilterFarmDownstreamStatus", "SMTR1607-5-I00-01:inPosition", this);

	upstreamActuator_ = new BioXASCarbonFilterFarmActuatorControl(upstreamPosition, upstreamStatus, this);

	addChildControl(upstreamActuator_);

	downstreamActuator_ = new BioXASCarbonFilterFarmActuatorControl(downstreamPosition, downstreamStatus, this);

	addChildControl(downstreamActuator_);



}

BioXASSideCarbonFilterFarmControl::~BioXASSideCarbonFilterFarmControl()
{

}

bool BioXASSideCarbonFilterFarmControl::validFilter(double value)
{
	bool result = false;

	switch ((int)value) {
	case Filter::Invalid:
		result = true;
		break;
	case Filter::None:
		result = true;
		break;
	case Filter::Fifty:
		result = true;
		break;
	case Filter::SevenHundred:
		result = true;
		break;
	case Filter::SevenHundredFifty:
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
	case Filter::None:
		result = true;
		break;
	case Filter::Fifty:
		result = true;
		break;
	case Filter::SevenHundred:
		result = true;
		break;
	case Filter::SevenHundredFifty:
		result = true;
		break;
	default:
		break;
	}

	return result;
}
