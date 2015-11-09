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

		upstreamActuator_ = newControl;

		if (upstreamActuator_)
			connect( upstreamActuator_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		updateConnected();
		onUpstreamActuatorFilterChanged();

		emit upstreamActuatorChanged(upstreamActuator_);
	}
}

void BioXASCarbonFilterFarmControl::setDownstreamActuator(BioXASCarbonFilterFarmActuatorControl *newControl)
{
	if (downstreamActuator_ != newControl) {

		if (downstreamActuator_)
			disconnect( downstreamActuator_, 0, this, 0 );

		downstreamActuator_ = newControl;

		if (downstreamActuator_)
			connect( downstreamActuator_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		updateConnected();
		onDownstreamActuatorFilterChanged();

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

			// Update the filter control with the latest upstream and downstream actuator
			// filter information.

			onUpstreamActuatorFilterChanged();
			onDownstreamActuatorFilterChanged();
		}

		updateConnected();

		emit filterChanged(filter_);
	}
}

void BioXASCarbonFilterFarmControl::onUpstreamActuatorFilterChanged()
{
	BioXASCarbonFilterFarmActuatorFilterControl *newControl = 0;

	if (upstreamActuator_)
		newControl = upstreamActuator_->currentFilter();

	if (filter_)
		filter_->setUpstreamFilter(newControl);
}

void BioXASCarbonFilterFarmControl::onDownstreamActuatorFilterChanged()
{
	BioXASCarbonFilterFarmActuatorFilterControl *newControl = 0;

	if (downstreamActuator_)
		newControl = downstreamActuator_->currentFilter();

	if (filter_)
		filter_->setDownstreamFilter(newControl);
}
