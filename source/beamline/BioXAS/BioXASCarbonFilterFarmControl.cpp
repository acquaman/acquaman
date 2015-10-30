#include "BioXASCarbonFilterFarmControl.h"

BioXASCarbonFilterFarmControl::BioXASCarbonFilterFarmControl(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize class variables.

	upstreamActuator_ = 0;
	downstreamActuator_ = 0;
	filter_ = 0;
}

BioXASCarbonFilterFarmControl::~BioXASCarbonFilterFarmControl()
{

}

bool BioXASCarbonFilterFarmControl::isConnected() const
{
	bool connected = (
				upstreamActuator_ && upstreamActuator_->isConnected() &&
				downstreamActuator_ && downstreamActuator_->isConnected() &&
				filter_ && filter_->isConnected()
				);

	return connected;
}

void BioXASCarbonFilterFarmControl::setUpstreamActuator(BioXASCarbonFilterFarmActuatorControl *newControl)
{
	if (upstreamActuator_ != newControl) {

		if (upstreamActuator_)
			disconnect( upstreamActuator_, 0, this, 0 );

		if (filter_)
			filter_->setUpstreamActuator(0);

		upstreamActuator_ = newControl;

		if (upstreamActuator_)
			connect( upstreamActuator_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		if (filter_)
			filter_->setUpstreamActuator(upstreamActuator_);

		updateConnected();

		emit upstreamActuatorChanged(upstreamActuator_);
	}
}

void BioXASCarbonFilterFarmControl::setDownstreamActuator(BioXASCarbonFilterFarmActuatorControl *newControl)
{
	if (downstreamActuator_ != newControl) {

		if (downstreamActuator_)
			disconnect( downstreamActuator_, 0, this, 0 );

		if (filter_)
			filter_->setDownstreamActuator(0);

		downstreamActuator_ = newControl;

		if (downstreamActuator_)
			connect( downstreamActuator_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		if (filter_)
			filter_->setDownstreamActuator(downstreamActuator_);

		updateConnected();

		emit downstreamActuatorChanged(downstreamActuator_);
	}
}

void BioXASCarbonFilterFarmControl::setFilter(BioXASCarbonFilterFarmFilterControl *newControl)
{
	if (filter_ != newControl) {

		if (filter_)
			disconnect( filter_, 0, this, 0 );

		filter_ = newControl;

		if (filter_) {
			connect( filter_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

			filter_->setUpstreamActuator(upstreamActuator_);
			filter_->setDownstreamActuator(downstreamActuator_);
		}

		updateConnected();

		emit filterChanged(filter_);
	}
}
