#include "BioXASIonPumps.h"
#include "util/AMErrorMonitor.h"

BioXASIonPumps::BioXASIonPumps(const QString &name, QObject *parent) :
	BioXASBiStateGroup(name, parent)
{
	// Set up basic value options.

	addOption(Bad, "Bad", true);
	addOption(Good, "Good", true);
}

BioXASIonPumps::~BioXASIonPumps()
{

}

bool BioXASIonPumps::isBad() const
{
	bool result = false;

	if (isConnected() && areAllChildrenState1())
		result = true;

	return result;
}

bool BioXASIonPumps::isGood() const
{
	bool result = false;

	if (isConnected() && areAllChildrenState2())
		result = true;

	return result;
}

QList<AMControl*> BioXASIonPumps::badIonPumpsList() const
{
	return childrenInState1();
}

QList<AMControl*> BioXASIonPumps::goodIonPumpsList() const
{
	return childrenInState2();
}

void BioXASIonPumps::addIonPump(AMControl *newPump)
{
	if (addBiStateControl(newPump, Bad, Good))
		emit ionPumpsChanged();
}

void BioXASIonPumps::removeIonPump(AMControl *pump)
{
	if (removeBiStateControl(pump))
		emit ionPumpsChanged();
}

void BioXASIonPumps::clearIonPumps()
{
	if (clearBiStateControls())
		emit ionPumpsChanged();
}

void BioXASIonPumps::updateValue()
{
	BioXASBiStateGroup::updateValue();

	// Display AMErrorMon if any child controls
	// are in a Bad state.

	QList<AMControl*> ionPumps = badIonPumpsList();

	if (!ionPumps.isEmpty()) {
		QString error("The following ion pumps are no longer operating correctly:\n");

		foreach (AMControl *control, ionPumps) {
			if (control)
				error += tr("%1\n").arg(control->name());
		}

		AMErrorMon::error(this, BIOXASIONPUMPS_BAD_STATE, error);
	}
}

int BioXASIonPumps::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isBad())
		result = Bad;
	else if (isGood())
		result = Good;

	return result;
}
