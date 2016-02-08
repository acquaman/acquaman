#include "BioXASFlowSwitches.h"

BioXASFlowSwitches::BioXASFlowSwitches(const QString &name, QObject *parent) :
	BioXASBiStateGroup(name, parent)
{
	// Setup value options.

	addOption(Bad, "Bad", true);
	addOption(Good, "Good", true);
}

BioXASFlowSwitches::~BioXASFlowSwitches()
{

}

bool BioXASFlowSwitches::isBad() const
{
	bool result = false;

	if (isConnected() && areAnyChildrenState1())
		result = true;

	return result;
}

bool BioXASFlowSwitches::isGood() const
{
	bool result = false;

	if (isConnected() && areAnyChildrenState2())
		result = true;

	return result;
}

bool BioXASFlowSwitches::hasFlowSwitch(AMControl *control) const
{
	return hasChildControl(control);
}

QList<AMControl*> BioXASFlowSwitches::badFlowSwitchesList() const
{
	return childrenInState1();
}

QList<AMControl*> BioXASFlowSwitches::goodFlowSwitchesList() const
{
	return childrenInState2();
}

bool BioXASFlowSwitches::addFlowSwitch(AMControl *newControl)
{
	bool result = addBiStateControl(newControl, Bad, Good);

	if (result)
		emit flowSwitchesChanged();

	return result;
}

bool BioXASFlowSwitches::removeFlowSwitch(AMControl *control)
{
	bool result = removeBiStateControl(control);

	if (result)
		emit flowSwitchesChanged();

	return result;
}

bool BioXASFlowSwitches::clearFlowSwitches()
{
	bool result = clearBiStateControls();

	if (result)
		emit flowSwitchesChanged();

	return result;
}

int BioXASFlowSwitches::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isBad())
		result = Bad;
	else if (isGood())
		result = Good;

	return result;
}
