#include "BioXASCarbonFilterFarmControl.h"

BioXASCarbonFilterFarmControl::BioXASCarbonFilterFarmControl(QObject *parent) :
	AMPseudoMotorControl("BioXAS Carbon Filter Farm", "mm", parent)
{
	// Initialize local variables.

	upstreamActuator_ = 0;
	downstreamActuator_ = 0;

	// Initialize inherited variables.
	// Subclasses should initialize their own enum states and enum move states, as these will vary between beamlines.

	value_ = Filter::None;
	setpoint_ = Filter::None;
	minimumValue_ = Filter::None;
	maximumValue_ = Filter::SevenHundredFifty;

	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Filter Farm");

	// Emit enumChanged signals when connected, value changes, and setpoint changes. This is to make sure the control is viewed as an enum.

	connect( this, SIGNAL(connected(bool)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(valueChanged(double)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(setpointChanged(double)), this, SIGNAL(enumChanged()) );

	// Current settings.

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

	if (value == Filter::None ||
			value == Filter::Fifty ||
			value == Filter::SeventyFive ||
			value == Filter::FiveHundred ||
			value == Filter::FiveHundredSeventyFive ||
			value == Filter::SevenHundred ||
			value == Filter::SevenHundredFifty
			) {
		result = true;
	}

	return result;
}

bool BioXASCarbonFilterFarmControl::validSetpoint(double value) const
{
	bool result = false;

	if (value == Filter::Fifty ||
			value == Filter::SeventyFive ||
			value == Filter::FiveHundred ||
			value == Filter::FiveHundredSeventyFive ||
			value == Filter::SevenHundred ||
			value == Filter::SevenHundredFifty
			) {
		result = true;
	}

	return result;
}

QString BioXASCarbonFilterFarmControl::filterToString(Filter::Thickness value)
{
	QString result = QString();

	switch (value) {
	case Filter::Invalid:
		result = "Invalid";
		break;
	case Filter::None:
		result = "None";
		break;
	case Filter::Fifty:
		result = "50";
		break;
	case Filter::SeventyFive:
		result = "75";
		break;
	case Filter::FiveHundred:
		result = "500";
		break;
	case Filter::FiveHundredSeventyFive:
		result = "575";
		break;
	case Filter::SevenHundred:
		result = "700";
		break;
	case Filter::SevenHundredFifty:
		result = "750";
		break;
	default:
		break;
	}

	return result;
}

BioXASCarbonFilterFarmControl::Filter::Thickness BioXASCarbonFilterFarmControl::stringToFilter(const QString &string)
{
	Filter::Thickness result = Filter::Invalid;

	if (!string.isEmpty()) {
		if (string == "None")
			result= Filter::None;
		else if (string == "50")
			result = Filter::Fifty;
		else if (string == "75")
			result = Filter::SeventyFive;
		else if (string == "500")
			result = Filter::FiveHundred;
		else if (string == "575")
			result = Filter::FiveHundredSeventyFive;
		else if (string == "700")
			result = Filter::SevenHundred;
		else if (string == "750")
			result = Filter::SevenHundredFifty;
	}

	return result;
}

double BioXASCarbonFilterFarmControl::filterToDouble(Filter::Thickness filter)
{
	double result = -1;

	switch (filter) {
	case Filter::None:
		result = 0;
		break;
	case Filter::Fifty:
		result = 50;
		break;
	case Filter::SeventyFive:
		result = 75;
		break;
	case Filter::FiveHundred:
		result = 500;
		break;
	case Filter::FiveHundredSeventyFive:
		result = 575;
		break;
	case Filter::SevenHundred:
		result = 700;
		break;
	case Filter::SevenHundredFifty:
		result = 750;
		break;
	default:
		break;
	}

	return result;
}

BioXASCarbonFilterFarmControl::Filter::Thickness BioXASCarbonFilterFarmControl::doubleToFilter(double value)
{
	Filter::Thickness filter = Filter::Invalid;

	switch ((int)value) {
	case 50:
		filter = Filter::Fifty;
		break;
	case 75:
		filter = Filter::SeventyFive;
		break;
	case 500:
		filter = Filter::FiveHundred;
		break;
	case 575:
		filter = Filter::FiveHundredSeventyFive;
		break;
	case 700:
		filter = Filter::SevenHundred;
		break;
	case 750:
		filter = Filter::SevenHundredFifty;
		break;
	default:
		break;
	}

	return filter;
}

BioXASCarbonFilterFarmControl::Filter::Thickness BioXASCarbonFilterFarmControl::filter(double index)
{
	Filter::Thickness result = Filter::Invalid;

	switch ((int)index) {
	case Filter::None:
		result = Filter::None;
		break;
	case Filter::Fifty:
		result = Filter::Fifty;
		break;
	case Filter::SeventyFive:
		result = Filter::SeventyFive;
		break;
	case Filter::FiveHundred:
		result = Filter::FiveHundred;
		break;
	case Filter::FiveHundredSeventyFive:
		result = Filter::FiveHundredSeventyFive;
		break;
	case Filter::SevenHundred:
		result = Filter::SevenHundred;
		break;
	case Filter::SevenHundredFifty:
		result = Filter::SevenHundredFifty;
		break;
	default:
		break;
	}

	return result;
}

BioXASCarbonFilterFarmControl::Filter::Thickness BioXASCarbonFilterFarmControl::filterAtWindow(Actuator::Position actuator, BioXASCarbonFilterFarmActuatorControl::Window::Selection window)
{
	Filter::Thickness filter = Filter::Invalid;

	if (actuator == Actuator::Upstream) {
		filter = upstreamFilterMap_.value(window, Filter::Invalid);

	} else if (actuator == Actuator::Downstream) {
		filter = downstreamFilterMap_.value(window, Filter::Invalid);
	}

	return filter;
}

void BioXASCarbonFilterFarmControl::setWindowFilter(Actuator::Position actuator, BioXASCarbonFilterFarmActuatorControl::Window::Selection window, Filter::Thickness filterThickness)
{
	if (actuator == Actuator::Upstream) {
		upstreamFilterMap_.insert(window, filterThickness);

	} else if (actuator == Actuator::Downstream) {
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
	Filter::Thickness newFilter = Filter::Invalid;

	if (isConnected()) {
		BioXASCarbonFilterFarmActuatorControl::Window::Selection upstreamWindow = upstreamActuator_->window(upstreamActuator_->value());
		BioXASCarbonFilterFarmActuatorControl::Window::Selection downstreamWindow = downstreamActuator_->window(downstreamActuator_->value());

		newFilter = calculateTotalFilterFromWindows(upstreamWindow, downstreamWindow);
	}

	setValue(newFilter);
}

void BioXASCarbonFilterFarmControl::updateIsMoving()
{
	if (isConnected()) {
		setIsMoving( upstreamActuator_->isMoving() || downstreamActuator_->isMoving() );
	}
}

AMAction3* BioXASCarbonFilterFarmControl::createMoveAction(double setpoint)
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Moving filter farm actuators", "Moving filter farm actuators"), AMListAction3::Parallel);

	switch ((int)setpoint) {

	case Filter::None: // Main, Side, Imaging
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::None));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::None));
		break;

	case Filter::Fifty: // Main, Side
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Bottom));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::None));
		break;

	case Filter::SeventyFive: // Imaging
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Bottom));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::None));
		break;

	case Filter::FiveHundred: // Imaging
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::None));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Bottom));
		break;

	case Filter::FiveHundredSeventyFive: // Imaging
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Bottom));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Bottom));
		break;

	case Filter::SevenHundred: // Main, Side
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::None));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Bottom));
		break;

	case Filter::SevenHundredFifty: // Main, Side
		action->addSubAction(AMActionSupport::buildControlMoveAction(upstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Bottom));
		action->addSubAction(AMActionSupport::buildControlMoveAction(downstreamActuator_, BioXASCarbonFilterFarmActuatorControl::Window::Bottom));
		break;
	default:
		action->deleteLater();
		action = 0;
		break;
	}

	return action;
}

