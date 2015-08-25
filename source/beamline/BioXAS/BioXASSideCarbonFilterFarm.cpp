#include "BioXASSideCarbonFilterFarm.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorWindowControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorFilterThicknessControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmFilterThicknessControl.h"

BioXASSideCarbonFilterFarm::BioXASSideCarbonFilterFarm(QObject *parent) :
	BioXASCarbonFilterFarm("SideCarbonFilterFarm", parent)
{
	// Create components.

	upstreamPosition_ = new AMPVControl(name_+"UpstreamActuatorPosition", "SMTR1607-5-I00-01:mm:fbk", "SMTR1607-5-I00-01:mm:sp", "SMTR1607-5-I00-01:stop", this);
	upstreamStatus_ = new AMReadOnlyPVControl(name_+"UpstreamActuatorStatus", "SMTR1607-5-I00-01:inPosition", this);
	upstreamActuatorWindow_ = new BioXASCarbonFilterFarmActuatorWindowControl(name_+"UpstreamActuatorWindow", "", upstreamPosition_, upstreamStatus_, this);
	upstreamActuatorFilterThickness_ = new BioXASCarbonFilterFarmActuatorFilterThicknessControl(name_+"UpstreamActuatorFilterThickness", "mm", upstreamPosition_, upstreamStatus_, this);

	downstreamPosition_ = new AMPVControl(name_+"DownstreamActuatorPosition", "SMTR1607-5-I00-02:mm:fbk", "SMTR1607-5-I00-02:mm:sp", "SMTR1607-5-I00-02:stop", this);
	downstreamStatus_ = new AMReadOnlyPVControl(name_+"DownstreamActuatorStatus", "SMTR1607-5-I00-02:inPosition", this);
	upstreamActuatorWindow_ = new BioXASCarbonFilterFarmActuatorWindowControl(name_+"DownstreamActuatorWindow", "", upstreamPosition_, upstreamStatus_, this);
	upstreamActuatorFilterThickness_ = new BioXASCarbonFilterFarmActuatorFilterThicknessControl(name_+"DownstreamActuatorFilterThickness", "mm", downstreamPosition_, downstreamStatus_, this);

	filterThickness_ = new BioXASCarbonFilterFarmFilterThicknessControl(name_+"FilterThickness", "mm", upstreamActuatorFilterThickness_, downstreamActuatorFilterThickness_, this);

	// Make connections.

	connect( filter_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Current settings.

	upstreamActuatorWindow_->setWindowPosition(Actuator::None, BIOXASSIDECARBONFILTERFARM_UPSTREAM_OUT);
	upstreamActuator_->setWindowPosition(Actuator::Bottom, BIOXASSIDECARBONFILTERFARM_UPSTREAM_BOTTOM);
	upstreamActuator_->setWindowPosition(Actuator::Top, BIOXASSIDECARBONFILTERFARM_UPSTREAM_TOP);

	downstreamActuator_->setWindowPosition(Actuator::None, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_OUT);
	downstreamActuator_->setWindowPosition(Actuator::Bottom, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_BOTTOM);
	downstreamActuator_->setWindowPosition(Actuator::Top, BIOXASSIDECARBONFILTERFARM_DOWNSTREAM_TOP);

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
