#include "BioXASShutters.h"

BioXASShutters::BioXASShutters(const QString &name, QObject *parent) :
	AMEnumeratedControl(name, "", parent)
{

}

BioXASShutters::~BioXASShutters()
{

}

bool BioXASShutters::canMeasure() const
{
	bool result = false;

	if (isConnected()) {

		// Iterate through the list of children, finding out if
		// all can be measured.

		QList<AMControl*> children = childControls();

		if (children.count() > 0) {
			bool measurable = true;

			for (int i = 0, count = children.count(); i < count && measurable; i++) {
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

bool BioXASShutters::canMove() const
{
	bool result = false;

	if (isConnected()) {

		// Iterate through the list of children, finding out if
		// all can be moved.

		QList<AMControl*> children = childControls();

		if (children.count() > 0) {
			bool movable = true;

			for (int i = 0, count = children.count(); i < count && movable; i++) {
				AMControl *child = children.at(i);

				if (child && child->canMove())
					movable = true;
				else
					movable = false;
			}

			result = movable;
		}
	}

	return result;
}

bool BioXASShutters::canStop() const
{
	bool result = false;

	if (isConnected()) {

		// Iterate through the list of children, finding out if
		// all can be stopped.

		QList<AMControl*> children = childControls();

		if (children.count() > 0) {
			bool stopable = true;

			for (int i = 0, count = children.count(); i < count && stopable; i++) {
				AMControl *child = children.at(i);

				if (child && child->canStop())
					stopable = true;
				else
					stopable = false;
			}

			result = stopable;
		}
	}

	return result;
}

bool BioXASShutters::isOpen() const
{
	return false;
}

bool BioXASShutters::isClosed() const
{
	return false;
}

QList<AMControl*> BioXASShutters::shuttersList() const
{
	return shuttersSet_->toList();
}

void BioXASShutters::updateConnected()
{

}

void BioXASShutters::updateMoving()
{

}

void BioXASShutters::addShutter(AMControl *newShutter)
{
	if (newValve && !shuttersSet_->contains(newValve->name())) {
		shuttersSet_->addControl(newValve);
		addChildControl(newValve);

		emit valvesChanged();
	}
}

void BioXASShutters::removeShutter(AMControl *shutter)
{
	if (valve && !valveSet_->contains(valve->name())) {
		shuttersSet_->removeControl(valve);
		removeChildControl(valve);

		emit valvesChanged();
	}
}

void BioXASShutters::addShutters(BioXASShutters *newShutters)
{
	if (newShutters && !valveSet_->contains(newValve->name())) {
		shuttersSet_->addControl(newValve);
		addChildControl(newValve);

		emit valvesChanged();
	}
}

void BioXASShutters::removeShutters(BioXASShutters *shutters)
{
	if (valve && !valveSet_->contains(valve->name())) {
		shuttersSet_->removeControl(valve);
		removeChildControl(valve);

		emit valvesChanged();
	}
}

void BioXASShutters::clearValves()
{
	if (!valveSet_->isEmpty()) {

		// Clear the valve set.

		valveSet_->clear();

		// Clear the list of children.

	}
}

AMAction3* BioXASShutters::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if ((int)setpoint == Open)
		result = createOpenValvesAction();

	return result;
}

AMAction3* BioXASShutters::createOpenValvesAction()
{
	AMAction3 *result = 0;

	if (valveSet_ && !valveSet_->isEmpty()) {
		AMListAction3 *actionList = new AMListAction3(new AMListActionInfo3("Open valves", "Open valves"), AMListAction3::Parallel);

		foreach (AMControl *control, valveSet_->toList()) {
			CLSBiStateControl *biStateValve = qobject_cast<CLSBiStateControl*>(control);
			if (biStateValve)
				actionList->addSubAction(AMActionSupport::buildControlMoveAction(biStateValve, CLSBiStateControl::Open));

			AMReadOnlyPVControl *readOnlyValve = qobject_cast<AMReadOnlyPVControl*>(control);
			if (!biStateValve && readOnlyValve)
				actionList->addSubAction(AMActionSupport::buildControlWaitAction(readOnlyValve, 1));
		}

		result = actionList;
	}

	return result;
}

QStringList BioXASShutters::generateEnumStates() const
{
	QStringList enumOptions = AMEnumeratedControl::generateEnumStates();

	// We want to add 'None' and 'Between' states to the list of enum states.

	if (!enumOptions.contains("None"))
		enumOptions << "None";

	return enumOptions;
}

int BioXASShutters::currentIndex() const
{
	int result = indicesNamed("None").first();

	if (isConnected()) {
		if (isOpen())
			result = Open;
		else
			result = Closed;
	}

	return result;
}
