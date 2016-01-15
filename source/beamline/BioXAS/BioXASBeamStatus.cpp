#include "BioXASBeamStatus.h"
#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"
#include "beamline/CLS/CLSBiStateControl.h"

BioXASBeamStatus::BioXASBeamStatus(const QString &name, QObject *parent) :
	BioXASBiStateGroup(name, parent)
{
	// Setup the basic value options.

	addOption(On, "On", true);
	addOption(Off, "Off", true);
}

BioXASBeamStatus::~BioXASBeamStatus()
{

}

bool BioXASBeamStatus::isOn() const
{
	bool result = false;

	if (isConnected() && areAllChildrenState1())
		result = true;

	return result;
}

bool BioXASBeamStatus::isOff() const
{
	bool result = false;

	if (isConnected() && areAnyChildrenState2())
		result = true;

	return result;
}

bool BioXASBeamStatus::isConnected() const
{
	bool connected = (
				shutters_ && shutters_->isConnected() &&
				valves_ && valves_->isConnected() &&
				mirrorMaskState_ && mirrorMaskState_->isConnected() &&
				monoMaskState_ && monoMaskState_->isConnected()
				);

	return connected;
}

void BioXASBeamStatus::setShutters(BioXASShutters *newShutters)
{
	if (shutters_ != newShutters) {

		if (shutters_)
			removeBiStateControl(shutters_);

		shutters_ = newShutters;

		if (shutters_)
			addBiStateControl(shutters_, BioXASShutters::Open, BioXASShutters::Closed);

		emit shuttersChanged(shutters_);
	}
}

void BioXASBeamStatus::setValves(BioXASValves *newValves)
{
	if (valves_ != newValves) {

		if (valves_)
			removeBiStateControl(valves_);

		valves_ = newValves;

		if (valves_)
			addBiStateControl(valves_, BioXASValves::Open, BioXASValves::Closed);

		emit valvesChanged(valves_);
	}
}

void BioXASBeamStatus::setMirrorMaskState(BioXASM1MirrorMaskState *newControl)
{
	if (mirrorMaskState_ != newControl) {

		if (mirrorMaskState_)
			removeBiStateControl(mirrorMaskState_);

		mirrorMaskState_ = newControl;

		if (mirrorMaskState_)
			addBiStateControl(mirrorMaskState_, BioXASM1MirrorMaskState::Open, BioXASM1MirrorMaskState::Closed);

		emit mirrorMaskStateChanged(mirrorMaskState_);
	}
}

void BioXASBeamStatus::setMonoMaskState(BioXASSSRLMonochromatorMaskState *newControl)
{
	if (monoMaskState_ != newControl) {

		if (monoMaskState_)
			removeBiStateControl(monoMaskState_);

		monoMaskState_ = newControl;

		if (monoMaskState_)
			addBiStateControl(monoMaskState_, BioXASSSRLMonochromatorMaskState::Open, BioXASSSRLMonochromatorMaskState::Closed);

		emit monoMaskStateChanged(monoMaskState_);
	}
}

void BioXASBeamStatus::addControl(AMControl *newControl, double onValue, double offValue)
{
	if (addBiStateControl(newControl, onValue, offValue))
		emit controlsChanged();
}

void BioXASBeamStatus::removeControl(AMControl *control)
{
	if (removeBiStateControl(control))
		emit controlsChanged();
}

void BioXASBeamStatus::clearControls()
{
	if (clearBiStateControls())
		emit controlsChanged();
}

int BioXASBeamStatus::currentIndex() const
{
	int result = AMEnumeratedControl::Unknown;

	if (isOn())
		result = On;
	else if (isOff())
		result = Off;

	return result;
}
