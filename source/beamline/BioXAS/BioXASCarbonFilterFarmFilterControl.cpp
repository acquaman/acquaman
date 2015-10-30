#include "BioXASCarbonFilterFarmFilterControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASCarbonFilterFarmFilterControl::BioXASCarbonFilterFarmFilterControl(const QString &name, const QString &units, QObject *parent) :
	AMPseudoMotorControl(name, units, parent)
{
	// Initialize inherited variables.

	value_ = -1;
	setpoint_ = -1;
	minimumValue_ = 0;
	maximumValue_ = 1000;

	setContextKnownDescription("Filter Control");

	// Initialize local variables.

	upstreamActuator_ = 0;
	downstreamActuator_ = 0;
}

BioXASCarbonFilterFarmFilterControl::~BioXASCarbonFilterFarmFilterControl()
{

}

bool BioXASCarbonFilterFarmFilterControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = ( upstreamActuator_->filter()->canMeasure() && downstreamActuator_->filter()->canMeasure() );

	return result;
}

bool BioXASCarbonFilterFarmFilterControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = ( upstreamActuator_->filter()->canMove() && downstreamActuator_->filter()->canMove() );

	return result;
}

bool BioXASCarbonFilterFarmFilterControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = ( upstreamActuator_->filter()->canStop() && downstreamActuator_->filter()->canStop() );

	return result;
}

bool BioXASCarbonFilterFarmFilterControl::validValue(double value) const
{
	bool result = (upstreamFilterThicknessesMap_.contains(value) && downstreamFilterThicknessesMap_.contains(value));

	return result;
}

//double BioXASCarbonFilterFarmFilterControl::upstreamFilterThickness(double totalFilterThickness)
//{
//	double result = upstreamFilterThicknessesMap_.value(totalFilterThickness, -1);

//	return result;
//}

//double BioXASCarbonFilterFarmFilterControl::downstreamFilterThickness(double totalFilterThickness)
//{
//	double result = downstreamFilterThicknessesMap_.value(totalFilterThickness, -1);

//	return result;
//}

void BioXASCarbonFilterFarmFilterControl::setUpstreamActuator(BioXASCarbonFilterFarmActuatorControl *newControl)
{
	if (upstreamActuator_ != newControl) {

		if (upstreamActuator_)
			removeChildControl(upstreamActuator_);

		upstreamActuator_ = newControl;

		if (upstreamActuator_)
			addChildControl(upstreamActuator_);

		updateStates();

		emit upstreamActuatorChanged(upstreamActuator_);
	}
}

void BioXASCarbonFilterFarmFilterControl::setDownstreamActuator(BioXASCarbonFilterFarmActuatorControl *newControl)
{
	if (downstreamActuator_ != newControl) {

		if (downstreamActuator_)
			removeChildControl(downstreamActuator_);

		downstreamActuator_ = newControl;

		if (downstreamActuator_)
			addChildControl(downstreamActuator_);

		updateStates();

		emit downstreamActuatorChanged(downstreamActuator_);
	}
}

//void BioXASCarbonFilterFarmFilterControl::setFilterThickness(double totalFilterThickness, double upstreamFilterThickness, double downstreamFilterThickness)
//{
//	upstreamFilterThicknessesMap_.insert(totalFilterThickness, upstreamFilterThickness);
//	downstreamFilterThicknessesMap_.insert(totalFilterThickness, downstreamFilterThickness);
//}

void BioXASCarbonFilterFarmFilterControl::updateConnected()
{
	bool isConnected = (
				upstreamActuator_ && upstreamActuator_->isConnected() &&
				downstreamActuator_ && downstreamActuator_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASCarbonFilterFarmFilterControl::updateValue()
{
	double newValue = -1;

	if (isConnected()) {
		double totalFilter = upstreamActuator_->filter()->value() + downstreamActuator_->filter()->value();

		if (validValue(totalFilter))
			newValue = totalFilter;
	}

	setValue(newValue);
}

void BioXASCarbonFilterFarmFilterControl::updateMoving()
{
	bool isMoving = false;

	if (isConnected())
		isMoving = ( upstreamActuator_->filter()->isMoving() || downstreamActuator_->filter()->isMoving() );

	setIsMoving(isMoving);
}

AMAction3* BioXASCarbonFilterFarmFilterControl::createMoveAction(double setpoint)
{
//	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Move BioXAS Carbon Filter Farm Actuators", "Move BioXAS Carbon Filter Farm Actuators"), AMListAction3::Parallel);

//	AMAction3 *upstreamMove = AMActionSupport::buildControlMoveAction(upstreamActuator_->filter(), upstreamFilterThicknessesMap_.value(setpoint));
//	action->addSubAction(upstreamMove);

//	AMAction3 *downstreamMove = AMActionSupport::buildControlMoveAction(downstreamActuator_->filter(), downstreamFilterThicknessesMap_.value(setpoint));
//	action->addSubAction(downstreamMove);

//	return action;

	return 0;
}
