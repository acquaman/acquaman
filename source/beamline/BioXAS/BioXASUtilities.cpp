#include "BioXASUtilities.h"
#include "beamline/BioXAS/BioXASUtilitiesState.h"
#include "beamline/BioXAS/BioXASIonPumps.h"
#include "beamline/BioXAS/BioXASValves.h"

BioXASUtilities::BioXASUtilities(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize class variables.

	state_ = new BioXASUtilitiesState(QString("%1%2").arg(name).arg("State"), this);
	connect( state_, SIGNAL(valueChanged(double)), this, SIGNAL(stateValueChanged(double)) );

	ionPumps_ = new BioXASIonPumps(QString("%1%2").arg(name).arg("IonPumps"), this);
	connect( ionPumps_, SIGNAL(valueChanged(double)), this, SIGNAL(ionPumpsStateValueChanged(double)) );
	connect( ionPumps_, SIGNAL(ionPumpsChanged()), this, SIGNAL(ionPumpsChanged()) );

	valves_ = new BioXASValves(QString("%1%2").arg(name).arg("Valves"), this);
	connect( valves_, SIGNAL(valueChanged(double)), this, SIGNAL(valvesStateValueChanged(double)) );
	connect( valves_, SIGNAL(valvesChanged()), this, SIGNAL(valvesChanged()) );

	// Setup state control.

	state_->setIonPumps(ionPumps_);
	state_->setValves(valves_);
}

BioXASUtilities::~BioXASUtilities()
{

}

bool BioXASUtilities::isConnected() const
{
	bool connected = (
				state_ && state_->isConnected() &&
				ionPumps_ && ionPumps_->isConnected() &&
				valves_ && valves_->isConnected()
				);

	return connected;
}

double BioXASUtilities::stateValue() const
{
	double result = -1;

	if (state_)
		result = state_->value();

	return result;
}

double BioXASUtilities::ionPumpsStateValue() const
{
	double result = -1;

	if (ionPumps_)
		result = ionPumps_->value();

	return result;
}

double BioXASUtilities::valvesStateValue() const
{
	double result = -1;

	if (valves_)
		result = valves_->value();

	return result;
}

void BioXASUtilities::addIonPump(AMControl *newControl)
{
	if (ionPumps_)
		ionPumps_->addIonPump(newControl);
}

void BioXASUtilities::removeIonPump(AMControl *control)
{
	if (ionPumps_)
		ionPumps_->removeIonPump(control);
}

void BioXASUtilities::clearIonPumps()
{
	if (ionPumps_)
		ionPumps_->clearIonPumps();
}

void BioXASUtilities::addValve(AMControl *newControl, double openValue, double closedValue)
{
	if (valves_)
		valves_->addValve(newControl, openValue, closedValue);
}

void BioXASUtilities::removeValve(AMControl *control)
{
	if (valves_)
		valves_->removeValve(control);
}

void BioXASUtilities::clearValves()
{
	if (valves_)
		valves_->clearValves();
}

