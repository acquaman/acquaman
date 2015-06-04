#include "BioXASCarbonFilterFarmControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h"

BioXASCarbonFilterFarmControl::BioXASCarbonFilterFarmControl(BioXASCarbonFilterFarmActuatorControl *upstreamActuatorControl, BioXASCarbonFilterFarmActuatorControl *downstreamActuatorControl, QObject *parent) :
	AMPseudoMotorControl("BioXAS Carbon Filter Farm", "mm", parent)
{
	// Initialize local variables.

	upstreamActuator_ = 0;
	downstreamActuator_ = 0;

	// Initialize inherited variables.
	// Subclasses should initialize their own enum states and enum move states, as these will vary between beamlines.

	value_ = BioXASCarbonFilterFarm::Filter::None;
	setpoint_ = BioXASCarbonFilterFarm::Filter::None;
	minimumValue_ = BioXASCarbonFilterFarm::Filter::None;
	maximumValue_ = BioXASCarbonFilterFarm::Filter::SevenHundredFifty;

	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Filter Farm");

	// Current settings.

	setUpstreamActuatorControl(upstreamActuatorControl);
	setDownstreamActuatorControl(downstreamActuatorControl);

	updateStates();
}

BioXASCarbonFilterFarmControl::~BioXASCarbonFilterFarmControl()
{

}

bool BioXASCarbonFilterFarmControl::canMeasure() const
{
	bool result = false;

	if (isConnected()) {
		result = (upstreamActuator_->canMeasure() && downstreamActuator_->canMeasure());
	}

	return result;
}

bool BioXASCarbonFilterFarmControl::canMove() const
{
	bool result = false;

	if (isConnected()) {
		result = (upstreamActuator_->canMove() && downstreamActuator_->canMove());
	}

	return result;
}

bool BioXASCarbonFilterFarmControl::canStop() const
{
	bool result = false;

	if (isConnected()) {
		result = (upstreamActuator_->canStop() && downstreamActuator_->canStop());
	}

	return result;
}

