#include "BioXASMainCarbonFilterFarm.h"
#include "beamline/CLS/CLSMAXvMotor.h"

BioXASMainCarbonFilterFarm::BioXASMainCarbonFilterFarm(QObject *parent) :
	BioXASCarbonFilterFarm("MainCarbonFilterFarm", parent)
{
	// Setup the upstream actuator control.

	CLSMAXvMotor *motor = new CLSMAXvMotor("SMTR1607-5-I00-03", "SMTR1607-5-I00-03", "SMTR1607-5-I00-03", true, 0.05, 2.0, this);
	AMReadOnlyPVControl *motorStatus = new AMReadOnlyPVControl(name()+"UpstreamStatus", "PFIL1607-5-I21-01:InPosition", this);

	BioXASCarbonFilterFarmActuatorPositionControl *position = new BioXASCarbonFilterFarmActuatorPositionControl(name()+"UpstreamActuatorPosition", "mm", this);
	position->setPositionControl(motor);
	position->setStatusControl(motorStatus);

	upstreamActuator_->setCurrentPosition(position);

	BioXASCarbonFilterFarmWindowOption *none = new BioXASCarbonFilterFarmWindowOption("None", AMNumber(BIOXASMAINCARBONFILTERFARM_UPSTREAM_OUT), AMNumber(BIOXASMAINCARBONFILTERFARM_UPSTREAM_OUT - 0.25), AMNumber(BIOXASMAINCARBONFILTERFARM_UPSTREAM_OUT + 0.25), AMNumber(0), this);
	BioXASCarbonFilterFarmWindowOption *bottom = new BioXASCarbonFilterFarmWindowOption("Bottom", AMNumber(BIOXASMAINCARBONFILTERFARM_UPSTREAM_BOTTOM), AMNumber(BIOXASMAINCARBONFILTERFARM_UPSTREAM_BOTTOM - 0.25), AMNumber(BIOXASMAINCARBONFILTERFARM_UPSTREAM_BOTTOM + 0.25), AMNumber(50), this);
	BioXASCarbonFilterFarmWindowOption *top = new BioXASCarbonFilterFarmWindowOption("Top", AMNumber(BIOXASMAINCARBONFILTERFARM_UPSTREAM_TOP), AMNumber(BIOXASMAINCARBONFILTERFARM_UPSTREAM_TOP - 0.25), AMNumber(BIOXASMAINCARBONFILTERFARM_UPSTREAM_TOP + 0.25), AMNumber(50), this);

	upstreamActuator_->currentWindow()->addWindow(none);
	upstreamActuator_->currentWindow()->addWindow(bottom);
	upstreamActuator_->currentWindow()->addWindow(top);

	upstreamActuator_->currentFilter()->setFilterWindowPreference(50, bottom); // There are multiple ways we can achieve a 50mm filter with this actuator--we set the window preference here.

	// Setup the downstream actuator control.

	motor = new CLSMAXvMotor("SMTR1607-5-I00-04", "SMTR1607-5-I00-04", "SMTR1607-5-I00-04", true, 0.05, 2.0, this);
	motorStatus = new AMReadOnlyPVControl(name()+"DownstreamActuatorStatus", "PFIL1607-5-I21-02:InPosition", this);

	position = new BioXASCarbonFilterFarmActuatorPositionControl(name()+"DownstreamActuatorPosition", "mm", this);
	position->setPositionControl(motor);
	position->setStatusControl(motorStatus);

	downstreamActuator_->setCurrentPosition(position);

	none = new BioXASCarbonFilterFarmWindowOption("None", AMNumber(BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_OUT), AMNumber(BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_OUT - 0.25), AMNumber(BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_OUT + 0.25), AMNumber(0), this);
	bottom = new BioXASCarbonFilterFarmWindowOption("Bottom", AMNumber(BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_BOTTOM), AMNumber(BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_BOTTOM - 0.25), AMNumber(BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_BOTTOM + 0.25), AMNumber(700), this);
	top = new BioXASCarbonFilterFarmWindowOption("Top", AMNumber(BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_TOP), AMNumber(BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_TOP - 0.25), AMNumber(BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_TOP + 0.25), AMNumber(0), this);

	downstreamActuator_->currentWindow()->addWindow(none);
	downstreamActuator_->currentWindow()->addWindow(bottom);
	downstreamActuator_->currentWindow()->addWindow(top);

	downstreamActuator_->currentFilter()->setFilterWindowPreference(0, none); // There are multiple ways we can achieve a 0mm filter with this actuator--we set the window preference here.
}

BioXASMainCarbonFilterFarm::~BioXASMainCarbonFilterFarm()
{

}
