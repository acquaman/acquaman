#include "BioXASSideCarbonFilterFarm.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuator.h"

BioXASSideCarbonFilterFarm::BioXASSideCarbonFilterFarm(QObject *parent) :
	BioXASCarbonFilterFarm("SideCarbonFilterFarm", parent)
{
	// Setup the upstream actuator.

	BioXASCarbonFilterFarmActuator *actuator = new BioXASCarbonFilterFarmActuator(name()+"UpstreamActuator", this);

	CLSMAXvMotor *motor = new CLSMAXvMotor("SMTR1607-5-I00-01", "SMTR1607-5-I00-01", "SMTR1607-5-I00-01", true, 0.05, 2.0, this);
	actuator->setMotor(motor);

	AMReadOnlyPVControl *positionStatus = new AMReadOnlyPVControl(name()+"UpstreamActuatorStatus", "PFIL1607-5-I22-01:InPosition", this);
	actuator->setPositionStatus(positionStatus);

	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::None, BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT, BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT_MIN, BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT_MAX, 0);
	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::Bottom, BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM, BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM_MIN, BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM_MAX, 50);
	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::Top, BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP, BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP_MIN, BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP_MAX, 50);

	actuator->setWindowPreference(50, BioXASCarbonFilterFarmActuator::Window::Bottom); // There are multiple ways we can achieve a 50mm filter with this actuator--we set the window preference here.

	setUpstreamActuator(actuator);

	// Setup the downstream actuator.

	actuator = new BioXASCarbonFilterFarmActuator(name()+"DownstreamActuator", this);

	motor = new CLSMAXvMotor("SMTR1607-5-I00-02", "SMTR1607-5-I00-02", "SMTR1607-5-I00-02", true, 0.05, 2.0, this);
	actuator->setMotor(motor);

	positionStatus = new AMReadOnlyPVControl(name()+"DownstreamActuatorStatus", "PFIL1607-5-I22-02:InPosition", this);
	actuator->setPositionStatus(positionStatus);

	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::None, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT_MIN, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT_MAX, 0);
	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::Bottom, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM_MIN, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM_MAX, 700);
	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::Top, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP_MIN, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP_MAX, 0);

	actuator->setWindowPreference(0, BioXASCarbonFilterFarmActuator::Window::None); // There are multiple ways we can achieve a 0mm filter with this actuator--we set the window preference here.

	setDownstreamActuator(actuator);
}

BioXASSideCarbonFilterFarm::~BioXASSideCarbonFilterFarm()
{

}
