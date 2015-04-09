#include "BioXASCarbonFilterFarm.h"

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
	case Filter::None:
		result = "None";
		break;
	case Filter::Between:
		result = "Between";
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

AMControl::FailureExplanation BioXASCarbonFilterFarmControl::move(double setpoint)
{
	if (!isConnected()) {
		return AMControl::NotConnectedFailure;
	}

	if (isMoving()) {
		return AMControl::AlreadyMovingFailure;
	}

	if (!validFilterSetpoint(setpoint)) {
		return AMControl::LimitFailure;
	}

	// Update the setpoint.

	setFilterSetpoint(setpoint);

	// Create an action that moves the appropriate actuators to the desired filter thickness.

	AMAction3 *moveAction = createMoveAction(setpoint_);

	if (!moveAction) {
		return AMControl::LimitFailure;
	}

	// Create move action signal mappings.

	cancelledMapper_->setMapping(moveAction, moveAction);
	failedMapper_->setMapping(moveAction, moveAction);
	succeededMapper_->setMapping(moveAction, moveAction);

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

void BioXASCarbonFilterFarmControl::setFilterMapping(Actuator::Position actuator, double window, Filter::Thickness filterThickness)
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
	double newFilter = calculateTotalFilterFromWindows(upstreamActuator_->value(), downstreamActuator_->value());
	setFilter(newFilter);
}

BioXASCarbonFilterFarmControl::Filter::Thickness BioXASCarbonFilterFarmControl::calculateTotalFilterFromWindows(double upstreamWindow, double downstreamWindow)
{
	Filter::Thickness upstreamThickness = Filter::None;
	Filter::Thickness downstreamThickness = Filter::None;

	if (upstreamActuator_->validWindow(upstreamWindow) && upstreamFilterMap_.contains(upstreamWindow)) {
		upstreamThickness = upstreamFilterMap_.value(upstreamWindow);
	}

	if (downstreamActuator_->validWindow(downstreamWindow) && downstreamFilterMap_.contains(downstreamWindow)) {
		downstreamThickness = downstreamFilterMap_.value(downstreamWindow);
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
