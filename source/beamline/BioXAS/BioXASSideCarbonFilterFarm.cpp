#include "BioXASSideCarbonFilterFarm.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuator.h"

BioXASSideCarbonFilterFarm::BioXASSideCarbonFilterFarm(QObject *parent) :
	BioXASCarbonFilterFarm("SideCarbonFilterFarm", parent)
{
	// Setup the upstream actuator.

	setUpstreamActuatorMotor(new CLSMAXvMotor("SMTR1607-5-I00-01", "SMTR1607-5-I00-01", "SMTR1607-5-I00-01", true, 0.05, 2.0, this));
	setUpstreamActuatorPositionStatus(new AMReadOnlyPVControl(name()+"UpstreamActuatorStatus", "PFIL1607-5-I22-01:InPosition", this));

	addUpstreamActuatorWindow(Window::None, BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT, BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT_MIN, BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT_MAX, 0);
	addUpstreamActuatorWindow(Window::Bottom, BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM, BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM_MIN, BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM_MAX, 50);
	addUpstreamActuatorWindow(Window::Top, BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP, BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP_MIN, BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP_MAX, 50);

	setUpstreamActuatorWindowPreference(50, Window::Bottom); // There are multiple ways we can achieve a 50um filter with this actuator--we set the window preference here.

	// Setup the downstream actuator.

	setDownstreamActuatorMotor(new CLSMAXvMotor("SMTR1607-5-I00-02", "SMTR1607-5-I00-02", "SMTR1607-5-I00-02", true, 0.05, 2.0, this));
	setDownstreamActuatorPositionStatus(new AMReadOnlyPVControl(name()+"DownstreamActuatorStatus", "PFIL1607-5-I22-02:InPosition", this));

	addDownstreamActuatorWindow(Window::None, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT_MIN, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT_MAX, 0);
	addDownstreamActuatorWindow(Window::Bottom, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM_MIN, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM_MAX, 700);
	addDownstreamActuatorWindow(Window::Top, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP_MIN, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP_MAX, 0);

	setDownstreamActuatorWindowPreference(0, Window::None); // There are multiple ways we can achieve a 0um filter with this actuator--we set the window preference here.
}

BioXASSideCarbonFilterFarm::~BioXASSideCarbonFilterFarm()
{

}