BioXASCarbonFilterFarmControl::Filter::Thickness BioXASCarbonFilterFarmControl::calculateTotalFilterFromWindows(BioXASCarbonFilterFarmActuatorControl::Window::Selection upstreamWindow, BioXASCarbonFilterFarmActuatorControl::Window::Selection downstreamWindow)
{
	Filter::Thickness upstreamThickness = Filter::Invalid;
	Filter::Thickness downstreamThickness = Filter::Invalid;

	if (upstreamActuator_ && upstreamActuator_->validValue(upstreamWindow) && upstreamFilterMap_.contains(upstreamWindow)) {
		upstreamThickness = filterAtWindow(Actuator::Upstream, upstreamWindow);
	}

	if (downstreamActuator_ && downstreamActuator_->validValue(downstreamWindow) && downstreamFilterMap_.contains(downstreamWindow)) {
		downstreamThickness = filterAtWindow(Actuator::Downstream, downstreamWindow);
	}

	return calculateTotalFilter(upstreamThickness, downstreamThickness);
}

BioXASCarbonFilterFarmControl::Filter::Thickness BioXASCarbonFilterFarmControl::calculateTotalFilter(Filter::Thickness upstreamFilter, Filter::Thickness downstreamFilter)
{
	return doubleToFilter( filterToDouble(upstreamFilter) + filterToDouble(downstreamFilter) );
}

