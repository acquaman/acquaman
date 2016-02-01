#include "BioXASImagingCarbonFilterFarm.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuator.h"

BioXASImagingCarbonFilterFarm::BioXASImagingCarbonFilterFarm(QObject *parent) :
	BioXASCarbonFilterFarm("BioXASImagingCarbonFilterFarm", parent)
{
	// Setup the upstream actuator.

	BioXASCarbonFilterFarmActuator *actuator = new BioXASCarbonFilterFarmActuator(QString("%1%2").arg(name()).arg("UpstreamActuator"), this);

	upstreamMotor_ = new CLSMAXvMotor("SMTR1607-5-I00-05", "SMTR1607-5-I00-05", "SMTR1607-5-I00-05", true, 0.05, 2.0, this);
	actuator->setMotor(upstreamMotor_);

	AMReadOnlyPVControl *positionStatus = new AMReadOnlyPVControl(QString("%1%2").arg(name()).arg("UpstreamActuatorStatus"), "PFIL1607-5-I10-01:InPosition", this);
	actuator->setPositionStatus(positionStatus);

	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::None, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_OUT, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_OUT_MIN, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_OUT_MAX, 0);
	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::Bottom, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_BOTTOM, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_BOTTOM_MIN, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_BOTTOM_MAX, 75);
	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::Top, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_TOP, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_TOP_MIN, BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_TOP_MAX, 75);

	actuator->setWindowPreference(75, BioXASCarbonFilterFarmActuator::Window::Bottom); // There are multiple ways we can achieve a 75mm filter with this actuator--we set the window preference here.

	setUpstreamActuator(actuator);

	// Setup the downstream actuator.

	actuator = new BioXASCarbonFilterFarmActuator(QString("%1%2").arg(name()).arg("DownstreamActuator"), this);

	downstreamMotor_ = new CLSMAXvMotor("SMTR1607-5-I00-06", "SMTR1607-5-I00-06", "SMTR1607-5-I00-06", true, 0.05, 2.0, this);
	actuator->setMotor(downstreamMotor_);

	positionStatus = new AMReadOnlyPVControl(QString("%1%2").arg(name()).arg("DownstreamActuatorStatus"), "PFIL1607-5-I10-02:InPosition", this);
	actuator->setPositionStatus(positionStatus);

	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::None, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_OUT, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_OUT_MIN, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_OUT_MAX, 0);
	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::Bottom, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_BOTTOM, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_BOTTOM_MIN, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_BOTTOM_MAX, 500);
	actuator->addWindow(BioXASCarbonFilterFarmActuator::Window::Top, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_TOP, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_TOP_MIN, BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_TOP_MAX, 0);

	actuator->setWindowPreference(0, BioXASCarbonFilterFarmActuator::Window::None); // There are multiple ways we can achieve a 0mm filter with this actuator--we set the window preference here.

	setDownstreamActuator(actuator);
}

BioXASImagingCarbonFilterFarm::~BioXASImagingCarbonFilterFarm()
{

}
