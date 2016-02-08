#include "BioXASUtilitiesState.h"
#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASIonPumps.h"

BioXASUtilitiesState::BioXASUtilitiesState(const QString &name, QObject *parent) :
	BioXASBiStateGroup(name, parent)
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

	if (isConnected()) {
		result = (
					shutters_ && shutters_->canMeasure() &&
					valves_ && valves_->canMeasure() &&
					ionPumps_ && ionPumps_->canMeasure()
					);
	}

	return result;
}

bool BioXASUtilitiesState::isGood() const
{
	bool result = false;

	if (isConnected() && areAllChildrenState1())
		result = true;

	return result;
}

bool BioXASUtilitiesState::isBad() const
{
	bool result = false;

	if (isConnected() && areAnyChildrenState2())
		result = true;

	return result;
}

void BioXASUtilitiesState::setShutters(BioXASShutters *newControl)
{
	if (shutters_ != newControl) {

		if (shutters_)
			removeBiStateControl(shutters_);

		shutters_ = newControl;

		if (shutters_)
			addBiStateControl(shutters_, BioXASShutters::Open, BioXASShutters::Closed);

		emit shuttersChanged(shutters_);
	}
}

void BioXASUtilitiesState::setValves(BioXASValves *newControl)
{
	if (valves_ != newControl) {

		if (valves_)
			removeBiStateControl(valves_);

		valves_ = newControl;

		if (valves_)
			addBiStateControl(valves_, BioXASValves::Open, BioXASValves::Closed);

		emit valvesChanged(valves_);
	}
}

void BioXASUtilitiesState::setIonPumps(BioXASIonPumps *newControl)
{
	if (ionPumps_ != newControl) {

		if (ionPumps_)
			removeBiStateControl(ionPumps_);

		ionPumps_ = newControl;

		if (ionPumps_)
			addBiStateControl(ionPumps_, BioXASIonPumps::Good, BioXASIonPumps::Bad);

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
	setIsMoving(false);
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

