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

	upstreamPosition_ = new CLSMAXvMotor(name+"UpstreamPosition", upstreamPVBase, name+"UpstreamPosition", true, 0.005, 2.0, this);
	upstreamStatus_ = new AMReadOnlyPVControl(name+"UpstreamStatus", upstreamPVBase+":inPosition", this);
	upstreamActuator_ = new BioXASCarbonFilterFarmActuatorControl(upstreamPosition_, upstreamStatus_, this);

	downstreamPosition_ = new CLSMAXvMotor(name+"DownstreamPosition", downstreamPVBase, name+"DownstreamPosition", true, 0.005, 2.0, this);
	downstreamStatus_ = new AMReadOnlyPVControl(name+"DownstreamStatus", downstreamPVBase+":inPosition", this);
	downstreamActuator_ = new BioXASCarbonFilterFarmActuatorControl(downstreamPosition_, downstreamStatus_, this);

	filter_ = new BioXASCarbonFilterFarmControl(upstreamActuator_, downstreamActuator_, this);
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
