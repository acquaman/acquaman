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
				measurable = (child && child->canMeasure());
			}

			result = measurable;
		}
	}

	return result;
}

bool BioXASBeamStatus::isOff() const
{
	bool result = false;

	if (isConnected()) {

		// Iterate through the list of children.
		// We want to stop iterating if we come across any children that are in the 'beam off' state.
		// (Only one child needs to be 'beam off' in order for the beam status to be off).

		bool beamOff = false;

		for (int i = 0, count = children_.count(); i < count && !beamOff; i++)
			beamOff = componentIsOff(children_.at(i));

		result = beamOff;
	}

	return result;
}

bool BioXASBeamStatus::isOn() const
{
	bool result = false;

	if (isConnected()) {

		// Iterate through the list of children.
		// We want to stop iterating if we come across any children not the in the 'beam on' state.
		// (All children must be 'beam on' in order for the beam status to be on).

		bool beamOn = true;

		for (int i = 0, count = children_.count(); i < count && beamOn; i++)
			beamOn = componentIsOn(children_.at(i));

		result = beamOn;
	}

	return result;
}

bool BioXASBeamStatus::componentIsOff(AMControl *control) const
{
	bool result = false;

	if (control && control->canMeasure() && children_.contains(control) && componentBeamStatusMap_.keys().contains(control)) {

		// Iterate through the list of beam status states associated with a given component.
		// We want to stop iterating if we come across any 'beam off' states the control happens to be in.

		bool componentOff = false;

		QList<BioXASBeamStatusState> states = componentBeamStatusMap_.values(control);

		for (int i = 0, count = states.count(); i < count && !componentOff; i++) {
			BioXASBeamStatusState state = states.at(i);

			if ((state.beamStatusValue() == BioXASBeamStatus::Off) && (state.controlMinValue() <= control->value()) && (control->value() <= state.controlMaxValue()))
				componentOff = true;
		}

		result = componentOff;
	}

	return result;
}

bool BioXASBeamStatus::componentIsOn(AMControl *control) const
{
	bool result = false;

	if (control && control->canMeasure() && children_.contains(control) && componentBeamStatusMap_.keys().contains(control)) {

		// Iterate through the list of beam status states associated with a given component.
		// We want to stop iterating if we come across any 'beam on' states the control happens to be in.

		bool componentOn = false;

		QList<BioXASBeamStatusState> states = componentBeamStatusMap_.values(control);

		for (int i = 0, count = states.count(); i < count && !componentOn; i++) {
			BioXASBeamStatusState state = states.at(i);

			if ((state.beamStatusValue() == BioXASBeamStatus::On) && (state.controlMinValue() <= control->value()) && (control->value() <= state.controlMaxValue()))
				componentOn = true;
		}

		result = componentOn;
	}

	return result;
}

QList<BioXASBeamStatusState> BioXASBeamStatus::componentBeamStatusStates(AMControl *control) const
{
	return componentBeamStatusMap_.values(control);
}

bool BioXASBeamStatus::addComponent(AMControl *control, const QList<BioXASBeamStatusState> beamStatusStates)
{
	bool result = false;

	if (control && !children_.contains(control) && !beamStatusStates.isEmpty()) {
		addChildControl(control);

		foreach (BioXASBeamStatusState state, beamStatusStates)
			componentBeamStatusMap_.insertMulti(control, state);

		result = true;
	}

	return result;
}

bool BioXASBeamStatus::removeComponent(AMControl *control)
{
	bool result = false;

	if (children_.contains(control)) {
		removeChildControl(control);
		componentBeamStatusMap_.remove(control);

		result = true;
	}

	return result;
}

bool BioXASBeamStatus::clearComponents()
{
	clearChildControls();
	componentBeamStatusMap_.clear();

	return true;
}

AMAction3* BioXASBeamStatus::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (setpoint == BioXASBeamStatus::Off)
		result = createMoveToOffAction();
	else if (setpoint == BioXASBeamStatus::On)
		result = createMoveToOnAction();

	return result;
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
