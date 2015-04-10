#include "BioXASXIAFilters.h"

BioXASXIAFilters::BioXASXIAFilters(QObject *parent) :
	AMCompositeControl("BioXAS XIA Filter Control", "", parent)
{
	// Initialize local variables.

	value_ = 0;
	setpoint_ = 0;
	moveInProgress_ = false;

	filter1_ = 0;
	filter2_ = 0;
	filter3_ = 0;
	filter4_ = 0;

	moveCancelledMapper_ = new QSignalMapper(this);
	moveFailedMapper_ = new QSignalMapper(this);
	moveSucceededMapper_ = new QSignalMapper(this);

	// Initialize inherited variables.

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

AMPVControl* BioXASXIAFilters::filterControl(Filter::Name name) const
{
	AMPVControl *result = 0;

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

AMControl::FailureExplanation BioXASXIAFilters::move(double setpoint)
{
	Q_UNUSED(setpoint)
	return AMControl::NoFailure;
}

void BioXASXIAFilters::setValue(double newValue)
{
	if (value_ != newValue) {
		value_ = newValue;
		emit valueChanged(value_);
	}
}

void BioXASXIAFilters::setSetpoint(double newValue)
{
	if (setpoint_ != newValue) {
		setpoint_ = newValue;
		emit setpointChanged(setpoint_);
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

void BioXASXIAFilters::setFilterPosition(Filter::Name name, Filter::Position position)
{
	AMControl *filter = filterControl(name);

	if (filter)
		filter->move(position);
}

AMAction3* BioXASXIAFilters::createMoveAction()
{
	return 0;
}
