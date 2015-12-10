#include "BioXASValves.h"
#include "beamline/BioXAS/BioXASBiStateGroup.h"

BioXASValves::BioXASValves(const QString &name, QObject *parent) :
	BioXASBiStateGroup(name, parent)
{
	// Setup basic value options.

	addOption(Open, "Open");
	addOption(Closed, "Closed");
}

BioXASValves::~BioXASValves()
{

}

void BioXASValves::addValve(AMControl *newValve, double openValue, double closedValue)
{
	if (addBiStateControl(newValve, openValue, closedValue))
		emit valvesChanged();
}

void BioXASValves::removeValve(AMControl *valve)
{
	if (removeBiStateControl(valve))
		emit valvesChanged();
}

void BioXASValves::clearValves()
{
	if (clearBiStateControls())
		emit valvesChanged();
}
