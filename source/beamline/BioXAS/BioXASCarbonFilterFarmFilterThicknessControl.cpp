#include "BioXASCarbonFilterFarmFilterThicknessControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASCarbonFilterFarmFilterThicknessControl::BioXASCarbonFilterFarmFilterThicknessControl(const QString &name, const QString &units, BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl *upstreamFilterThickness, BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl *downstreamFilterThickness, QObject *parent) :
	BioXASCarbonFilterFarmControl(name, units, parent)
{
	// Initialize inherited variables.

	value_ = -1;
	setpoint_ = -1;
	minimumValue_ = 0;
	maximumValue_ = 1000;

	setContextKnownDescription("Filter Thickness Control");

	// Initialize local variables.

	upstreamFilterThickness_ = 0;
	downstreamFilterThickness_ = 0;

	// Current settings.

	setUpstreamFilterThicknessControl(upstreamFilterThickness);
	setDownstreamFilterThicknessControl(downstreamFilterThickness);
}

BioXASCarbonFilterFarmFilterThicknessControl::~BioXASCarbonFilterFarmFilterThicknessControl()
{

}

bool BioXASCarbonFilterFarmFilterThicknessControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = ( upstreamFilterThickness_->canMeasure() && downstreamFilterThickness_->canMeasure() );

	return result;
}

bool BioXASCarbonFilterFarmFilterThicknessControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = ( upstreamFilterThickness_->canMove() && downstreamFilterThickness_->canMove() );

	return result;
}

bool BioXASCarbonFilterFarmFilterThicknessControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = ( upstreamFilterThickness_->canStop() && downstreamFilterThickness_->canStop() );

	return result;
}

bool BioXASCarbonFilterFarmFilterThicknessControl::validValue(double value) const
{
	bool result = (upstreamFilterThicknessesMap_.contains(value) && downstreamFilterThicknessesMap_.contains(value));

	return result;
}

double BioXASCarbonFilterFarmFilterThicknessControl::upstreamFilterThickness(double totalFilterThickness)
{
	double result = upstreamFilterThicknessesMap_.value(totalFilterThickness, -1);

	return result;
}

double BioXASCarbonFilterFarmFilterThicknessControl::downstreamFilterThickness(double totalFilterThickness)
{
	double result = downstreamFilterThicknessesMap_.value(totalFilterThickness, -1);

	return result;
}

void BioXASCarbonFilterFarmFilterThicknessControl::setUpstreamFilterThicknessControl(BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl *newControl)
{
	if (upstreamFilterThickness_ != newControl) {

		if (upstreamFilterThickness_)
			removeChildControl(upstreamFilterThickness_);

		upstreamFilterThickness_ = newControl;

		if (upstreamFilterThickness_)
			addChildControl(upstreamFilterThickness_);

		updateStates();
	}
}

void BioXASCarbonFilterFarmFilterThicknessControl::setDownstreamFilterThicknessControl(BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl *newControl)
{
	if (downstreamFilterThickness_ != newControl) {

		if (downstreamFilterThickness_)
			removeChildControl(downstreamFilterThickness_);

		downstreamFilterThickness_ = newControl;

		if (downstreamFilterThickness_)
			addChildControl(downstreamFilterThickness_);

		updateStates();
	}
}

void BioXASCarbonFilterFarmFilterThicknessControl::setFilterThickness(double totalFilterThickness, double upstreamFilterThickness, double downstreamFilterThickness)
{
	upstreamFilterThicknessesMap_.insert(totalFilterThickness, upstreamFilterThickness);
	downstreamFilterThicknessesMap_.insert(totalFilterThickness, downstreamFilterThickness);
}

void BioXASCarbonFilterFarmFilterThicknessControl::updateConnected()
{
	bool isConnected = (
				upstreamFilterThickness_ && upstreamFilterThickness_->isConnected() &&
				downstreamFilterThickness_ && downstreamFilterThickness_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASCarbonFilterFarmFilterThicknessControl::updateValue()
{
	double newValue = -1;

	if (isConnected()) {
		double filterThickness = upstreamFilterThickness_->value() + downstreamFilterThickness_->value();

		if (validValue(filterThickness))
			newValue = filterThickness;
	}

	setValue(newValue);
}

void BioXASCarbonFilterFarmFilterThicknessControl::updateMoving()
{
	bool isMoving = false;

	if (isConnected())
		isMoving = ( upstreamFilterThickness_->isMoving() || downstreamFilterThickness_->isMoving() );

	setIsMoving(isMoving);
}

AMAction3* BioXASCarbonFilterFarmFilterThicknessControl::createMoveAction(double setpoint)
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Move BioXAS Carbon Filter Farm Actuators", "Move BioXAS Carbon Filter Farm Actuators"), AMListAction3::Parallel);

	AMAction3 *upstreamMove = AMActionSupport::buildControlMoveAction(upstreamFilterThickness_, upstreamFilterThicknessesMap_.value(setpoint));
	action->addSubAction(upstreamMove);

	AMAction3 *downstreamMove = AMActionSupport::buildControlMoveAction(downstreamFilterThickness_, downstreamFilterThicknessesMap_.value(setpoint));
	action->addSubAction(downstreamMove);

	return action;
}
