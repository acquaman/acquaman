#include "BioXASShutters.h"

BioXASShutters::BioXASShutters(const QString &name, QObject *parent) :
	BioXASBiStateGroup(name, parent)
{
	// Setup basic value options.

	addOption(Open, "Open");
	addOption(Closed, "Closed");
}

BioXASShutters::~BioXASShutters()
{

}

void BioXASShutters::addShutter(AMControl *newShutter, double openValue, double closedValue)
{
	if (addBiStateControl(newShutter, openValue, closedValue))
		emit shuttersChanged();
}

void BioXASShutters::removeShutter(AMControl *shutter)
{
	if (removeBiStateControl(shutter))
		emit shuttersChanged();
}

void BioXASShutters::clearShutters()
{
	if (clearBiStateControls())
		emit shuttersChanged();
}
