#include "BioXASUtilitiesState.h"
#include "beamline/BioXAS/BioXASIonPumps.h"
#include "beamline/BioXAS/BioXASValves.h"

BioXASUtilitiesState::BioXASUtilitiesState(const QString &name, QObject *parent) :
	AMEnumeratedControl(name, "", parent)
{
	// Initialize class variables.

	ionPumps_ = 0;
	valves_ = 0;

	// Setup value options.

	addOption(Good, "Good", true);
	addOption(Bad, "Bad", true);
}

BioXASUtilitiesState::~BioXASUtilitiesState()
{

}

bool BioXASUtilitiesState::canMeasure() const
{
	bool result = 0;

	if (isConnected())
		result = ( ionPumps_->canMeasure() && valves_->canMeasure() );

	return result;
}

bool BioXASUtilitiesState::isGood() const
{
	bool result = false;

	if (isConnected())
		result = ( ionPumps_->isGood() && valves_->isOpen() );

	return result;
}

bool BioXASUtilitiesState::isBad() const
{
	bool result = false;

	if (isConnected())
		result = ( ionPumps_->isBad() || valves_->isClosed() );

	return result;
}

void BioXASUtilitiesState::setIonPumps(BioXASIonPumps *newControl)
{
	if (ionPumps_ != newControl) {

		if (ionPumps_)
			removeChildControl(ionPumps_);

		ionPumps_ = newControl;

		if (ionPumps_)
			addChildControl(ionPumps_);

		emit ionPumpsChanged(ionPumps_);
	}
}

void BioXASUtilitiesState::setValves(BioXASValves *newControl)
{
	if (valves_ != newControl) {

		if (valves_)
			removeChildControl(valves_);

		valves_ = newControl;

		if (valves_)
			addChildControl(valves_);

		emit valvesChanged(valves_);
	}
}

void BioXASUtilitiesState::updateConnected()
{
	bool connected = (
				ionPumps_ && ionPumps_->isConnected() &&
				valves_ && valves_->isConnected()
				);

	setConnected(connected);
}

void BioXASUtilitiesState::updateMoving()
{
	bool moving = (
				(ionPumps_ && ionPumps_->isMoving()) ||
				(valves_ && valves_->isMoving())
				);

	setIsMoving(moving);
}

int BioXASUtilitiesState::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isGood())
		result = Good;
	else if (isBad())
		result = Bad;

	return result;
}

