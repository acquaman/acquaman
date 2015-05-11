#include "BioXASSideCarbonFilterFarm.h"

BioXASSideCarbonFilterFarm::BioXASSideCarbonFilterFarm(QObject *parent) :
	BioXASCarbonFilterFarm("BioXASSideCarbonFilterFarm", parent)
{
	// Create components.

	upstreamPosition_ = new AMPVControl(name_+"UpstreamPosition", "SMTR1607-5-I00-01:mm:fbk", "SMTR1607-5-I00-01:mm:sp", "SMTR1607-5-I00-01:stop", this);
	upstreamStatus_ = new AMReadOnlyPVControl(name_+"UpstreamStatus", "SMTR1607-5-I00-01:inPosition", this);
	upstreamActuator_ = new BioXASCarbonFilterFarmActuatorControl(upstreamPosition_, upstreamStatus_, this);

	downstreamPosition_ = new AMPVControl(name_+"DownstreamPosition", "SMTR1607-5-I00-02:mm:fbk", "SMTR1607-5-I00-02:mm:sp", "SMTR1607-5-I00-02:stop", this);
	downstreamStatus_ = new AMReadOnlyPVControl(name_+"DownstreamStatus", "SMTR1607-5-I00-02:inPosition", this);
	downstreamActuator_ = new BioXASCarbonFilterFarmActuatorControl(downstreamPosition_, downstreamStatus_, this);

	filter_ = new BioXASSideCarbonFilterFarmControl(upstreamActuator_, downstreamActuator_, this);

	// Make connections.

	connect( filter_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Current settings.

	upstreamActuator_->setWindowPosition(Window::None, BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT);
	upstreamActuator_->setWindowPosition(Window::Bottom, BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM);
	upstreamActuator_->setWindowPosition(Window::Top, BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP);

	downstreamActuator_->setWindowPosition(Window::None, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT);
	downstreamActuator_->setWindowPosition(Window::Bottom, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM);
	downstreamActuator_->setWindowPosition(Window::Top, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP);

	filter_->setWindowFilter(Actuator::Upstream, Window::Bottom, Filter::Fifty);
	filter_->setWindowFilter(Actuator::Upstream, Window::Top, Filter::Fifty);
	filter_->setWindowFilter(Actuator::Downstream, Window::Bottom, Filter::None);
	filter_->setWindowFilter(Actuator::Downstream, Window::Top, Filter::SevenHundred);

	updateConnected();
}

BioXASSideCarbonFilterFarm::~BioXASSideCarbonFilterFarm()
{

}
