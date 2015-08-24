#include "BioXASCarbonFilterFarmActuatorControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"
#include "util/AMErrorMonitor.h"

BioXASCarbonFilterFarmActuatorControl::BioXASCarbonFilterFarmActuatorControl(const QString &name, const QString &units, QObject *parent) :
	AMPseudoMotorControl(name, units, parent)
{
	// Initialize local variables.

	position_ = 0;
	status_ = 0;
}

BioXASCarbonFilterFarmActuatorControl::~BioXASCarbonFilterFarmActuatorControl()
{

}



void BioXASCarbonFilterFarmActuatorControl::setPositionControl(AMControl *newControl)
{
	if (position_ != newControl) {

		if (position_) {
			removeChildControl(position_);
		}

		position_ = newControl;

		if (position_) {
			addChildControl(position_);
		}
	}
}

void BioXASCarbonFilterFarmActuatorControl::setStatusControl(AMControl *newControl)
{
	if (status_ != newControl) {

		if (status_) {
			removeChildControl(status_);
		}

		status_ = newControl;

		if (status_) {
			addChildControl(status_);
		}
	}
}

