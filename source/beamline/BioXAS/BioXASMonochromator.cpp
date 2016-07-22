#include "BioXASMonochromator.h"
#include "beamline/BioXAS/BioXASMonochromatorEnergyControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"

BioXASMonochromator::BioXASMonochromator(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize class variables.

	energy_ = 0;
}

BioXASMonochromator::~BioXASMonochromator()
{

}

bool BioXASMonochromator::isConnected() const
{
	bool connected = ( energy_ && energy_->isConnected() );

	return connected;
}

void BioXASMonochromator::setEnergy(BioXASMonochromatorEnergyControl *newControl)
{
	if (energy_ != newControl) {

		removeChildControl(energy_);

		energy_ = newControl;

		addChildControl(energy_);

		emit energyChanged(energy_);
	}
}
