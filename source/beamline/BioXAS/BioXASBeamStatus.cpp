#include "BioXASBeamStatus.h"
#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"

BioXASBeamStatus::BioXASBeamStatus(const QString &name, QObject *parent) :
	BioXASBiStateGroup(name, parent)
{
	// Initialize shutters.

	shutters_ = new BioXASShutters(QString("%1%2").arg(name).arg("Shutters"), this);
	addBiStateControl(shutters_, BioXASShutters::Open, BioXASShutters::Closed);

	connect( shutters_, SIGNAL(valueChanged(double)), this, SIGNAL(shuttersValueChanged(double)) );

	// Initialize valves.

	valves_ = new BioXASValves(QString("%1%2").arg(name).arg("Valves"), this);
	addBiStateControl(valves_, BioXASValves::Open, BioXASValves::Closed);

	connect( valves_, SIGNAL(valueChanged(double)), this, SIGNAL(valvesValueChanged(double)) );

	// Setup the basic value options.

	addOption(On, "On", true);
	addOption(Off, "Off", true);
}

BioXASBeamStatus::~BioXASBeamStatus()
{

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

double BioXASBeamStatus::shuttersValue() const
{
	double result = -1;

	if (shutters_ && shutters_->canMeasure())
		result = shutters_->value();

	return result;
}

double BioXASBeamStatus::valvesValue() const
{
	double result = -1;

	if (valves_ && valves_->canMeasure())
		result = valves_->value();

	return result;
}

bool BioXASBeamStatus::addShutter(AMControl *newControl, double openValue, double closedValue)
{
	bool result = false;

	if (shutters_)
		result = shutters_->addShutter(newControl, openValue, closedValue);

	return result;
}

bool BioXASBeamStatus::removeShutter(AMControl *control)
{
	bool result = false;

	if (shutters_)
		result = shutters_->removeShutter(control);

	return result;
}

bool BioXASBeamStatus::clearShutters()
{
	bool result = false;

	if (shutters_)
		result = shutters_->clearShutters();

	return result;
}

bool BioXASBeamStatus::addValve(AMControl *newControl, double openValue, double closedValue)
{
	bool result = false;

	if (valves_)
		result = valves_->addValve(newControl, openValue, closedValue);

	return result;
}

bool BioXASBeamStatus::removeValve(AMControl *control)
{
	bool result = false;

	if (valves_)
		result = valves_->removeValve(control);

	return result;
}

bool BioXASBeamStatus::clearValves()
{
	bool result = false;

	if (valves_)
		result = valves_->clearValves();

	return result;
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

int BioXASBeamStatus::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isOn())
		result = On;
	else if (isOff())
		result = Off;

	return result;
}
