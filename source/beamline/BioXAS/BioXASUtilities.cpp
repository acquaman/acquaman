#include "BioXASUtilities.h"
#include "beamline/AMPVControl.h"
#include "beamline/BioXAS/BioXASUtilitiesState.h"
#include "beamline/BioXAS/BioXASFrontEndShutters.h"
#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASIonPumps.h"
#include "beamline/CLS/CLSExclusiveStatesControl.h"

BioXASUtilities::BioXASUtilities(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize state.

	state_ = new BioXASUtilitiesState(QString("%1%2").arg(name).arg("State"), this);
	addChildControl(state_);

	connect( state_, SIGNAL(valueChanged(double)), this, SIGNAL(stateValueChanged(double)) );

	// Initialize shutters.

	shutters_ = new BioXASShutters(QString("%1%2").arg(name).arg("Shutters"), this);
	addChildControl(shutters_);

	connect( shutters_, SIGNAL(shuttersChanged()), this, SIGNAL(shuttersChanged()) );
	connect( shutters_, SIGNAL(valueChanged(double)), this, SIGNAL(shuttersValueChanged(double)) );

	// Initialize valves.

	beampathValves_ = new BioXASValves(QString("%1%2").arg(name).arg("BeampathValves"), this);
	addChildControl(beampathValves_);

	connect( beampathValves_, SIGNAL(valvesChanged()), this, SIGNAL(beampathValvesChanged()) );
	connect( beampathValves_, SIGNAL(valueChanged(double)), this, SIGNAL(beampathValvesValueChanged(double)) );

	valves_ = new BioXASValves(QString("%1%2").arg(name).arg("Valves"), this);
	addChildControl(valves_);

	connect( valves_, SIGNAL(valueChanged(double)), this, SIGNAL(valvesValueChanged(double)) );
	connect( valves_, SIGNAL(valvesChanged()), this, SIGNAL(valvesChanged()) );

	// Initialize ion pumps.

	ionPumps_ = new BioXASIonPumps(QString("%1%2").arg(name).arg("IonPumps"), this);
	addChildControl(ionPumps_);

	connect( ionPumps_, SIGNAL(valueChanged(double)), this, SIGNAL(ionPumpsValueChanged(double)) );
	connect( ionPumps_, SIGNAL(ionPumpsChanged()), this, SIGNAL(ionPumpsChanged()) );

	// Setup state control.

	state_->setShutters(shutters_);
	state_->setValves(valves_);
	state_->setIonPumps(ionPumps_);
}

BioXASUtilities::~BioXASUtilities()
{

}

bool BioXASUtilities::isConnected() const
{
	bool connected = (
				state_ && state_->isConnected() &&
				shutters_ && shutters_->isConnected() &&
				beampathValves_ && beampathValves_->isConnected() &&
				valves_ && valves_->isConnected() &&
				ionPumps_ && ionPumps_->isConnected()
				);

	return connected;
}

double BioXASUtilities::stateValue() const
{
	double result = -1;

	if (state_ && state_->canMeasure())
		result = state_->value();

	return result;
}

double BioXASUtilities::shuttersValue() const
{
	double result = -1;

	if (shutters_ && shutters_->canMeasure())
		result = shutters_->value();

	return result;
}

double BioXASUtilities::beampathValvesValue() const
{
	double result = -1;

	if (beampathValves_ && beampathValves_->canMeasure())
		result = beampathValves_->value();

	return result;
}

double BioXASUtilities::valvesValue() const
{
	double result = -1;

	if (valves_ && valves_->canMeasure())
		result = valves_->value();

	return result;
}

double BioXASUtilities::ionPumpsValue() const
{
	double result = -1;

	if (ionPumps_ && ionPumps_->canMeasure())
		result = ionPumps_->value();

	return result;
}

bool BioXASUtilities::hasShutter(AMControl *control) const
{
	bool result = false;

	if (shutters_)
		result = shutters_->hasShutter(control);

	return result;
}

bool BioXASUtilities::hasBeampathValve(AMControl *control) const
{
	bool result = false;

	if (beampathValves_)
		result = beampathValves_->hasValve(control);

	return result;
}

bool BioXASUtilities::hasValve(AMControl *control) const
{
	bool result = false;

	if (valves_)
		result = valves_->hasValve(control);

	return result;
}

bool BioXASUtilities::hasIonPump(AMControl *control) const
{
	bool result = false;

	if (ionPumps_)
		result = ionPumps_->hasIonPump(control);

	return result;
}

bool BioXASUtilities::addShutter(AMControl *newControl, double openValue, double closedValue)
{
	bool result = false;

	if (shutters_)
		result = shutters_->addShutter(newControl, openValue, closedValue);

	return result;
}

bool BioXASUtilities::removeShutter(AMControl *control)
{
	bool result = false;

	if (shutters_)
		result = shutters_->removeShutter(control);

	return result;
}

bool BioXASUtilities::clearShutters()
{
	bool result = false;

	if (shutters_)
		result = shutters_->clearShutters();

	return result;
}

bool BioXASUtilities::addBeampathValve(AMControl *newControl, double openValue, double closedValue)
{
	bool added = false;

	if (beampathValves_)
		added = beampathValves_->addValve(newControl, openValue, closedValue);

	// If the valve was added and is not a member of valves_, add it.

	if (added && !hasValve(newControl))
		addValve(newControl, openValue, closedValue);

	return added;
}

bool BioXASUtilities::removeBeampathValve(AMControl *control)
{
	bool result = false;

	if (beampathValves_)
		result = beampathValves_->removeValve(control);

	return result;
}

bool BioXASUtilities::clearBeampathValves()
{
	bool result = false;

	if (beampathValves_)
		result = beampathValves_->clearValves();

	return result;
}

bool BioXASUtilities::addValve(AMControl *newControl, double openValue, double closedValue)
{
	bool result = false;

	if (valves_)
		result = valves_->addValve(newControl, openValue, closedValue);

	return result;
}

bool BioXASUtilities::removeValve(AMControl *control)
{
	bool removed = false;

	if (valves_)
		removed = valves_->removeValve(control);

	// If the valve was removed and is a member of beampathValves_, remove it.

	if (removed && hasBeampathValve(control))
		removeBeampathValve(control);

	return removed;
}

bool BioXASUtilities::clearValves()
{
	bool result = false;

	if (valves_) {
		bool cleared = valves_->clearValves();

		if (cleared)
			clearBeampathValves();

		result = cleared;
	}

	return result;
}

bool BioXASUtilities::addIonPump(AMControl *newControl)
{
	bool result = false;

	if (ionPumps_)
		result = ionPumps_->addIonPump(newControl);

	return result;
}

bool BioXASUtilities::removeIonPump(AMControl *control)
{
	bool result = false;

	if (ionPumps_)
		result = ionPumps_->removeIonPump(control);

	return result;
}

bool BioXASUtilities::clearIonPumps()
{
	bool result = false;

	if (ionPumps_)
		result = ionPumps_->clearIonPumps();

	return result;
}

