#include "BioXASSideCarbonFilterFarm.h"
#include "beamline/CLS/CLSMAXvMotor.h"

BioXASSideCarbonFilterFarm::BioXASSideCarbonFilterFarm(QObject *parent) :
	BioXASCarbonFilterFarm("SideCarbonFilterFarm", parent)
{
	// Setup the upstream actuator control.

	CLSMAXvMotor *upstreamMotor = new CLSMAXvMotor("SMTR1607-5-I00-01", "SMTR1607-5-I00-01", "SMTR1607-5-I00-01", true, 0.05, 2.0, this);
	AMReadOnlyPVControl *upstreamStatus = new AMReadOnlyPVControl(name()+"UpstreamActuatorStatus", "SMTR1607-5-I00-01:InPosition", this);

	upstreamActuator_->position()->setPositionControl(upstreamMotor);
	upstreamActuator_->position()->setStatusControl(upstreamStatus);

	upstreamActuator_->window()->setWindowPosition(BioXASCarbonFilterFarmActuatorWindowControl::Top, BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP);
	upstreamActuator_->window()->setWindowPosition(BioXASCarbonFilterFarmActuatorWindowControl::Bottom, BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM);
	upstreamActuator_->window()->setWindowPosition(BioXASCarbonFilterFarmActuatorWindowControl::None, BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT);

	upstreamActuator_->filter()->setWindowFilter(BioXASCarbonFilterFarmActuatorWindowControl::Top, 50);
	upstreamActuator_->filter()->setWindowFilter(BioXASCarbonFilterFarmActuatorWindowControl::Bottom, 50);

	// Setup the downstream actuator control.

	CLSMAXvMotor *downstreamMotor = new CLSMAXvMotor("SMTR1607-5-I00-02", "SMTR1607-5-I00-02", "SMTR1607-5-I00-02", true, 0.05, 2.0, this);
	AMReadOnlyPVControl *downstreamStatus = new AMReadOnlyPVControl(name()+"DownstreamActuatorStatus", "SMTR1607-5-I00-02:InPosition", this);

	downstreamActuator_->position()->setPositionControl(downstreamMotor);
	downstreamActuator_->position()->setStatusControl(downstreamStatus);

	downstreamActuator_->window()->setWindowPosition(BioXASCarbonFilterFarmActuatorWindowControl::Top, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP);
	downstreamActuator_->window()->setWindowPosition(BioXASCarbonFilterFarmActuatorWindowControl::Bottom, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM);
	downstreamActuator_->window()->setWindowPosition(BioXASCarbonFilterFarmActuatorWindowControl::None, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT);

	downstreamActuator_->filter()->setWindowFilter(BioXASCarbonFilterFarmActuatorWindowControl::Top, 0);
	downstreamActuator_->filter()->setWindowFilter(BioXASCarbonFilterFarmActuatorWindowControl::Bottom, 700);

	// Setup the effective filter thickness control.

//	filterThickness_->setFilterThickness(0, 0, 0);
//	filterThickness_->setFilterThickness(50, 50, 0);
//	filterThickness_->setFilterThickness(700, 0, 700);
//	filterThickness_->setFilterThickness(750, 50, 700);
}

BioXASSideCarbonFilterFarm::~BioXASSideCarbonFilterFarm()
{

}
