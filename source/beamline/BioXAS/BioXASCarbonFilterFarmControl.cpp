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
			removeChildControl(upstreamActuator_); // disconnects from all signals.

		upstreamActuator_ = newControl;

		if (upstreamActuator_) {
			addChildControl(upstreamActuator_);

			connect( upstreamActuator_, SIGNAL(filterChanged(AMControl*)), this, SLOT(updateFilter()) );
		}

		updateFilter();
		updateConnected();

		emit upstreamActuatorChanged(upstreamActuator_);
	}
}

void BioXASCarbonFilterFarmControl::setDownstreamActuator(BioXASCarbonFilterFarmActuatorControl *newControl)
{
	if (downstreamActuator_ != newControl) {

		if (downstreamActuator_)
			removeChildControl(downstreamActuator_); // disconnects from all signals.

		downstreamActuator_ = newControl;

		if (downstreamActuator_) {
			addChildControl(downstreamActuator_);

			connect( downstreamActuator_, SIGNAL(filterChanged(AMControl*)), this, SLOT(updateFilter()) );
		}

		updateFilter();
		updateConnected();

		emit downstreamActuatorChanged(downstreamActuator_);
	}
}

void BioXASCarbonFilterFarmControl::setFilter(BioXASCarbonFilterFarmFilterControl *newControl)
{
	if (filter_ != newControl) {

		if (filter_)
			removeChildControl(filter_); // disconnects from all signals.

		filter_ = newControl;

		if (filter_) {
			addChildControl(filter_);

			connect( filter_, SIGNAL(upstreamFilterChanged(BioXASCarbonFilterFarmActuatorFilterControl*)), this, SLOT(updateUpstreamFilter()) );
			connect( filter_, SIGNAL(downstreamFilterChanged(BioXASCarbonFilterFarmActuatorFilterControl*)), this, SLOT(updateDownstreamFilter()) );
		}

		updateFilter();
		updateConnected();

		emit filterChanged(filter_);
	}
}

void BioXASCarbonFilterFarmControl::updateUpstreamFilter()
{
	if (upstreamActuator_) {
		if (filter_)
			upstreamActuator_->setFilter(filter_->upstreamFilter());
		else
			upstreamActuator_->setFilter(0);
	}
}

void BioXASCarbonFilterFarmControl::updateDownstreamFilter()
{
	if (downstreamActuator_) {
		if (filter_)
			downstreamActuator_->setFilter(filter_->downstreamFilter());
		else
			downstreamActuator_->setFilter(0);
	}
}

void BioXASCarbonFilterFarmControl::updateFilter()
{
	BioXASCarbonFilterFarmActuatorFilterControl *upstreamFilter = 0;

	if (upstreamActuator_)
		upstreamFilter = upstreamActuator_->filter();

	BioXASCarbonFilterFarmActuatorFilterControl *downstreamFilter = 0;

	if (downstreamActuator_)
		downstreamFilter = downstreamActuator_->filter();

	if (filter_) {
		filter_->setUpstreamFilter(upstreamFilter);
		filter_->setDownstreamFilter(downstreamFilter);
	}
}
