#include "BioXASMainCarbonFilterFarm.h"
#include "beamline/CLS/CLSMAXvMotor.h"

BioXASMainCarbonFilterFarm::BioXASMainCarbonFilterFarm(QObject *parent) :
	BioXASCarbonFilterFarm("MainCarbonFilterFarm", parent)
{
	// Setup the upstream actuator control.

	CLSMAXvMotor *motor = new CLSMAXvMotor("SMTR1607-5-I00-01", "SMTR1607-5-I00-01", "SMTR1607-5-I00-01", true, 0.05, 2.0, this);
	AMReadOnlyPVControl *motorStatus = new AMReadOnlyPVControl(name()+"UpstreamActuatorStatus", "PFIL1607-5-I22-01:InPosition", this);

	upstreamActuator_->position()->setPositionControl(motor);
	upstreamActuator_->position()->setStatusControl(motorStatus);

	upstreamActuator_->window()->addWindow(BioXASCarbonFilterFarmActuatorWindowControl::None, BIOXASMAINCARBONFILTERFARM_UPSTREAM_OUT, BIOXASMAINCARBONFILTERFARM_UPSTREAM_OUT - 0.25, BIOXASMAINCARBONFILTERFARM_UPSTREAM_OUT + 0.25);
	upstreamActuator_->window()->addWindow(BioXASCarbonFilterFarmActuatorWindowControl::Bottom, BIOXASMAINCARBONFILTERFARM_UPSTREAM_BOTTOM, BIOXASMAINCARBONFILTERFARM_UPSTREAM_BOTTOM - 0.25, BIOXASMAINCARBONFILTERFARM_UPSTREAM_BOTTOM + 0.25);
	upstreamActuator_->window()->addWindow(BioXASCarbonFilterFarmActuatorWindowControl::Top, BIOXASMAINCARBONFILTERFARM_UPSTREAM_TOP, BIOXASMAINCARBONFILTERFARM_UPSTREAM_TOP - 0.25, BIOXASMAINCARBONFILTERFARM_UPSTREAM_TOP + 0.25);

	upstreamActuator_->filter()->addFilter(BioXASCarbonFilterFarmActuatorWindowControl::None, 0);
	upstreamActuator_->filter()->addFilter(BioXASCarbonFilterFarmActuatorWindowControl::Bottom, 50);
	upstreamActuator_->filter()->addFilter(BioXASCarbonFilterFarmActuatorWindowControl::Top, 50);

	upstreamActuator_->filter()->setWindowPreference(50, BioXASCarbonFilterFarmActuatorWindowControl::Bottom); // There are multiple ways we can achieve a 50mm filter with this actuator--we set the window preference here.

	// Setup the downstream actuator control.

	motor = new CLSMAXvMotor("SMTR1607-5-I00-02", "SMTR1607-5-I00-02", "SMTR1607-5-I00-02", true, 0.05, 2.0, this);
	motorStatus = new AMReadOnlyPVControl(name()+"DownstreamActuatorStatus", "PFIL1607-5-I22-02:InPosition", this);

	downstreamActuator_->position()->setPositionControl(motor);
	downstreamActuator_->position()->setStatusControl(motorStatus);

	downstreamActuator_->window()->addWindow(BioXASCarbonFilterFarmActuatorWindowControl::None, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_OUT, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_OUT - 0.25, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_OUT + 0.25);
	downstreamActuator_->window()->addWindow(BioXASCarbonFilterFarmActuatorWindowControl::Bottom, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_BOTTOM, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_BOTTOM - 0.25, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_BOTTOM + 0.25);
	downstreamActuator_->window()->addWindow(BioXASCarbonFilterFarmActuatorWindowControl::Top, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_TOP, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_TOP - 0.25, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_TOP + 0.25);

	upstreamActuator_->filter()->addFilter(BioXASCarbonFilterFarmActuatorWindowControl::None, 0);
	upstreamActuator_->filter()->addFilter(BioXASCarbonFilterFarmActuatorWindowControl::Bottom, 700);
	upstreamActuator_->filter()->addFilter(BioXASCarbonFilterFarmActuatorWindowControl::Top, 0);

	downstreamActuator_->filter()->setWindowPreference(0, BioXASCarbonFilterFarmActuatorWindowControl::None); // There are multiple ways we can achieve a 0mm filter with this actuator--we set the window preference here.
}

BioXASMainCarbonFilterFarm::~BioXASMainCarbonFilterFarm()
{

}
