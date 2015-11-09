#include "BioXASCarbonFilterFarmActuatorControl.h"

BioXASCarbonFilterFarmActuatorControl::BioXASCarbonFilterFarmActuatorControl(const QString &actuatorName, QObject *parent) :
	BioXASBeamlineComponent(actuatorName, parent)
{
	// Initialize class varibles.

	position_ = 0;
	window_ = 0;
	filter_ = 0;
}

BioXASCarbonFilterFarmActuatorControl::~BioXASCarbonFilterFarmActuatorControl()
{

}

AMControl* BioXASCarbonFilterFarmActuatorControl::position() const
{
	AMControl *result = 0;

	if (position_)
		result = position_->positionControl();

	return result;
}

AMControl* BioXASCarbonFilterFarmActuatorControl::positionStatus() const
{
	AMControl *result = 0;

	if (position_)
		result = position_->statusControl();

	return result;
}

bool BioXASCarbonFilterFarmActuatorControl::isConnected() const
{
	bool connected = (
				position_ && position_->isConnected() &&
				window_ && window_->isConnected() &&
				filter_ && filter_->isConnected()
				);

	return connected;
}

void BioXASCarbonFilterFarmActuatorControl::setPositionControl(AMControl *newControl)
{
	if (position_)
		position_->setPositionControl(newControl);
}

void BioXASCarbonFilterFarmActuatorControl::setPositionStatusControl(AMControl *newControl)
{
	if (position_)
		position_->setStatusControl(newControl);
}

bool BioXASCarbonFilterFarmActuatorControl::setWindow(BioXASCarbonFilterFarmActuatorWindowControl *newControl)
{
	bool result = false;

	if (window_ != newControl) {

		if (window_)
			removeChildControl(window_); // disconnects from all signals.

		window_ = newControl;

		if (window_) {
			addChildControl(window_);

			connect( window_, SIGNAL(positionChanged(BioXASCarbonFilterFarmActuatorPositionControl*)), this, SLOT(updatePosition()) );
		}

		updateFilter();
		updateConnected();

		result = true;
	}

	return result;
}

bool BioXASCarbonFilterFarmActuatorControl::setFilter(BioXASCarbonFilterFarmActuatorFilterControl *newControl)
{
	bool result = false;

	if (filter_ != newControl) {

		if (filter_)
			removeChildControl(filter_); // disconnects from all signals.

		filter_ = newControl;

		if (filter_) {
			addChildControl(filter_);

			connect( filter_, SIGNAL(windowChanged(BioXASCarbonFilterFarmActuatorWindowControl*)), this, SLOT(updateWindow()) );
		}

		updateFilter();
		updateConnected();

		result = true;
	}

	return result;
}

bool BioXASCarbonFilterFarmActuatorControl::setPosition(BioXASCarbonFilterFarmActuatorPositionControl *newControl)
{
	bool result = false;

	if (position_ != newControl) {

		if (position_)
			removeChildControl(position_); // disconnects from all signals.

		position_ = newControl;

		if (position_) {
			addChildControl(position_);

			connect( position_, SIGNAL(positionControlChanged(AMPVControl*)), this, SIGNAL(positionChanged(AMPVControl*)) );
			connect( position_, SIGNAL(statusControlChanged(AMReadOnlyPVControl*)), this, SIGNAL(positionStatusChanged(AMReadOnlyPVControl*)) );
		}

		updateWindow();
		updateConnected();

		result = true;
	}

	return result;
}

void BioXASCarbonFilterFarmActuatorControl::updatePosition()
{
	if (window_)
		setPosition(window_->positionControl());
}

void BioXASCarbonFilterFarmActuatorControl::updateWindow()
{
	if (filter_)
		setWindow(filter_->windowControl());

	if (window_)
		window_->setPositionControl(position_);
}

void BioXASCarbonFilterFarmActuatorControl::updateFilter()
{
	if (filter_)
		filter_->setWindowControl(window_);
}
