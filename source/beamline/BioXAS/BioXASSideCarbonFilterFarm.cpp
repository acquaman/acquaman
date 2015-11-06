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

	BioXASCarbonFilterFarmWindowOption *none = new BioXASCarbonFilterFarmWindowOption("None", AMNumber(BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT), AMNumber(BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT - 0.25), AMNumber(BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT + 0.25), AMNumber(0), this);
	BioXASCarbonFilterFarmWindowOption *bottom = new BioXASCarbonFilterFarmWindowOption("Bottom", AMNumber(BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM), AMNumber(BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM - 0.25), AMNumber(BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM + 0.25), AMNumber(50), this);
	BioXASCarbonFilterFarmWindowOption *top = new BioXASCarbonFilterFarmWindowOption("Top", AMNumber(BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP), AMNumber(BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP - 0.25), AMNumber(BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP + 0.25), AMNumber(50), this);

	upstreamActuator_->currentWindow()->addWindow(none);
	upstreamActuator_->currentWindow()->addWindow(bottom);
	upstreamActuator_->currentWindow()->addWindow(top);

	upstreamActuator_->currentFilter()->setFilterWindowPreference(50, bottom); // There are multiple ways we can achieve a 50mm filter with this actuator--we set the window preference here.

	// Setup the downstream actuator control.

	motor = new CLSMAXvMotor("SMTR1607-5-I00-02", "SMTR1607-5-I00-02", "SMTR1607-5-I00-02", true, 0.05, 2.0, this);
	motorStatus = new AMReadOnlyPVControl(name()+"DownstreamActuatorStatus", "PFIL1607-5-I22-02:InPosition", this);

	position = new BioXASCarbonFilterFarmActuatorPositionControl(name()+"DownstreamActuatorPosition", "mm", this);
	position->setPositionControl(motor);
	position->setStatusControl(motorStatus);

	downstreamActuator_->setCurrentPosition(position);

	none = new BioXASCarbonFilterFarmWindowOption("None", AMNumber(BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT), AMNumber(BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT - 0.25), AMNumber(BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT + 0.25), AMNumber(0), this);
	bottom = new BioXASCarbonFilterFarmWindowOption("Bottom", AMNumber(BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM), AMNumber(BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM - 0.25), AMNumber(BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM + 0.25), AMNumber(700), this);
	top = new BioXASCarbonFilterFarmWindowOption("Top", AMNumber(BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP), AMNumber(BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP - 0.25), AMNumber(BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP + 0.25), AMNumber(0), this);

	downstreamActuator_->currentWindow()->addWindow(none);
	downstreamActuator_->currentWindow()->addWindow(bottom);
	downstreamActuator_->currentWindow()->addWindow(top);

	downstreamActuator_->currentFilter()->setFilterWindowPreference(0, none); // There are multiple ways we can achieve a 0mm filter with this actuator--we set the window preference here.
}

BioXASSideCarbonFilterFarm::~BioXASSideCarbonFilterFarm()
{

}
