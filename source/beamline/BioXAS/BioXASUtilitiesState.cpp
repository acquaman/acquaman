#include "BioXASUtilitiesState.h"
#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASIonPumps.h"

BioXASUtilitiesState::BioXASUtilitiesState(const QString &name, QObject *parent) :
	AMEnumeratedControl(name, "", parent)
{
	// Initialize class variables.

	shutters_ = 0;
	valves_ = 0;
	ionPumps_ = 0;

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
		result = (
					shutters_->isOpen() &&
					valves_->isOpen() &&
					ionPumps_->isGood()
					);

	return result;
}

bool BioXASUtilitiesState::isBad() const
{
	bool result = false;

	if (isConnected())
		result = (
					shutters_->isClosed() ||
					valves_->isClosed() ||
					ionPumps_->isBad()
					);

	return result;
}

void BioXASUtilitiesState::setShutters(BioXASShutters *newControl)
{
	if (shutters_ != newControl) {

		if (shutters_)
			removeChildControl(shutters_);

		shutters_ = newControl;

		if (shutters_)
			addChildControl(shutters_);

		emit shuttersChanged(shutters_);
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

void BioXASUtilitiesState::updateConnected()
{
	bool connected = (
				shutters_ && shutters_->isConnected() &&
				valves_ && valves_->isConnected() &&
				ionPumps_ && ionPumps_->isConnected()
				);

	setConnected(connected);
}

void BioXASUtilitiesState::updateMoving()
{
	bool moving = (
				(shutters_ && shutters_->isMoving()) ||
				(valves_ && valves_->isMoving()) ||
				(ionPumps_ && ionPumps_->isMoving())
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

