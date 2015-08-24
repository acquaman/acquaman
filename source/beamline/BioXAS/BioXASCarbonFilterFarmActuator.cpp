#include "BioXASCarbonFilterFarmActuator.h"

BioXASCarbonFilterFarmActuator::BioXASCarbonFilterFarmActuator(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize member variables.

	position_ = 0;
	status_ = 0;

	actuator_ = 0;
}

BioXASCarbonFilterFarmActuator::~BioXASCarbonFilterFarmActuator()
{

}

bool BioXASCarbonFilterFarmActuator::isConnected() const
{
	bool connected = (
				actuator_ && actuator_->isConnected()
				);

	return connected;
}
