#include "BioXASCarbonFilterFarm.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmFilterThicknessControl.h"

BioXASCarbonFilterFarm::BioXASCarbonFilterFarm(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize local variables.

	upstreamPosition_ = 0;
	upstreamStatus_ = 0;
	upstreamActuator_ = 0;
	upstreamActuatorWindow_ = 0;
	upstreamActuatorFilterThickness_ = 0;

	downstreamPosition_ = 0;
	downstreamActuator_ = 0;
	downstreamStatus_ = 0;
	downstreamActuatorWindow_ = 0;
	downstreamActuatorFilterThickness_ = 0;

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

AMControl* BioXASCarbonFilterFarm::upstreamPositionControl()
{
	return upstreamPosition_;
}

AMControl* BioXASCarbonFilterFarm::upstreamStatusControl()
{
	return upstreamStatus_;
}

AMControl* BioXASCarbonFilterFarm::upstreamActuatorControl()
{
	return upstreamActuator_;
}

AMControl* BioXASCarbonFilterFarm::upstreamActuatorWindowControl()
{
	return upstreamActuatorWindow_;
}

AMControl* BioXASCarbonFilterFarm::upstreamActuatorFilterThicknessControl()
{
	return upstreamActuatorFilterThickness_;
}

AMControl* BioXASCarbonFilterFarm::downstreamPositionControl()
{
	return downstreamPosition_;
}

AMControl* BioXASCarbonFilterFarm::downstreamStatusControl()
{
	return downstreamStatus_;
}

AMControl* BioXASCarbonFilterFarm::downstreamActuatorControl()
{
	return downstreamActuator_;
}

AMControl* BioXASCarbonFilterFarm::downstreamActuatorWindowControl()
{
	return downstreamActuatorWindow_;
}

AMControl* BioXASCarbonFilterFarm::downstreamActuatorFilterThicknessControl()
{
	return downstreamActuatorFilterThickness_;
}

AMControl* BioXASCarbonFilterFarm::filterThicknessControl()
{
	return filterThickness_;
}
