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

bool BioXASCarbonFilterFarmActuatorControl::isConnected() const
{
	bool connected = (
				position_ && position_->isConnected() &&
				window_ && window_->isConnected() &&
				filter_ && filter_->isConnected()
				);

	return connected;
}

void BioXASCarbonFilterFarmActuatorControl::setPosition(BioXASCarbonFilterFarmActuatorPositionControl *newControl)
{
	if (position_ != newControl) {

		if (position_) {
			removeChildControl(position_); // disconnects from all signals.

			if (window_)
				window_->setPositionControl(0);
		}

		position_ = newControl;

		if (position_)
			addChildControl(position_);

		updateWindow();
		updateConnected();

		emit positionChanged(position_);
	}
}

void BioXASCarbonFilterFarmActuatorControl::setWindow(BioXASCarbonFilterFarmActuatorWindowControl *newControl)
{
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

		emit windowChanged(window_);
	}
}

void BioXASCarbonFilterFarmActuatorControl::setFilter(BioXASCarbonFilterFarmActuatorFilterControl *newControl)
{
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

		emit filterChanged(filter_);
	}
}

void BioXASCarbonFilterFarmActuatorControl::updatePosition()
{
	if (window_ && window_->positionControl() != position_)
		setPosition(window_->positionControl());
}

void BioXASCarbonFilterFarmActuatorControl::updateWindow()
{
	if (filter_ && filter_->windowControl() != window_)
		setWindow(filter_->windowControl());

	if (window_ && window_->positionControl() != position_)
		window_->setPositionControl(position_);
}

void BioXASCarbonFilterFarmActuatorControl::updateFilter()
{
	if (filter_ && filter_->windowControl() != window_)
		filter_->setWindowControl(window_);
}
