#include "BioXASCarbonFilterFarm.h"
#include "util/AMErrorMonitor.h"

BioXASCarbonFilterFarmControl::BioXASCarbonFilterFarmControl(QObject *parent) :
	AMCompositeControl("BioXAS Carbon Filter Farm", "mm", parent)
{
	// Initialize local variables.

	value_ = Filter::None;
	setpoint_ = Filter::None;

	upstreamActuator_ = 0;
	downstreamActuator_ = 0;

	cancelledMapper_ = new QSignalMapper(this);
	connect( cancelledMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveCancelled(QObject*)) );

	failedMapper_ = new QSignalMapper(this);
	connect( failedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveFailed(QObject*)) );

	succeededMapper_ = new QSignalMapper(this);
	connect( succeededMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveSucceeded(QObject*)) );

	// Initialize inherited variables.
	// Subclasses should initialize their own enum states and enum move states, as these will vary between beamlines.

	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Filter Farm");

	// Emit enumChanged signals when connected, value changes, and setpoint changes. This is to make sure the control is viewed as an enum.

	connect( this, SIGNAL(connected(bool)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(valueChanged(double)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(setpointChanged(double)), this, SIGNAL(enumChanged()) );

	// Current settings.

	updateFilter();
}

BioXASCarbonFilterFarmControl::~BioXASCarbonFilterFarmControl()
{

}

bool BioXASCarbonFilterFarmControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = true;

	return result;
}

bool BioXASCarbonFilterFarmControl::isConnected() const
{
	bool result = false;

	if (upstreamActuator_ && downstreamActuator_ && children_.contains(upstreamActuator_) && children_.contains(downstreamActuator_)) {
		result = AMCompositeControl::isConnected();
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
	Filter::Thickness filter = Filter::None;

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

AMControl::FailureExplanation BioXASCarbonFilterFarmControl::move(double setpoint)
{
	if (!isConnected()) {
		AMErrorMon::alert(this, BIOXAS_FILTER_FARM_NOT_CONNECTED, "Failed to change filters: filter farm not connected.");
		return AMControl::NotConnectedFailure;
	}

	if (isMoving()) {
		AMErrorMon::alert(this, BIOXAS_FILTER_FARM_ALREADY_MOVING, "Failed to change filters: filter farm already moving.");
		return AMControl::AlreadyMovingFailure;
	}

	if (!validFilterSetpoint(setpoint)) {
		AMErrorMon::alert(this, BIOXAS_FILTER_FARM_INVALID_SETPOINT, "Failed to change filters: invalid setpoint.");
		return AMControl::LimitFailure;
	}

	// Update the setpoint.

	setFilterSetpoint(setpoint);

	// Create actions that move the appropriate actuators to the desired filter thickness.

	AMAction3 *moveAction = createMoveAction(setpoint);

	if (!moveAction) {
		AMErrorMon::alert(this, BIOXAS_FILTER_FARM_MOVE_FAILED, "Failed to change filters.");
		return AMControl::LimitFailure;
	}

	// Create move action signal mappings.

	connect( moveAction, SIGNAL(started()), this, SLOT(onMoveStarted()) );

	cancelledMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(cancelled()), cancelledMapper_, SLOT(map()) );

	failedMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(failed()), failedMapper_, SLOT(map()) );

	succeededMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(succeeded()), succeededMapper_, SLOT(map()) );

	// Run action.

	moveAction->start();

	return AMControl::NoFailure;
}

void BioXASCarbonFilterFarmControl::setFilter(double newValue)
{
	if (value_ != newValue) {
		value_ = newValue;
		emit valueChanged(value_);
	}
}

void BioXASCarbonFilterFarmControl::setFilterSetpoint(double newSetpoint)
{
	if (setpoint_ != newSetpoint) {
		setpoint_ = newSetpoint;
		emit setpointChanged(setpoint_);
	}
}

