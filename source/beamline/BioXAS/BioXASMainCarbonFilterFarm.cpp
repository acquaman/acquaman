#include "BioXASMainCarbonFilterFarm.h"
#include "beamline/CLS/CLSMAXvMotor.h"

BioXASMainCarbonFilterFarm::BioXASMainCarbonFilterFarm(QObject *parent) :
	BioXASCarbonFilterFarm("MainCarbonFilterFarm", parent)
{
	// Setup the upstream actuator.

	setUpstreamActuatorMotor(new CLSMAXvMotor("SMTR1607-5-I00-03", "SMTR1607-5-I00-03", "SMTR1607-5-I00-03", true, 0.05, 2.0, this));
	setUpstreamActuatorPositionStatus(new AMReadOnlyPVControl(name()+"UpstreamActuatorStatus", "PFIL1607-5-I21-01:InPosition", this));

	addUpstreamActuatorWindow(Window::None, BIOXASMAINCARBONFILTERFARM_UPSTREAM_OUT, BIOXASMAINCARBONFILTERFARM_UPSTREAM_OUT_MIN, BIOXASMAINCARBONFILTERFARM_UPSTREAM_OUT_MAX, 0);
	addUpstreamActuatorWindow(Window::Bottom, BIOXASMAINCARBONFILTERFARM_UPSTREAM_BOTTOM, BIOXASMAINCARBONFILTERFARM_UPSTREAM_BOTTOM_MIN, BIOXASMAINCARBONFILTERFARM_UPSTREAM_BOTTOM_MAX, 50);
	addUpstreamActuatorWindow(Window::Top, BIOXASMAINCARBONFILTERFARM_UPSTREAM_TOP, BIOXASMAINCARBONFILTERFARM_UPSTREAM_TOP_MIN, BIOXASMAINCARBONFILTERFARM_UPSTREAM_TOP_MAX, 50);

	setUpstreamActuatorWindowPreference(50, Window::Bottom); // There are multiple ways we can achieve a 50um filter with this actuator--we set the window preference here.

	// Setup the downstream actuator.

	setDownstreamActuatorMotor(new CLSMAXvMotor("SMTR1607-5-I00-04", "SMTR1607-5-I00-04", "SMTR1607-5-I00-04", true, 0.05, 2.0, this));
	setDownstreamActuatorPositionStatus(new AMReadOnlyPVControl(name()+"DownstreamActuatorStatus", "PFIL1607-5-I21-02:InPosition", this));

	addDownstreamActuatorWindow(Window::None, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_OUT, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_OUT_MIN, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_OUT_MAX, 0);
	addDownstreamActuatorWindow(Window::Bottom, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_BOTTOM, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_BOTTOM_MIN, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_BOTTOM_MAX, 700);
	addDownstreamActuatorWindow(Window::Top, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_TOP, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_TOP_MIN, BIOXASMAINCARBONFILTERFARM_DOWNSTREAM_TOP_MAX, 0);

	setDownstreamActuatorWindowPreference(0, Window::None); // There are multiple ways we can achieve a 0um filter with this actuator--we set the window preference here.
}

BioXASMainCarbonFilterFarm::~BioXASMainCarbonFilterFarm()
{

}
