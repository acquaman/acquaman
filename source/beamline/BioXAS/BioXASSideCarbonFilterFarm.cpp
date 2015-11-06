#include "BioXASSideCarbonFilterFarm.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmWindowOption.h"

BioXASSideCarbonFilterFarm::BioXASSideCarbonFilterFarm(QObject *parent) :
	BioXASCarbonFilterFarm("SideCarbonFilterFarm", parent)
{
	// Setup the upstream actuator control.

	CLSMAXvMotor *motor = new CLSMAXvMotor("SMTR1607-5-I00-01", "SMTR1607-5-I00-01", "SMTR1607-5-I00-01", true, 0.05, 2.0, this);
	AMReadOnlyPVControl *motorStatus = new AMReadOnlyPVControl(name()+"UpstreamActuatorStatus", "PFIL1607-5-I22-01:InPosition", this);

	BioXASCarbonFilterFarmActuatorPositionControl *position = new BioXASCarbonFilterFarmActuatorPositionControl(name()+"UpstreamActuatorPosition", "mm", this);
	position->setPositionControl(motor);
	position->setStatusControl(motorStatus);

	upstreamActuator_->setCurrentPosition(position);

	upstreamActuator_->currentWindow()->addOption(0, "None", BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT, BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT - 0.25, BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT + 0.25);
	upstreamActuator_->currentWindow()->addOption(1, "Bottom", BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM, BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM - 0.25, BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM + 0.25);
	upstreamActuator_->currentWindow()->addOption(2, "Top", BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP, BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP - 0.25, BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP + 0.25);

//	upstreamActuator_->currentFilter()->setFilterWindowPreference(50, bottom); // There are multiple ways we can achieve a 50mm filter with this actuator--we set the window preference here.

	// Setup the downstream actuator control.

	motor = new CLSMAXvMotor("SMTR1607-5-I00-02", "SMTR1607-5-I00-02", "SMTR1607-5-I00-02", true, 0.05, 2.0, this);
	motorStatus = new AMReadOnlyPVControl(name()+"DownstreamActuatorStatus", "PFIL1607-5-I22-02:InPosition", this);

	position = new BioXASCarbonFilterFarmActuatorPositionControl(name()+"DownstreamActuatorPosition", "mm", this);
	position->setPositionControl(motor);
	position->setStatusControl(motorStatus);

	downstreamActuator_->setCurrentPosition(position);

	downstreamActuator_->currentWindow()->addOption(0, "None", BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT - 0.25, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT + 0.25);
	downstreamActuator_->currentWindow()->addOption(1, "Bottom", BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM - 0.25, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM + 0.25);
	downstreamActuator_->currentWindow()->addOption(2, "Top", BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP - 0.25, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP + 0.25);


//	downstreamActuator_->currentFilter()->setFilterWindowPreference(0, none); // There are multiple ways we can achieve a 0mm filter with this actuator--we set the window preference here.
}

BioXASSideCarbonFilterFarm::~BioXASSideCarbonFilterFarm()
{

}
