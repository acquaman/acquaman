#include "BioXASXIAFilters.h"
#include "util/AMErrorMonitor.h"
#include "actions3/AMActionSupport.h"

BioXASXIAFilters::BioXASXIAFilters(QObject *parent) :
	AMCompositeControl("BioXAS XIA Filter Control", "", parent)
{
	// Initialize local variables.

	value_ = Filter::Invalid;
	setpoint_ = Filter::Invalid;
	moveInProgress_ = false;

	filter1_ = 0;
	filter2_ = 0;
	filter3_ = 0;
	filter4_ = 0;

	moveCancelledMapper_ = new QSignalMapper(this);
	moveFailedMapper_ = new QSignalMapper(this);
	moveSucceededMapper_ = new QSignalMapper(this);

	// Initialize inherited variables.
	// If enum states and move states end up being beamline specific, they should be initialized in the beamline subclass constructors.

	setEnumStates(QStringList());
	setMoveEnumStates(QStringList());
	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("XIA filter control");

	// Make connections.

	connect( this, SIGNAL(connected(bool)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(valueChanged(double)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(setpointChanged(double)), this, SIGNAL(enumChanged()) );

	connect( moveCancelledMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveCancelled(QObject*)) );
	connect( moveFailedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveFailed(QObject*)) );
	connect( moveSucceededMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveSucceeded(QObject*)) );

	// Current settings.

	updateValue();
}

BioXASXIAFilters::~BioXASXIAFilters()
{

}

bool BioXASXIAFilters::canMove() const
{
	return (isConnected() && filter1_->canMove() && filter2_->canMove() && filter3_->canMove() && filter4_->canMove());
}

bool BioXASXIAFilters::isConnected() const
{
	bool filter1OK = (filter1_ && filter1_->isConnected() && children_.contains(filter1_));
	bool filter2OK = (filter2_ && filter2_->isConnected() && children_.contains(filter2_));
	bool filter3OK = (filter3_ && filter3_->isConnected() && children_.contains(filter3_));
	bool filter4OK = (filter4_ && filter4_->isConnected() && children_.contains(filter4_));

	return (filter1OK && filter2OK && filter3OK && filter4OK);
}

AMControl* BioXASXIAFilters::filterControl(Filter::Name name) const
{
	AMControl *result = 0;

	switch (name) {

	case Filter::First:
		result = filter1_;
		break;

	case Filter::Second:
		result = filter2_;
		break;

	case Filter::Third:
		result = filter3_;
		break;

	case Filter::Fourth:
		result = filter4_;
		break;

	default:
		break;
	}

	return result;
}

BioXASXIAFilters::Filter::Position BioXASXIAFilters::filterPosition(Filter::Name name) const
{
	Filter::Position result = Filter::Other;

	AMControl *control = filterControl(name);

	if (filterControlIn(control))
		result = Filter::In;

	else if (filterControlOut(control))
		result = Filter::Out;

	return result;
}

BioXASXIAFilters::Filter::Thickness BioXASXIAFilters::filterThickness(double value)
{
	Q_UNUSED(value)
	return Filter::Invalid;
}

bool BioXASXIAFilters::validFilter(double value) const
{
	Q_UNUSED(value)
	return false;
}

bool BioXASXIAFilters::validFilterSetpoint(double value) const
{
	Q_UNUSED(value)
	return false;
}

AMControl::FailureExplanation BioXASXIAFilters::move(double setpoint)
{
	Q_UNUSED(setpoint)

	if (!isConnected()) {
		AMErrorMon::alert(this, BIOXAS_XIA_FILTERS_NOT_CONNECTED, "Failed to move XIA filters: filters not connected.");
		return AMControl::NotConnectedFailure;
	}

	if (isMoving()) {
		AMErrorMon::alert(this, BIOXAS_XIA_FILTERS_ALREADY_MOVING, "Failed to move XIA filters: filters already moving.");
		return AMControl::AlreadyMovingFailure;
	}

	Filter::Thickness thicknessSetpoint = filterThickness(setpoint);

	if (!validFilterSetpoint(thicknessSetpoint)) {
		AMErrorMon::alert(this, BIOXAS_XIA_FILTERS_INVALID_SETPOINT, "Failed to move XIA filters: invalid setpoint.");
		return AMControl::LimitFailure;
	}

	AMAction3 *moveAction = createMoveAction(thicknessSetpoint);

	if (!moveAction) {
		AMErrorMon::alert(this, BIOXAS_XIA_FILTERS_MOVE_FAILED, "Failed to move XIA filters.");
		return AMControl::LimitFailure;
	}

	// Create signal mappings for this action.

	connect( moveAction, SIGNAL(started()), this, SLOT(onMoveStarted()) );

	moveCancelledMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(cancelled()), moveCancelledMapper_, SLOT(map()) );

	moveFailedMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(failed()), moveFailedMapper_, SLOT(map()) );

	moveSucceededMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(succeeded()), moveSucceededMapper_, SLOT(map()) );

	// Run action.

	moveAction->start();

	return AMControl::NoFailure;
}

void BioXASXIAFilters::setValue(Filter::Thickness newValue)
{
	if (value_ != newValue) {
		value_ = newValue;
		emit valueChanged((double)value_);
	}
}

void BioXASXIAFilters::setSetpoint(Filter::Thickness newValue)
{
	if (setpoint_ != newValue) {
		setpoint_ = newValue;
		emit setpointChanged((double)setpoint_);
	}
}