bool BioXASCarbonFilterFarmControl::validValue(double value) const
{
	bool result = false;

	switch ((int)value) {
	case BioXASCarbonFilterFarm::Filter::None:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Filter::Fifty:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Filter::SeventyFive:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Filter::FiveHundred:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Filter::FiveHundredSeventyFive:
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

bool BioXASCarbonFilterFarmControl::validSetpoint(double value) const
{
	bool result = false;

	switch ((int)value) {
	case BioXASCarbonFilterFarm::Filter::Fifty:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Filter::SeventyFive:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Filter::FiveHundred:
		result = true;
		break;
	case BioXASCarbonFilterFarm::Filter::FiveHundredSeventyFive:
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

QString BioXASCarbonFilterFarmControl::filterToString(double value)
{
	QString result = QString();

	switch ((int)value) {
	case BioXASCarbonFilterFarm::Filter::Invalid:
		result = "Invalid";
		break;
	case BioXASCarbonFilterFarm::Filter::None:
		result = "None";
		break;
	case BioXASCarbonFilterFarm::Filter::Fifty:
		result = "50";
		break;
	case BioXASCarbonFilterFarm::Filter::SeventyFive:
		result = "75";
		break;
	case BioXASCarbonFilterFarm::Filter::FiveHundred:
		result = "500";
		break;
	case BioXASCarbonFilterFarm::Filter::FiveHundredSeventyFive:
		result = "575";
		break;
	case BioXASCarbonFilterFarm::Filter::SevenHundred:
		result = "700";
		break;
	case BioXASCarbonFilterFarm::Filter::SevenHundredFifty:
		result = "750";
		break;
	default:
		break;
	}

	return result;
}

double BioXASCarbonFilterFarmControl::stringToFilter(const QString &string)
{
	BioXASCarbonFilterFarm::Filter::Thickness result = BioXASCarbonFilterFarm::Filter::Invalid;

	if (!string.isEmpty()) {
		if (string == "None")
			result= BioXASCarbonFilterFarm::Filter::None;
		else if (string == "50")
			result = BioXASCarbonFilterFarm::Filter::Fifty;
		else if (string == "75")
			result = BioXASCarbonFilterFarm::Filter::SeventyFive;
		else if (string == "500")
			result = BioXASCarbonFilterFarm::Filter::FiveHundred;
		else if (string == "575")
			result = BioXASCarbonFilterFarm::Filter::FiveHundredSeventyFive;
		else if (string == "700")
			result = BioXASCarbonFilterFarm::Filter::SevenHundred;
		else if (string == "750")
			result = BioXASCarbonFilterFarm::Filter::SevenHundredFifty;
	}

	return result;
}

double BioXASCarbonFilterFarmControl::filterToDouble(double filter)
{
	double result = -1;

	switch ((int)filter) {
	case BioXASCarbonFilterFarm::Filter::None:
		result = 0;
		break;
	case BioXASCarbonFilterFarm::Filter::Fifty:
		result = 50;
		break;
	case BioXASCarbonFilterFarm::Filter::SeventyFive:
		result = 75;
		break;
	case BioXASCarbonFilterFarm::Filter::FiveHundred:
		result = 500;
		break;
	case BioXASCarbonFilterFarm::Filter::FiveHundredSeventyFive:
		result = 575;
		break;
	case BioXASCarbonFilterFarm::Filter::SevenHundred:
		result = 700;
		break;
	case BioXASCarbonFilterFarm::Filter::SevenHundredFifty:
		result = 750;
		break;
	default:
		break;
	}

	return result;
}

double BioXASCarbonFilterFarmControl::doubleToFilter(double value)
{
	BioXASCarbonFilterFarm::Filter::Thickness filter = BioXASCarbonFilterFarm::Filter::Invalid;

	switch ((int)value) {
	case 50:
		filter = BioXASCarbonFilterFarm::Filter::Fifty;
		break;
	case 75:
		filter = BioXASCarbonFilterFarm::Filter::SeventyFive;
		break;
	case 500:
		filter = BioXASCarbonFilterFarm::Filter::FiveHundred;
		break;
	case 575:
		filter = BioXASCarbonFilterFarm::Filter::FiveHundredSeventyFive;
		break;
	case 700:
		filter = BioXASCarbonFilterFarm::Filter::SevenHundred;
		break;
	case 750:
		filter = BioXASCarbonFilterFarm::Filter::SevenHundredFifty;
		break;
	default:
		break;
	}

	return filter;
}

double BioXASCarbonFilterFarmControl::filterAtWindow(double actuator, double window)
{
	double filter = BioXASCarbonFilterFarm::Filter::Invalid;

	if ((int)actuator == BioXASCarbonFilterFarm::Actuator::Upstream) {
		filter = upstreamFilterMap_.value(window, BioXASCarbonFilterFarm::Filter::Invalid);

	} else if ((int)actuator == BioXASCarbonFilterFarm::Actuator::Downstream) {
		filter = downstreamFilterMap_.value(window, BioXASCarbonFilterFarm::Filter::Invalid);
	}

	return filter;
}

void BioXASCarbonFilterFarmControl::setUpstreamActuatorControl(BioXASCarbonFilterFarmActuatorControl *newControl)
{
	if (upstreamActuator_ != newControl) {

		if (upstreamActuator_)
			removeChildControl(upstreamActuator_);

		upstreamActuator_ = newControl;

		if (upstreamActuator_)
			addChildControl(upstreamActuator_);
	}
}

void BioXASCarbonFilterFarmControl::setDownstreamActuatorControl(BioXASCarbonFilterFarmActuatorControl *newControl)
{
	if (downstreamActuator_ != newControl) {

		if (downstreamActuator_)
			removeChildControl(downstreamActuator_);

		downstreamActuator_ = newControl;

		if (downstreamActuator_)
			addChildControl(downstreamActuator_);
	}
}

void BioXASCarbonFilterFarmControl::setWindowFilter(double actuator, double window, double filterThickness)
{
	if ((int)actuator == BioXASCarbonFilterFarm::Actuator::Upstream && upstreamActuator_ && upstreamActuator_->validValue(window)) {
		upstreamFilterMap_.insert(window, filterThickness);

	} else if ((int)actuator == BioXASCarbonFilterFarm::Actuator::Downstream && downstreamActuator_ && downstreamActuator_->validValue(window)) {
		downstreamFilterMap_.insert(window, filterThickness);
	}
}

void BioXASCarbonFilterFarmControl::updateConnected()
{
	bool isConnected = (
				upstreamActuator_ && upstreamActuator_->isConnected() &&
				downstreamActuator_ && downstreamActuator_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASCarbonFilterFarmControl::updateValue()
{
	double newFilter = BioXASCarbonFilterFarm::Filter::Invalid;

	if (isConnected()) {
		double upstreamWindow = upstreamActuator_->value();
		double downstreamWindow = downstreamActuator_->value();

		newFilter = calculateTotalFilterFromWindows(upstreamWindow, downstreamWindow);
	}

	setValue(newFilter);
}

void BioXASCarbonFilterFarmControl::updateMoving()
{
	if (isConnected()) {
		setIsMoving( upstreamActuator_->isMoving() || downstreamActuator_->isMoving() );
	}
}

AMAction3* BioXASCarbonFilterFarmControl::createMoveAction(double setpoint)
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Moving filter farm actuators", "Moving filter farm actuators"), AMListAction3::Parallel);

	switch ((int)setpoint) {

	case BioXASCarbonFilterFarm::Filter::None: // Main, Side, Imaging
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarm::Window::None));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarm::Window::None));
		break;

	case BioXASCarbonFilterFarm::Filter::Fifty: // Main, Side
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarm::Window::Bottom));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarm::Window::None));
		break;

	case BioXASCarbonFilterFarm::Filter::SeventyFive: // Imaging
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarm::Window::Bottom));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarm::Window::None));
		break;

	case BioXASCarbonFilterFarm::Filter::FiveHundred: // Imaging
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarm::Window::None));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarm::Window::Bottom));
		break;

	case BioXASCarbonFilterFarm::Filter::FiveHundredSeventyFive: // Imaging
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarm::Window::Bottom));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarm::Window::Bottom));
		break;

	case BioXASCarbonFilterFarm::Filter::SevenHundred: // Main, Side
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarm::Window::None));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarm::Window::Bottom));
		break;

	case BioXASCarbonFilterFarm::Filter::SevenHundredFifty: // Main, Side
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarm::Window::Bottom));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarm::Window::Bottom));
		break;
	default:
		action->deleteLater();
		action = 0;
		break;
	}

	return action;
}

double BioXASCarbonFilterFarmControl::calculateTotalFilterFromWindows(double upstreamWindow, double downstreamWindow)
{
	double upstreamThickness = BioXASCarbonFilterFarm::Filter::Invalid;
	double downstreamThickness = BioXASCarbonFilterFarm::Filter::Invalid;

	if (upstreamActuator_ && upstreamActuator_->validValue(upstreamWindow) && upstreamFilterMap_.contains(upstreamWindow)) {
		upstreamThickness = filterAtWindow(BioXASCarbonFilterFarm::Actuator::Upstream, upstreamWindow);
	}

	if (downstreamActuator_ && downstreamActuator_->validValue(downstreamWindow) && downstreamFilterMap_.contains(downstreamWindow)) {
		downstreamThickness = filterAtWindow(BioXASCarbonFilterFarm::Actuator::Downstream, downstreamWindow);
	}

	return calculateTotalFilter(upstreamThickness, downstreamThickness);
}

double BioXASCarbonFilterFarmControl::calculateTotalFilter(double upstreamFilter, double downstreamFilter)
{
	return ( filterToDouble(upstreamFilter) + filterToDouble(downstreamFilter) );
}

