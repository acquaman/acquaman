#include "BioXASCarbonFilterFarm.h"
#include "util/AMErrorMonitor.h"

BioXASCarbonFilterFarm::BioXASCarbonFilterFarm(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize local variables.

	upstreamPosition_ = 0;
	upstreamStatus_ = 0;
	upstreamActuator_ = 0;

	downstreamPosition_ = 0;
	downstreamStatus_ = 0;
	downstreamActuator_ = 0;

	filter_ = 0;
}

BioXASCarbonFilterFarm::~BioXASCarbonFilterFarm()
{

}

bool BioXASCarbonFilterFarm::isConnected() const
{
	bool connected = ( filter_ && filter_->isConnected() );

	return connected;
}
