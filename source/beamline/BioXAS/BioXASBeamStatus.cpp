#include "BioXASBeamStatus.h"
#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"

BioXASBeamStatus::BioXASBeamStatus(const QString &name, QObject *parent) :
	BioXASBiStateGroup(name, parent)
{
	// Setup the basic value options.

	addOption(On, "On", true);
	addOption(Off, "Off", true);
}

BioXASBeamStatus::~BioXASBeamStatus()
{

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
	return !isOn();
}

QList<AMControl*> BioXASBeamStatus::componentsInBeamOnState() const
{
	return childrenInState1();
}

QList<AMControl*> BioXASBeamStatus::componentsNotInBeamOnState() const
{
	return childrenNotInState1();
}

bool BioXASBeamStatus::addComponent(AMControl *newControl, double beamOnValue)
{
	bool result = addBiStateControl(newControl, beamOnValue);

	if (result)
		emit componentsChanged();

	return result;
}

bool BioXASBeamStatus::removeComponent(AMControl *control)
{
	bool result = removeBiStateControl(control);

	if (result)
		emit componentsChanged();

	return result;
}

bool BioXASBeamStatus::clearComponents()
{
	bool result = clearBiStateControls();

	if (result)
		emit componentsChanged();

	return result;
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
