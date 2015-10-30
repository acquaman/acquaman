#include "BioXASCarbonFilterFarmActuatorControl.h"

BioXASCarbonFilterFarmActuatorControl::BioXASCarbonFilterFarmActuatorControl(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
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

		if (position_)
			disconnect( position_, 0, this, 0 );

		if (window_)
			window_->setActuatorPosition(0);

		position_ = newControl;

		if (position_)
			connect( position_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		if (window_)
			window_->setActuatorPosition(position_);

		updateConnected();

		emit positionChanged(position_);
	}
}

void BioXASCarbonFilterFarmActuatorControl::setWindow(BioXASCarbonFilterFarmActuatorWindowControl *newControl)
{
	if (window_ != newControl) {

		if (window_)
			disconnect( window_, 0, this, 0 );

		if (filter_)
			filter_->setWindow(0);

		window_ = newControl;

		if (window_) {
			connect( window_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

			window_->setActuatorPosition(position_);
		}

		if (filter_)
			filter_->setWindow(window_);

		updateConnected();

		emit windowChanged(window_);
	}
}

void BioXASCarbonFilterFarmActuatorControl::setFilter(BioXASCarbonFilterFarmActuatorFilterControl *newControl)
{
	if (filter_ != newControl) {

		if (filter_)
			disconnect( filter_, 0, this, 0 );

		filter_ = newControl;

		if (filter_) {
			connect( filter_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

			filter_->setWindow(window_);
		}

		updateConnected();

		emit filterChanged(filter_);
	}
}
