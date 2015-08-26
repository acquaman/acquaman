#include "BioXASSideCarbonFilterFarm.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorWindowControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmFilterThicknessControl.h"

BioXASSideCarbonFilterFarm::BioXASSideCarbonFilterFarm(QObject *parent) :
	BioXASCarbonFilterFarm("SideCarbonFilterFarm", parent)
{
	// Create components.

	upstreamPosition_ = new AMPVControl(name_+"UpstreamActuatorPosition", "SMTR1607-5-I00-01:mm:fbk", "SMTR1607-5-I00-01:mm:sp", "SMTR1607-5-I00-01:stop", this);
	upstreamStatus_ = new AMReadOnlyPVControl(name_+"UpstreamActuatorStatus", "SMTR1607-5-I00-01:inPosition", this);
	upstreamActuator_ = new BioXASCarbonFilterFarmActuatorControl(name_+"UpstreamActuator", "mm", upstreamPosition_, upstreamStatus_, this);
	upstreamActuatorWindow_ = new BioXASCarbonFilterFarmActuatorWindowControl(name_+"UpstreamActuatorWindow", "", upstreamActuator_, this);
	upstreamActuatorFilterThickness_ = new BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl(name_+"UpstreamActuatorFilterThickness", "mm", upstreamActuatorWindow_, this);

	downstreamPosition_ = new AMPVControl(name_+"DownstreamActuatorPosition", "SMTR1607-5-I00-02:mm:fbk", "SMTR1607-5-I00-02:mm:sp", "SMTR1607-5-I00-02:stop", this);
	downstreamStatus_ = new AMReadOnlyPVControl(name_+"DownstreamActuatorStatus", "SMTR1607-5-I00-02:inPosition", this);
	downstreamActuator_ = new BioXASCarbonFilterFarmActuatorControl(name_+"DownstreamActuator", "mm", downstreamPosition_, downstreamStatus_, this);
	downstreamActuatorWindow_ = new BioXASCarbonFilterFarmActuatorWindowControl(name_+"DownstreamActuatorWindow", "", downstreamActuator_, this);
	downstreamActuatorFilterThickness_ = new BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl(name_+"DownstreamActuatorFilterThickness", "mm", downstreamActuatorWindow_, this);

	filterThickness_ = new BioXASCarbonFilterFarmFilterThicknessControl(name_+"FilterThickness", "mm", upstreamActuatorFilterThickness_, downstreamActuatorFilterThickness_, this);

	// Make connections.

	connect( filterThickness_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Current settings.

	upstreamActuatorWindow_->setWindowPosition(Actuator::None, BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT);
	upstreamActuatorWindow_->setWindowPosition(Actuator::Bottom, BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM);
	upstreamActuatorWindow_->setWindowPosition(Actuator::Top, BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP);

	downstreamActuatorWindow_->setWindowPosition(Actuator::None, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT);
	downstreamActuatorWindow_->setWindowPosition(Actuator::Bottom, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM);
	downstreamActuatorWindow_->setWindowPosition(Actuator::Top, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP);

	upstreamActuatorFilterThickness_->setWindowFilterThickness(Actuator::Bottom, 50);
	upstreamActuatorFilterThickness_->setWindowFilterThickness(Actuator::Top, 50);

	downstreamActuatorFilterThickness_->setWindowFilterThickness(Actuator::Bottom, 700);
	downstreamActuatorFilterThickness_->setWindowFilterThickness(Actuator::Top, 0);

	filterThickness_->setFilterThickness(0, 0, 0);
	filterThickness_->setFilterThickness(50, 50, 0);
	filterThickness_->setFilterThickness(700, 0, 700);
	filterThickness_->setFilterThickness(750, 50, 700);

	updateConnected();
}

BioXASSideCarbonFilterFarm::~BioXASSideCarbonFilterFarm()
{

}
