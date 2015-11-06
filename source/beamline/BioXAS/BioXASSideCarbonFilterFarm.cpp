#include "BioXASSideCarbonFilterFarm.h"
#include "beamline/CLS/CLSMAXvMotor.h"

BioXASSideCarbonFilterFarm::BioXASSideCarbonFilterFarm(QObject *parent) :
	BioXASCarbonFilterFarm("SideCarbonFilterFarm", parent)
{
	// Setup the upstream actuator control.

	CLSMAXvMotor *motor = new CLSMAXvMotor("SMTR1607-5-I00-01", "SMTR1607-5-I00-01", "SMTR1607-5-I00-01", true, 0.05, 2.0, this);
	AMReadOnlyPVControl *motorStatus = new AMReadOnlyPVControl(name()+"UpstreamActuatorStatus", "PFIL1607-5-I22-01:InPosition", this);

	upstreamActuator_->position()->setPositionControl(motor);
	upstreamActuator_->position()->setStatusControl(motorStatus);

	upstreamActuator_->window()->addWindow(BioXASCarbonFilterFarmActuatorWindowControl::None, BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT, BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT - 0.25, BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT + 0.25);
	upstreamActuator_->window()->addWindow(BioXASCarbonFilterFarmActuatorWindowControl::Bottom, BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM, BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM - 0.25, BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM + 0.25);
	upstreamActuator_->window()->addWindow(BioXASCarbonFilterFarmActuatorWindowControl::Top, BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP, BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP - 0.25, BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP + 0.25);

	upstreamActuator_->filter()->addFilter(BioXASCarbonFilterFarmActuatorWindowControl::None, 0);
	upstreamActuator_->filter()->addFilter(BioXASCarbonFilterFarmActuatorWindowControl::Bottom, 50);
	upstreamActuator_->filter()->addFilter(BioXASCarbonFilterFarmActuatorWindowControl::Top, 50);

	upstreamActuator_->filter()->setWindowPreference(50, BioXASCarbonFilterFarmActuatorWindowControl::Bottom); // There are multiple ways we can achieve a 50mm filter with this actuator--we set the window preference here.

	// Setup the downstream actuator control.

	motor = new CLSMAXvMotor("SMTR1607-5-I00-02", "SMTR1607-5-I00-02", "SMTR1607-5-I00-02", true, 0.05, 2.0, this);
	motorStatus = new AMReadOnlyPVControl(name()+"DownstreamActuatorStatus", "PFIL1607-5-I22-02:InPosition", this);

	downstreamActuator_->position()->setPositionControl(motor);
	downstreamActuator_->position()->setStatusControl(motorStatus);

	downstreamActuator_->window()->addWindow(BioXASCarbonFilterFarmActuatorWindowControl::None, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT - 0.25, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT + 0.25);
	downstreamActuator_->window()->addWindow(BioXASCarbonFilterFarmActuatorWindowControl::Bottom, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM - 0.25, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM + 0.25);
	downstreamActuator_->window()->addWindow(BioXASCarbonFilterFarmActuatorWindowControl::Top, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP - 0.25, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP + 0.25);

	upstreamActuator_->filter()->addFilter(BioXASCarbonFilterFarmActuatorWindowControl::None, 0);
	upstreamActuator_->filter()->addFilter(BioXASCarbonFilterFarmActuatorWindowControl::Bottom, 700);
	upstreamActuator_->filter()->addFilter(BioXASCarbonFilterFarmActuatorWindowControl::Top, 0);

	downstreamActuator_->filter()->setWindowPreference(0, BioXASCarbonFilterFarmActuatorWindowControl::None); // There are multiple ways we can achieve a 0mm filter with this actuator--we set the window preference here.
}

BioXASSideCarbonFilterFarm::~BioXASSideCarbonFilterFarm()
{

}
