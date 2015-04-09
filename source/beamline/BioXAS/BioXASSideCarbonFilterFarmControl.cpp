#include "BioXASSideCarbonFilterFarmControl.h"

BioXASSideCarbonFilterFarmControl::BioXASSideCarbonFilterFarmControl(QObject *parent) :
    BioXASCarbonFilterFarmControl(parent)
{
	// Initialize inherited variables.

	upstreamActuator_ = new BioXASCarbonFilterFarmActuatorControl("PFIL1607.5-I22-01", this);
	upstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::None, BIOXAS_FILTER_FARM_UPSTREAM_OUT);
	upstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::First, BIOXAS_FILTER_FARM_UPSTREAM_BOTTOM);
	upstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::Second, BIOXAS_FILTER_FARM_UPSTREAM_TOP);
	addChildControl(upstreamActuator_);

	downstreamActuator_ = new BioXASCarbonFilterFarmActuatorControl("PFIL1607.5-I22-02", this);
	downstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::None, BIOXAS_FILTER_FARM_DOWNSTREAM_OUT);
	downstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::First, BIOXAS_FILTER_FARM_DOWNSTREAM_BOTTOM);
	downstreamActuator_->setWindowPosition(BioXASCarbonFilterFarmActuatorControl::Window::Second, BIOXAS_FILTER_FARM_DOWNSTREAM_TOP);
	addChildControl(downstreamActuator_);

	setFilterMapping(Actuator::Upstream, BioXASCarbonFilterFarmActuatorControl::Window::First, Filter::Fifty);
	setFilterMapping(Actuator::Upstream, BioXASCarbonFilterFarmActuatorControl::Window::Second, Filter::Fifty);
	setFilterMapping(Actuator::Downstream, BioXASCarbonFilterFarmActuatorControl::Window::First, Filter::None);
	setFilterMapping(Actuator::Downstream, BioXASCarbonFilterFarmActuatorControl::Window::Second, Filter::SevenHundred);

	setEnumStates(QStringList() << filterToString(Filter::None) << filterToString(Filter::Between) << filterToString(Filter::Fifty) << filterToString(Filter::SevenHundred) << filterToString(Filter::SevenHundredFifty));
	setMoveEnumStates(QStringList() << filterToString(Filter::None) << filterToString(Filter::Fifty) << filterToString(Filter::SevenHundred) << filterToString(Filter::SevenHundredFifty));
}

BioXASSideCarbonFilterFarmControl::~BioXASSideCarbonFilterFarmControl()
{

}

bool BioXASSideCarbonFilterFarmControl::validFilter(double value)
{
	bool result = false;

	switch ((int)value) {
	case Filter::None:
		result = true;
		break;
	case Filter::Between:
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

AMAction3* BioXASCarbonFilterFarmControl::createWaitAction(double setpoint)
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Waiting for actuators to reach valid position", "Waiting for actuators to reach valid position"), AMListAction3::Parallel);

	switch ((int)setpoint) {
	case Filter::None:
		action->addSubAction(AMActionSupport::buildControlWaitAction(upstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Removed));
		action->addSubAction(AMActionSupport::buildControlWaitAction(downstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Removed));
		break;
	case Filter::Fifty:
		action->addSubAction(AMActionSupport::buildControlWaitAction(upstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::First));
		action->addSubAction(AMActionSupport::buildControlWaitAction(downstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Removed));
		break;
	case Filter::SevenHundred:
		action->addSubAction(AMActionSupport::buildControlWaitAction(upstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Removed));
		action->addSubAction(AMActionSupport::buildControlWaitAction(downstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Second));
		break;
	case Filter::SevenHundredFifty:
		action->addSubAction(AMActionSupport::buildControlWaitAction(upstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::First));
		action->addSubAction(AMActionSupport::buildControlWaitAction(downstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Second));
		break;
	default:
		action->deleteLater();
		action = 0;
		break;
	}

	return action;
}
