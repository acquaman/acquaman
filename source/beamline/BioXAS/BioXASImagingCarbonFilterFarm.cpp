#include "BioXASImagingCarbonFilterFarm.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuator.h"

BioXASImagingCarbonFilterFarm::BioXASImagingCarbonFilterFarm(QObject *parent) :
	BioXASCarbonFilterFarm("BioXASImagingCarbonFilterFarm", parent)
{
	// Setup the upstream actuator.

	setUpstreamActuatorMotor(new CLSMAXvMotor("SMTR1607-5-I00-05", "SMTR1607-5-I00-05", "SMTR1607-5-I00-05", true, 0.05, 2.0, this));
	setUpstreamActuatorPositionStatus(new AMReadOnlyPVControl(QString("%1%2").arg(name()).arg("UpstreamActuatorStatus"), "PFIL1607-5-I10-01:InPosition", this));

	addUpstreamActuatorWindow(Window::None, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_OUT, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_OUT_MIN, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_OUT_MAX, 0);
	addUpstreamActuatorWindow(Window::Bottom, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_BOTTOM, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_BOTTOM_MIN, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_BOTTOM_MAX, 75);
	addUpstreamActuatorWindow(Window::Top, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_TOP, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_TOP_MIN, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_TOP_MAX, 75);

	setUpstreamActuatorWindowPreference(75, Window::Bottom); // There are multiple ways we can achieve a 75um filter with this actuator--we set the window preference here.

	// Setup the downstream actuator.

	setDownstreamActuatorMotor(new CLSMAXvMotor("SMTR1607-5-I00-06", "SMTR1607-5-I00-06", "SMTR1607-5-I00-06", true, 0.05, 2.0, this));
	setDownstreamActuatorPositionStatus(new AMReadOnlyPVControl(QString("%1%2").arg(name()).arg("DownstreamActuatorStatus"), "PFIL1607-5-I10-02:InPosition", this));

	addDownstreamActuatorWindow(Window::None, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_OUT, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_OUT_MIN, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_OUT_MAX, 0);
	addDownstreamActuatorWindow(Window::Bottom, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_BOTTOM, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_BOTTOM_MIN, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_BOTTOM_MAX, 500);
	addDownstreamActuatorWindow(Window::Top, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_TOP, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_TOP_MIN, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_TOP_MAX, 0);

	setDownstreamActuatorWindowPreference(0, Window::None); // There are multiple ways we can achieve a 0um filter with this actuator--we set the window preference here.
}

BioXASImagingCarbonFilterFarm::~BioXASImagingCarbonFilterFarm()
{

}
