#include "CLSBeamlineStatus.h"

CLSBeamlineStatus::CLSBeamlineStatus(const QString &name, QObject *parent) :
	CLSBiStateGroup(name, parent)
{
	shuttersControlSet_ = new AMControlSet(this);
	valvesControlSet_ = new AMControlSet(this);
	mirrorMaskControlSet_ = new AMControlSet(this);
	monoMaskControlSet_ = new AMControlSet(this);

	// Setup the basic value options.
	addOption(On, "On", true);
	addOption(Off, "Off", true);
}

CLSBeamlineStatus::~CLSBeamlineStatus()
{
	shuttersControlSet_->clear();
	valvesControlSet_->clear();
	mirrorMaskControlSet_->clear();
	monoMaskControlSet_->clear();
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

bool CLSBeamlineStatus::addShutterControl(AMControl *newControl, double beamOnValue)
{
	bool result = addComponent(newControl, beamOnValue);
	if (result)
		shuttersControlSet_->addControl(newControl);

	return result;
}

bool CLSBeamlineStatus::addValveControl(AMControl *newControl, double beamOnValue)
{
	bool result = addComponent(newControl, beamOnValue);
	if (result)
		valvesControlSet_->addControl(newControl);

	return result;
}

bool CLSBeamlineStatus::addMirrorMaskControl(AMControl *newControl, double beamOnValue)
{
	bool result = addComponent(newControl, beamOnValue);
	if (result)
		mirrorMaskControlSet_->addControl(newControl);

	return result;
}

bool CLSBeamlineStatus::addMonoMaskControl(AMControl *newControl, double beamOnValue)
{
	bool result = addComponent(newControl, beamOnValue);
	if (result)
		monoMaskControlSet_->addControl(newControl);

	return result;
}

void CLSBeamlineStatus::onBeamlineStatusControlValueChanged()
{
	bool beamOn = false;

	emit beamStatusChanged(beamOn);
}

bool CLSBeamlineStatus::removeComponent(AMControl *control)
{
	bool result = removeBiStateControl(control);

	if (result) {
		if (isShutterControl(control))
			shuttersControlSet_->removeControl(control);
		else if (isValveControl(control))
			valvesControlSet_->removeControl(control);
		else if (isMirrorMaskControl(control))
			mirrorMaskControlSet_->removeControl(control);
		else if (isMonoMaskControl(control))
			monoMaskControlSet_->removeControl(control);

		emit componentsChanged();
	}

	return result;
}

bool CLSBeamlineStatus::clearComponents()
{
	bool result = clearBiStateControls();

	if (result)
		emit componentsChanged();

	return result;
}

bool CLSBeamlineStatus::addComponent(AMControl *newControl, double beamOnValue)
{
	bool result = addBiStateControl(newControl, beamOnValue);

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
