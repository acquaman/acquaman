#include "BioXASMainCarbonFilterFarm.h"
#include "beamline/CLS/CLSMAXvMotor.h"

BioXASMainCarbonFilterFarm::BioXASMainCarbonFilterFarm(QObject *parent) :
	BioXASCarbonFilterFarm("MainCarbonFilterFarm", parent)
{
	// Setup the upstream actuator.

	BioXASCarbonFilterFarmActuator *actuator = new BioXASCarbonFilterFarmActuator(name()+"UpstreamActuator", this);

	CLSMAXvMotor *motor = new CLSMAXvMotor("SMTR1607-5-I00-03", "SMTR1607-5-I00-03", "SMTR1607-5-I00-03", true, 0.05, 2.0, this);
	actuator->setPositionControl(motor);

	AMReadOnlyPVControl *motorStatus = new AMReadOnlyPVControl(name()+"UpstreamActuatorStatus", "PFIL1607-5-I21-01:InPosition", this);
	actuator->setPositionStatusControl(motorStatus);

	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::None, BIOXASMAINCARBONFILTERFARM_UPSTREAM_OUT, BIOXASMAINCARBONFILTERFARM_UPSTREAM_OUT - 0.25, BIOXASMAINCARBONFILTERFARM_UPSTREAM_OUT + 0.25, 0);
	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::Bottom, BIOXASMAINCARBONFILTERFARM_UPSTREAM_BOTTOM, BIOXASMAINCARBONFILTERFARM_UPSTREAM_BOTTOM - 0.25, BIOXASMAINCARBONFILTERFARM_UPSTREAM_BOTTOM + 0.25, 50);
	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::Top, BIOXASMAINCARBONFILTERFARM_UPSTREAM_TOP, BIOXASMAINCARBONFILTERFARM_UPSTREAM_TOP - 0.25, BIOXASMAINCARBONFILTERFARM_UPSTREAM_TOP + 0.25, 50);

	actuator->setWindowPreference(50, BioXASCarbonFilterFarmActuator::Window::Bottom); // There are multiple ways we can achieve a 50mm filter with this actuator--we set the window preference here.

	setUpstreamActuator(actuator);

	// Setup the downstream actuator.

	actuator = new BioXASCarbonFilterFarmActuator(name()+"DownstreamActuator", this);

	motor = new CLSMAXvMotor("SMTR1607-5-I00-04", "SMTR1607-5-I00-04", "SMTR1607-5-I00-04", true, 0.05, 2.0, this);
	actuator->setPositionControl(motor);

	motorStatus = new AMReadOnlyPVControl(name()+"DownstreamActuatorStatus", "PFIL1607-5-I21-02:InPosition", this);
	actuator->setPositionStatusControl(motorStatus);

	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::None, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_OUT, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_OUT - 0.25, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_OUT + 0.25, 0);
	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::Bottom, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_BOTTOM, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_BOTTOM - 0.25, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_BOTTOM + 0.25, 700);
	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::Top, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_TOP, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_TOP - 0.25, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_TOP + 0.25, 0);

	actuator->setWindowPreference(0, BioXASCarbonFilterFarmActuator::Window::None); // There are multiple ways we can achieve a 0mm filter with this actuator--we set the window preference here.

	setDownstreamActuator(actuator);
}

BioXASMainCarbonFilterFarm::~BioXASMainCarbonFilterFarm()
{

}
