#include "BioXASCarbonFilterFarmActuatorControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"

BioXASCarbonFilterFarmActuatorControl::BioXASCarbonFilterFarmActuatorControl(const QString &name, const QString &units, QObject *parent) :
	BioXASCarbonFilterFarmControl(name, units, parent)
{
	// Initialize local variables.

	position_ = 0;
	status_ = 0;
}

BioXASCarbonFilterFarmActuatorControl::~BioXASCarbonFilterFarmActuatorControl()
{

}

bool BioXASCarbonFilterFarmActuatorControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = position_->canMeasure();

	return result;
}

bool BioXASCarbonFilterFarmActuatorControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = position_->canMove();

	return result;
}

bool BioXASCarbonFilterFarmActuatorControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = position_->canStop();

	return result;
}

void BioXASCarbonFilterFarmActuatorControl::setPositionControl(AMControl *newControl)
{
	if (position_ != newControl) {

		if (position_) {
			removeChildControl(position_);
		}

		position_ = newControl;

		if (position_) {
			addChildControl(position_);
		}

		updateStates();
	}
}

void BioXASCarbonFilterFarmActuatorControl::setStatusControl(AMControl *newControl)
{
	if (status_ != newControl) {

		if (status_) {
			removeChildControl(status_);
		}

		status_ = newControl;

		if (status_) {
			addChildControl(status_);
		}

		updateStates();
	}
}

void BioXASCarbonFilterFarmActuatorControl::updateConnected()
{
	bool isConnected = (
				position_ && position_->isConnected() &&
				status_ && status_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASCarbonFilterFarmActuatorControl::updateMoving()
{
	if (isConnected())
		setIsMoving(position_->isMoving());
}
