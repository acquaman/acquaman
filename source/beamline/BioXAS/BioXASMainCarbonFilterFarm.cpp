#include "BioXASMainCarbonFilterFarm.h"
#include "beamline/CLS/CLSMAXvMotor.h"

BioXASMainCarbonFilterFarm::BioXASMainCarbonFilterFarm(QObject *parent) :
	BioXASCarbonFilterFarm("MainCarbonFilterFarm", parent)
{
	// Setup the upstream actuator.

	BioXASCarbonFilterFarmActuator *actuator = new BioXASCarbonFilterFarmActuator(name()+"UpstreamActuator", this);

	CLSMAXvMotor *motor = new CLSMAXvMotor("SMTR1607-5-I00-03", "SMTR1607-5-I00-03", "SMTR1607-5-I00-03", true, 0.05, 2.0, this);
	actuator->setMotor(motor);

	AMReadOnlyPVControl *positionStatus = new AMReadOnlyPVControl(name()+"UpstreamActuatorStatus", "PFIL1607-5-I21-01:InPosition", this);
	actuator->setPositionStatus(positionStatus);

	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::None, BIOXASMAINCARBONFILTERFARM_UPSTREAM_OUT, BIOXASMAINCARBONFILTERFARM_UPSTREAM_OUT_MIN, BIOXASMAINCARBONFILTERFARM_UPSTREAM_OUT_MAX, 0);
	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::Bottom, BIOXASMAINCARBONFILTERFARM_UPSTREAM_BOTTOM, BIOXASMAINCARBONFILTERFARM_UPSTREAM_BOTTOM_MIN, BIOXASMAINCARBONFILTERFARM_UPSTREAM_BOTTOM_MAX, 50);
	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::Top, BIOXASMAINCARBONFILTERFARM_UPSTREAM_TOP, BIOXASMAINCARBONFILTERFARM_UPSTREAM_TOP_MIN, BIOXASMAINCARBONFILTERFARM_UPSTREAM_TOP_MAX, 50);

	actuator->setWindowPreference(50, BioXASCarbonFilterFarmActuator::Window::Bottom); // There are multiple ways we can achieve a 50mm filter with this actuator--we set the window preference here.

	setUpstreamActuator(actuator);

	// Setup the downstream actuator.

	actuator = new BioXASCarbonFilterFarmActuator(name()+"DownstreamActuator", this);

	motor = new CLSMAXvMotor("SMTR1607-5-I00-04", "SMTR1607-5-I00-04", "SMTR1607-5-I00-04", true, 0.05, 2.0, this);
	actuator->setMotor(motor);

	positionStatus = new AMReadOnlyPVControl(name()+"DownstreamActuatorStatus", "PFIL1607-5-I21-02:InPosition", this);
	actuator->setPositionStatus(positionStatus);

	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::None, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_OUT, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_OUT_MIN, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_OUT_MAX, 0);
	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::Bottom, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_BOTTOM, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_BOTTOM_MIN, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_BOTTOM_MAX, 700);
	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::Top, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_TOP, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_TOP_MIN, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_TOP_MAX, 0);

	actuator->setWindowPreference(0, BioXASCarbonFilterFarmActuator::Window::None); // There are multiple ways we can achieve a 0mm filter with this actuator--we set the window preference here.

	setDownstreamActuator(actuator);
}

BioXASMainCarbonFilterFarm::~BioXASMainCarbonFilterFarm()
{

}
