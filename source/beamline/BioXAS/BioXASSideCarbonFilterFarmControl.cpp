#include "BioXASSideCarbonFilterFarmControl.h"

BioXASSideCarbonFilterFarmControl::BioXASSideCarbonFilterFarmControl(QObject *parent) :
    BioXASCarbonFilterFarmControl(parent)
{
	// Initialize inherited variables.

	upstreamActuator_ = new BioXASCarbonFilterFarmActuatorControl("SMTR1607-5-I00-01", this);
	upstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::Removed, BIOXAS_FILTER_FARM_UPSTREAM_OUT);
	upstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::First, BIOXAS_FILTER_FARM_UPSTREAM_BOTTOM);
	upstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::Second, BIOXAS_FILTER_FARM_UPSTREAM_TOP);
	addChildControl(upstreamActuator_);

	downstreamActuator_ = new BioXASCarbonFilterFarmActuatorControl("SMTR1607-5-I00-02", this);
	downstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::Removed, BIOXAS_FILTER_FARM_DOWNSTREAM_OUT);
	downstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::First, BIOXAS_FILTER_FARM_DOWNSTREAM_BOTTOM);
	downstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::Second, BIOXAS_FILTER_FARM_DOWNSTREAM_TOP);
	addChildControl(downstreamActuator_);

	setWindowFilter(Actuator::Upstream, BioXASCarbonFilterFarmActuatorControl::Window::First, Filter::Fifty);
	setWindowFilter(Actuator::Upstream, BioXASCarbonFilterFarmActuatorControl::Window::Second, Filter::Fifty);
	setWindowFilter(Actuator::Downstream, BioXASCarbonFilterFarmActuatorControl::Window::First, Filter::None);
	setWindowFilter(Actuator::Downstream, BioXASCarbonFilterFarmActuatorControl::Window::Second, Filter::SevenHundred);

	setEnumStates(QStringList() << filterToString(Filter::None) << filterToString(Filter::Fifty) << filterToString(Filter::SevenHundred) << filterToString(Filter::SevenHundredFifty));
	setMoveEnumStates(QStringList() << filterToString(Filter::None) << filterToString(Filter::Fifty) << filterToString(Filter::SevenHundred) << filterToString(Filter::SevenHundredFifty));
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

AMAction3* BioXASSideCarbonFilterFarmControl::createMoveAction(double setpoint)
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Moving filter farm actuators", "Moving filter farm actuators"), AMListAction3::Parallel);

	switch ((int)setpoint) {
	case Filter::None:
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Removed));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Removed));
		break;
	case Filter::Fifty:
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::First));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Removed));
		break;
	case Filter::SevenHundred:
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Removed));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Second));
		break;
	case Filter::SevenHundredFifty:
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::First));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Second));
		break;
	default:
		action->deleteLater();
		action = 0;
		break;
	}

	return action;
}