void BioXASCarbonFilterFarmControl::setWindowFilter(Actuator::Position actuator, BioXASCarbonFilterFarmActuatorControl::Window::Selection window, Filter::Thickness filterThickness)
{
	if (actuator == Actuator::Upstream) {
		upstreamFilterMap_.insert(window, filterThickness);

	} else if (actuator == Actuator::Downstream) {
		downstreamFilterMap_.insert(window, filterThickness);
	}
}

void BioXASCarbonFilterFarmControl::setMoveInProgress(bool isMoving)
{
	if (moveInProgress_ != isMoving) {
		moveInProgress_ = isMoving;
		emit movingChanged(moveInProgress_);

		if (moveInProgress_)
			emit moveStarted();
	}
}

void BioXASCarbonFilterFarmControl::onMoveStarted()
{
	setMoveInProgress(true);
}

void BioXASCarbonFilterFarmControl::onMoveCancelled(QObject *action)
{
	moveCleanup(action);
	emit moveFailed(AMControl::WasStoppedFailure);
}

void BioXASCarbonFilterFarmControl::onMoveFailed(QObject *action)
{
	moveCleanup(action);
	emit moveFailed(AMControl::OtherFailure);
}

void BioXASCarbonFilterFarmControl::onMoveSucceeded(QObject *action)
{
	moveCleanup(action);
	emit moveSucceeded();
}

void BioXASCarbonFilterFarmControl::updateFilter()
{
	Filter::Thickness newFilter = Filter::Invalid;

	if (upstreamActuator_ && upstreamActuator_->isConnected() && downstreamActuator_ && downstreamActuator_->isConnected()) {
		BioXASCarbonFilterFarmActuatorControl::Window::Selection upstreamWindow = upstreamActuator_->window(upstreamActuator_->value());
		BioXASCarbonFilterFarmActuatorControl::Window::Selection downstreamWindow = downstreamActuator_->window(downstreamActuator_->value());

		newFilter = calculateTotalFilterFromWindows(upstreamWindow, downstreamWindow);
	}

	setFilter(newFilter);
}

BioXASCarbonFilterFarmControl::Filter::Thickness BioXASCarbonFilterFarmControl::calculateTotalFilterFromWindows(BioXASCarbonFilterFarmActuatorControl::Window::Selection upstreamWindow, BioXASCarbonFilterFarmActuatorControl::Window::Selection downstreamWindow)
{
	Filter::Thickness upstreamThickness = Filter::Invalid;
	Filter::Thickness downstreamThickness = Filter::Invalid;

	if (upstreamActuator_ && upstreamActuator_->validWindow(upstreamWindow) && upstreamFilterMap_.contains(upstreamWindow)) {
		upstreamThickness = filterAtWindow(Actuator::Upstream, upstreamWindow);
	}

	if (downstreamActuator_ && downstreamActuator_->validWindow(downstreamWindow) && downstreamFilterMap_.contains(downstreamWindow)) {
		downstreamThickness = filterAtWindow(Actuator::Downstream, downstreamWindow);
	}

	return calculateTotalFilter(upstreamThickness, downstreamThickness);
}

BioXASCarbonFilterFarmControl::Filter::Thickness BioXASCarbonFilterFarmControl::calculateTotalFilter(Filter::Thickness upstreamFilter, Filter::Thickness downstreamFilter)
{
	return doubleToFilter(filterToDouble(upstreamFilter) + filterToDouble(downstreamFilter));
}

void BioXASCarbonFilterFarmControl::moveCleanup(QObject *action)
{
	if (action) {
		disconnect( action, 0, cancelledMapper_, 0 );
		disconnect( action, 0, failedMapper_, 0 );
		disconnect( action, 0, succeededMapper_, 0 );

		cancelledMapper_->removeMappings(action);
		failedMapper_->removeMappings(action);
		succeededMapper_->removeMappings(action);

		action->deleteLater();
	}
}
