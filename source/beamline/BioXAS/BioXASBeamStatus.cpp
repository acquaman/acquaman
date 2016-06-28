#include "BioXASBeamStatus.h"

BioXASBeamStatus::BioXASBeamStatus(const QString &name, QObject *parent) :
	AMEnumeratedControl(name, "", parent)
{
	addOption(Off, "Off", true);
	addOption(On, "On", true);
}

BioXASBeamStatus::~BioXASBeamStatus()
{

}

bool BioXASBeamStatus::canMeasure() const
{
	bool result = false;

	if (isConnected()) {

		// Iterate through the list of children, finding out if
		// all can be measured.

		QList<AMControl*> children = childControls();

		if (children.count() > 0) {
			bool measurable = true;

			for (int i = 0, count = children.count(); i < count && measurable; i++) { // we want to stop if any one child is not measurable.
				AMControl *child = children.at(i);

				if (child && child->canMeasure())
					measurable = true;
				else
					measurable = false;
			}

			result = measurable;
		}
	}

	return result;
}

bool BioXASBeamStatus::isOff() const
{
	return !isOn();
}

bool BioXASBeamStatus::isOn() const
{
	bool result = false;

	if (isConnected()) {

		// Iterate through the list of children, finding out if
		// any are NOT in the 'beam on' state.

		bool beamOn = true;

		for (int i = 0, count = children_.count(); i < count && beamOn; i++) { // we want to stop if any one child is not in the 'beam on' state.
			AMControl *control = children_.at(i);
			beamOn = (control && componentBeamOnValueMap_.keys().contains(control) && control->value() == componentBeamOnValueMap_.value(control));
		}

		result = beamOn;
	}

	return result;
}

bool BioXASBeamStatus::addComponent(AMControl *control, double beamOffValue, double beamOnValue)
{
	bool result = false;

	if (control && !children_.contains(control)) {
		addChildControl(control);
		componentBeamOffValueMap_.insert(control, beamOffValue);
		componentBeamOnValueMap_.insert(control, beamOnValue);

		result = true;
	}

	return result;
}

bool BioXASBeamStatus::removeComponent(AMControl *control)
{
	bool result = false;

	if (children_.contains(control)) {
		removeChildControl(control);
		componentBeamOffValueMap_.remove(control);
		componentBeamOnValueMap_.remove(control);

		result = true;
	}

	return result;
}

bool BioXASBeamStatus::clearComponents()
{
	clearChildControls();
	componentBeamOffValueMap_.clear();
	componentBeamOnValueMap_.clear();

	return true;
}

int BioXASBeamStatus::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isOff())
		result = Off;
	else if (isOn())
		result = On;

	return result;
}
