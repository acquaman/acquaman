#include "CLSBeamlineStatus.h"

//#include "beamline/BioXAS/BioXASShutters.h"
//#include "beamline/BioXAS/BioXASValves.h"
//#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
//#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"

CLSBeamlineStatus::CLSBeamlineStatus(const QString &name, QObject *parent) :
	CLSBiStateGroup(name, parent)
{
	// Setup the basic value options.

	addOption(On, "On", true);
	addOption(Off, "Off", true);
}

CLSBeamlineStatus::~CLSBeamlineStatus()
{

}

bool CLSBeamlineStatus::isOn() const
{
	bool result = false;

	if (isConnected() && areAllChildrenState1())
		result = true;

	return result;
}

bool CLSBeamlineStatus::isOff() const
{
	return !isOn();
}

QList<AMControl*> CLSBeamlineStatus::componentsInBeamOnState() const
{
	return childrenInState1();
}

QList<AMControl*> CLSBeamlineStatus::componentsNotInBeamOnState() const
{
	return childrenNotInState1();
}

bool CLSBeamlineStatus::addComponent(AMControl *newControl, double beamOnValue)
{
	bool result = addBiStateControl(newControl, beamOnValue);

	if (result)
		emit componentsChanged();

	return result;
}

bool CLSBeamlineStatus::removeComponent(AMControl *control)
{
	bool result = removeBiStateControl(control);

	if (result)
		emit componentsChanged();

	return result;
}

bool CLSBeamlineStatus::clearComponents()
{
	bool result = clearBiStateControls();

	if (result)
		emit componentsChanged();

	return result;
}

int CLSBeamlineStatus::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isOn())
		result = On;
	else if (isOff())
		result = Off;

	return result;
}