void BioXASXIAFilters::setMoveInProgress(bool isMoving)
{
	if (moveInProgress_ != isMoving) {
		moveInProgress_ = isMoving;
		emit movingChanged(moveInProgress_);

		if (moveInProgress_)
			emit moveStarted();
	}
}

void BioXASXIAFilters::updateValue()
{
	return;
}

bool BioXASXIAFilters::setFilterPosition(Filter::Name name, Filter::Position position)
{
	return setFilterControlPosition(filterControl(name), position);
}

bool BioXASXIAFilters::setFilterControlPosition(AMControl *filterControl, Filter::Position position)
{
	bool result = false;

	if (filterControl) {
		switch (position) {
		case Filter::In:
			result = setFilterControlIn(filterControl);
			break;
		case Filter::Out:
			result = setFilterControlOut(filterControl);
			break;
		default:
			break;
		}
	}

	return result;
}

bool BioXASXIAFilters::setFilterControlIn(AMControl *filterControl)
{
	bool result = false;

	if (filterControl) {
		AMControl::FailureExplanation moveResult = filterControl->move(BIOXAS_XIA_FILTERS_POSITION_IN);
		result = (moveResult == AMControl::NoFailure);
	}

	return result;
}

bool BioXASXIAFilters::setFilterControlOut(AMControl *filterControl)
{
	bool result = false;

	if (filterControl) {
		AMControl::FailureExplanation moveResult = filterControl->move(BIOXAS_XIA_FILTERS_POSITION_OUT);
		result = (moveResult == AMControl::NoFailure);
	}

	return result;
}

void BioXASXIAFilters::onMoveStarted()
{
	setMoveInProgress(true);
}

void BioXASXIAFilters::onMoveCancelled(QObject *action)
{
	moveCleanup(action);
	emit moveFailed(AMControl::WasStoppedFailure);
}

void BioXASXIAFilters::onMoveFailed(QObject *action)
{
	moveCleanup(action);
	emit moveFailed(AMControl::OtherFailure);
}

void BioXASXIAFilters::onMoveSucceeded(QObject *action)
{
	moveCleanup(action);
	emit moveSucceeded();
}

AMAction3* BioXASXIAFilters::createMoveAction(Filter::Thickness setpoint)
{
	Q_UNUSED(setpoint)
	return 0;
}

AMAction3* BioXASXIAFilters::createSetFilterPositionAction(Filter::Name name, Filter::Position position)
{
	return createSetFilterControlPositionAction(filterControl(name), position);
}

AMAction3* BioXASXIAFilters::createSetFilterControlPositionAction(AMControl *filterControl, Filter::Position position)
{
	AMAction3 *result = 0;

	if (filterControl) {
		switch (position) {
		case Filter::In:
			result = createSetFilterControlIn(filterControl);
			break;
		case Filter::Out:
			result = createSetFilterControlOut(filterControl);
			break;
		default:
			break;
		}
	}

	return result;
}

AMAction3* BioXASXIAFilters::createSetFilterControlIn(AMControl *filterControl)
{
	AMAction3 *result = 0;

	if (filterControl) {
		result = AMActionSupport::buildControlMoveAction(filterControl, BIOXAS_XIA_FILTERS_POSITION_IN);
	}

	return result;
}

AMAction3* BioXASXIAFilters::createSetFilterControlOut(AMControl *filterControl)
{
	AMAction3 *result = 0;

	if (filterControl) {
		result = AMActionSupport::buildControlMoveAction(filterControl, BIOXAS_XIA_FILTERS_POSITION_OUT);
	}

	return result;
}

bool BioXASXIAFilters::filterControlInPosition(AMControl *filterControl, Filter::Position position) const
{
	bool result = false;

	if (filterControl) {
		switch (position) {
		case Filter::Out:
			result = filterControlOut(filterControl);
			break;
		case Filter::In:
			result = filterControlOut(filterControl);
			break;
		case Filter::Other:
			result = filterControlOther(filterControl);
			break;
		default:
			break;
		}
	}

	return result;
}

bool BioXASXIAFilters::filterControlIn(AMControl *filterControl) const
{
	bool result = false;

	if (filterControl)
		result = ( fabs(filterControl->value() - BIOXAS_XIA_FILTERS_POSITION_IN) < BIOXAS_XIA_FILTERS_POSITION_TOLERANCE );

	return result;
}

bool BioXASXIAFilters::filterControlOut(AMControl *filterControl) const
{
	bool result = false;

	if (filterControl)
		result = ( fabs(filterControl->value() - BIOXAS_XIA_FILTERS_POSITION_OUT) < BIOXAS_XIA_FILTERS_POSITION_TOLERANCE );

	return result;
}

bool BioXASXIAFilters::filterControlOther(AMControl *filterControl) const
{
	bool result = false;

	if (filterControl)
		result = (!filterControlIn(filterControl) && !filterControlOut(filterControl));

	return result;
}

void BioXASXIAFilters::moveCleanup(QObject *action)
{
	if (action) {

		setMoveInProgress(false);

		action->disconnect();

		moveCancelledMapper_->removeMappings(action);
		moveFailedMapper_->removeMappings(action);
		moveSucceededMapper_->removeMappings(action);

		action->deleteLater();
	}
}
