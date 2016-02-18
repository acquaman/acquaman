#include "BioXASUtilitiesGroup.h"

BioXASUtilitiesGroup::BioXASUtilitiesGroup(const QString &name, QObject *parent) :
	BioXASBiStateGroup(name, parent)
{
	// Setup value options.

	addOption(Bad, "Bad", true);
	addOption(Good, "Good", true);
}

BioXASUtilitiesGroup::~BioXASUtilitiesGroup()
{

}

bool BioXASUtilitiesGroup::isBad() const
{
	return (isConnected() && areAnyChildrenState1());
}

bool BioXASUtilitiesGroup::isGood() const
{
	return (isConnected() && areAllChildrenState2());
}

bool BioXASUtilitiesGroup::hasControl(AMControl *control) const
{
	return hasChildControl(control);
}

QList<AMControl*> BioXASUtilitiesGroup::badControlsList() const
{
	return childrenInState1();
}

QList<AMControl*> BioXASUtilitiesGroup::goodControlsList() const
{
	return childrenInState2();
}

bool BioXASUtilitiesGroup::addControl(AMControl *newControl)
{
	return addControl(newControl, Bad, Good);
}

bool BioXASUtilitiesGroup::addControl(AMControl *newControl, double badValue, double goodValue)
{
	bool result = addBiStateControl(newControl, badValue, goodValue);

	if (result)
		emit controlsChanged();

	return result;
}

bool BioXASUtilitiesGroup::removeControl(AMControl *control)
{
	bool result = removeBiStateControl(control);

	if (result)
		emit controlsChanged();

	return result;
}

bool BioXASUtilitiesGroup::clearControls()
{
	bool result = clearBiStateControls();

	if (result)
		emit controlsChanged();

	return result;
}

int BioXASUtilitiesGroup::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isBad())
		result = Bad;
	else if (isGood())
		result = Good;

	return result;
}

