#include "BioXASCarbonFilterFarm.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorWindowControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorFilterThicknessControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmFilterThicknessControl.h"

BioXASCarbonFilterFarm::BioXASCarbonFilterFarm(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize local variables.

	upstreamPosition_ = 0;
	upstreamStatus_ = 0;
	upstreamActuatorWindow_ = 0;
	upstreamActuatorFilterThicknessControl_ = 0;

	downstreamPosition_ = 0;
	downstreamStatus_ = 0;
	downstreamActuatorWindow_ = 0;
	downstreamActuatorFilterThicknessControl_ = 0;

	filterThickness_ = 0;
}

BioXASCarbonFilterFarm::~BioXASCarbonFilterFarm()
{

}

bool BioXASCarbonFilterFarm::isConnected() const
{
	bool connected = ( filterThickness_ && filterThickness_->isConnected() );

	return connected;
}
