#include "BioXASBeamStatus.h"

BioXASBeamStatus::BioXASBeamStatus(const QString &name, QObject *parent) :
	BioXASBiStateGroup(name, parent)
{
	// Setup the basic value options.

	addOption(On, "On");
	addOption(Off, "Off");
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
	bool result = false;

	if (isConnected() && areAnyChildrenState2())
		result = true;

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
