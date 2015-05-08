#include "BioXASCarbonFilterFarm.h"
#include "util/AMErrorMonitor.h"

BioXASCarbonFilterFarm::BioXASCarbonFilterFarm(const QString &name, const QString &upstreamPVBase, const QString &downstreamPVBase, QObject *parent) :
	QObject(parent)
{
	// Initialize local variables.

	name_ = name;
	connected_ = false;

	filter_ = 0;

	upstreamActuator_ = 0;
	upstreamPosition_ = 0;
	upstreamStatus_ = 0;

	downstreamActuator_ = 0;
	downstreamPosition_ = 0;
	downstreamStatus_ = 0;

	// Create components.

	upstreamPosition_ = new AMPVControl(name+"UpstreamPosition", upstreamPVBase+":mm:fbk", upstreamPVBase+":mm:sp", upstreamPVBase+":stop", this);
	upstreamStatus_ = new AMReadOnlyPVControl(name+"UpstreamStatus", upstreamPVBase+":inPosition", this);
	upstreamActuator_ = new BioXASCarbonFilterFarmActuatorControl(upstreamPosition_, upstreamStatus_, this);

	downstreamPosition_ = new AMPVControl(name+"DownstreamPosition", downstreamPVBase+":mm:fbk", downstreamPVBase+":mm:sp", downstreamPVBase+":stop", this);
	downstreamStatus_ = new AMReadOnlyPVControl(name+"DownstreamStatus", downstreamPVBase+":inPosition", this);
	downstreamActuator_ = new BioXASCarbonFilterFarmActuatorControl(downstreamPosition_, downstreamStatus_, this);

	filter_ = new BioXASCarbonFilterFarmControl(upstreamActuator_, downstreamActuator_, this);

	// Make connections.

	connect( filter_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
}

BioXASCarbonFilterFarm::~BioXASCarbonFilterFarm()
{

}

void BioXASCarbonFilterFarm::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connectedChanged(connected_);
	}
}

void BioXASCarbonFilterFarm::updateConnected()
{
	bool filterOK = (filter_ && filter_->isConnected());

	setConnected(filterOK);
}
