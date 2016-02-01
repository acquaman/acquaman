#include "BioXASCarbonFilterFarm.h"

BioXASCarbonFilterFarm::BioXASCarbonFilterFarm(const QString &deviceName, QObject *parent) :
	BioXASBeamlineComponent(deviceName, parent)
{
	// Create upstream actuator.

	upstreamActuator_ = new BioXASCarbonFilterFarmActuator(QString("%1%2").arg(deviceName).arg("UpstreamActuator"), this);
	addChildControl(upstreamActuator_);

	// Create downstream actuator.

	downstreamActuator_ = new BioXASCarbonFilterFarmActuator(QString("%1%2").arg(deviceName).arg("DownstreamActuator"), this);
	addChildControl(downstreamActuator_);

	// Create filter control.

	filter_ = new BioXASCarbonFilterFarmFilterControl(QString("%1%2").arg(name()).arg("Filter"), "um", this);
	addChildControl(filter_);

	filter_->setUpstreamFilter(upstreamActuator_->filter());
	filter_->setDownstreamFilter(downstreamActuator_->filter());
}

BioXASCarbonFilterFarm::~BioXASCarbonFilterFarm()
{

}
