#include "BioXASUtilities.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMBeamlineControlGroup.h"
#include "beamline/CLS/CLSShutters.h"
#include "beamline/CLS/CLSValves.h"

BioXASUtilities::BioXASUtilities(const QString &name, QObject *parent) :
	BioXASUtilitiesGroup(name, parent)
{
	// Initialize shutters.

	shutters_ = new CLSShutters(QString("%1%2").arg(name).arg("Shutters"), this);
	addControl(shutters_, CLSShutters::Closed, CLSShutters::Open);

	connect( shutters_, SIGNAL(valueChanged(double)), this, SIGNAL(shuttersValueChanged(double)) );

	// Initialize valves.

	beampathValves_ = new CLSValves(QString("%1%2").arg(name).arg("BeampathValves"), this);
	addControl(beampathValves_, CLSValves::Closed, CLSValves::Open);

	connect( beampathValves_, SIGNAL(valueChanged(double)), this, SIGNAL(beampathValvesValueChanged(double)) );

	valves_ = new CLSValves(QString("%1%2").arg(name).arg("Valves"), this);
	addControl(valves_, CLSValves::Closed, CLSValves::Open);

	connect( valves_, SIGNAL(valueChanged(double)), this, SIGNAL(valvesValueChanged(double)) );
}

BioXASUtilities::~BioXASUtilities()
{

}

bool BioXASUtilities::isConnected() const
{
	bool connected = (
				shutters_ && shutters_->isConnected() &&
				beampathValves_ && beampathValves_->isConnected() &&
				valves_ && valves_->isConnected()
				);

	return connected;
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

bool BioXASUtilities::addShutter(AMControl *newControl, double openValue, double closedValue)
{
	bool result = false;

	if (shutters_)
		result = shutters_->addShutter(newControl, -1, openValue, closedValue);

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
		added = beampathValves_->addValve(newControl, -1, openValue, closedValue);

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
		result = valves_->addValve(newControl, -1, openValue, closedValue);

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

void BioXASUtilities::updateMoving()
{
	setIsMoving(false);
}

int BioXASUtilities::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isBad())
		result = Bad;
	else if (isGood())
		result = Good;

	return result;
}
