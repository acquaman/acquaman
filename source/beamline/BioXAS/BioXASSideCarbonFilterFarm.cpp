#include "BioXASSideCarbonFilterFarm.h"

BioXASSideCarbonFilterFarm::BioXASSideCarbonFilterFarm(QObject *parent) :
	BioXASCarbonFilterFarm("BioXASSideCarbonFilterFarm", "SMTR1607-5-I00-01", "SMTR1607-5-I00-02", parent)
{
	upstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::None, BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT);
	upstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::Bottom, BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM);
	upstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::Top, BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP);

	downstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::None, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT);
	downstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::Bottom, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM);
	downstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::Top, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP);

	setWindowFilter(Actuator::Upstream, BioXASCarbonFilterFarmActuatorControl::Window::Bottom, Filter::Fifty);
	setWindowFilter(Actuator::Upstream, BioXASCarbonFilterFarmActuatorControl::Window::Top, Filter::Fifty);
	setWindowFilter(Actuator::Downstream, BioXASCarbonFilterFarmActuatorControl::Window::Bottom, Filter::None);
	setWindowFilter(Actuator::Downstream, BioXASCarbonFilterFarmActuatorControl::Window::Top, Filter::SevenHundred);

	setEnumStates(QStringList() << filterToString(Filter::Invalid) << filterToString(Filter::None) << filterToString(Filter::Fifty) << filterToString(Filter::SevenHundred) << filterToString(Filter::SevenHundredFifty));
	setMoveEnumStates(QStringList() << filterToString(Filter::None) << filterToString(Filter::Fifty) << filterToString(Filter::SevenHundred) << filterToString(Filter::SevenHundredFifty));
}
