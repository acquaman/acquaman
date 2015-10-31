#include "BioXASMainCarbonFilterFarm.h"
#include "beamline/CLS/CLSMAXvMotor.h"

BioXASMainCarbonFilterFarm::BioXASMainCarbonFilterFarm(QObject *parent) :
	BioXASCarbonFilterFarm("MainCarbonFilterFarm", parent)
{
	// Setup the upstream actuator control.

	CLSMAXvMotor *upstreamMotor = new CLSMAXvMotor("SMTR1607-5-I00-03", "SMTR1607-5-I00-03", "SMTR1607-5-I00-03", true, 0.05, 2.0, this);
	AMReadOnlyPVControl *upstreamStatus = new AMReadOnlyPVControl(name()+"UpstreamStatus", "PFIL1607-5-I21-01:InPosition", this);

	upstreamActuator_->position()->setPositionControl(upstreamMotor);
	upstreamActuator_->position()->setStatusControl(upstreamStatus);

	upstreamActuator_->window()->setWindowPosition(BioXASCarbonFilterFarmActuatorWindowControl::Top, BIOXASMAINCARBONFILTERFARM_UPSTREAM_TOP);
	upstreamActuator_->window()->setWindowPosition(BioXASCarbonFilterFarmActuatorWindowControl::Bottom, BIOXASMAINCARBONFILTERFARM_UPSTREAM_BOTTOM);
	upstreamActuator_->window()->setWindowPosition(BioXASCarbonFilterFarmActuatorWindowControl::None, BIOXASMAINCARBONFILTERFARM_UPSTREAM_OUT);

	upstreamActuator_->filter()->setWindowFilter(BioXASCarbonFilterFarmActuatorWindowControl::Top, 50);
	upstreamActuator_->filter()->setWindowFilter(BioXASCarbonFilterFarmActuatorWindowControl::Bottom, 50);

	// Setup the downstream actuator control.

	CLSMAXvMotor *downstreamMotor = new CLSMAXvMotor("SMTR1607-5-I00-04", "SMTR1607-5-I00-04", "SMTR1607-5-I00-04", true, 0.05, 2.0, this);
	AMReadOnlyPVControl *downstreamStatus = new AMReadOnlyPVControl(name()+"DownstreamActuatorStatus", "PFIL1607-5-I21-02:InPosition", this);

	downstreamActuator_->position()->setPositionControl(downstreamMotor);
	downstreamActuator_->position()->setStatusControl(downstreamStatus);

	downstreamActuator_->window()->setWindowPosition(BioXASCarbonFilterFarmActuatorWindowControl::Top, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_TOP);
	downstreamActuator_->window()->setWindowPosition(BioXASCarbonFilterFarmActuatorWindowControl::Bottom, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_BOTTOM);
	downstreamActuator_->window()->setWindowPosition(BioXASCarbonFilterFarmActuatorWindowControl::None, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_OUT);

	downstreamActuator_->filter()->setWindowFilter(BioXASCarbonFilterFarmActuatorWindowControl::Top, 0);
	downstreamActuator_->filter()->setWindowFilter(BioXASCarbonFilterFarmActuatorWindowControl::Bottom, 700);

	// Setup the effective filter control.

//	filterThickness_->setFilterThickness(0, 0, 0);
//	filterThickness_->setFilterThickness(50, 50, 0);
//	filterThickness_->setFilterThickness(700, 0, 700);
//	filterThickness_->setFilterThickness(750, 50, 700);
}

BioXASMainCarbonFilterFarm::~BioXASMainCarbonFilterFarm()
{

}
