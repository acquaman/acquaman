#include "BioXASBiStateGroup.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASBiStateGroup::BioXASBiStateGroup(const QString &name, QObject *parent) :
	AMEnumeratedControl(name, "", parent)
{

}

BioXASBiStateGroup::~BioXASBiStateGroup()
{

}

bool BioXASBiStateGroup::canMeasure() const
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

bool BioXASBiStateGroup::canMove() const
{
	bool result = false;

	if (isConnected()) {

		// Iterate through the list of children, finding out if
		// all can be moved.

		QList<AMControl*> children = childControls();

		if (children.count() > 0) {
			bool movable = true;

			for (int i = 0, count = children.count(); i < count && movable; i++) { // we want to stop if any one child is not movable.
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

bool BioXASBiStateGroup::canStop() const
{
	bool result = false;

	if (isConnected()) {

		// Iterate through the list of children, finding out if
		// all can be stopped.

		QList<AMControl*> children = childControls();

		if (children.count() > 0) {
			bool stopable = true;

			for (int i = 0, count = children.count(); i < count && stopable; i++) { // we want to stop if any one child is not stopable.
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

void BioXASBiStateGroup::updateConnected()
{
	// Iterate through list of children, finding out if
	// all are connected.

	QList<AMControl*> children = childControls();

	if (children.count() > 0) {
		bool connected = false;

		for (int i = 0, count = children.count(); i < count && connected; i++) { // we want to stop if any one child isn't connected.
			AMControl *child = children.at(i);

			if (child && child->isConnected())
				connected = true;
			else
				connected = false;
		}

		setConnected(connected);
	}
}

void BioXASBiStateGroup::updateMoving()
{
	// Iterate through list of children, finding out if
	// any are moving.

	QList<AMControl*> children = childControls();

	if (children.count() > 0) {
		bool moving = false;

		for (int i = 0, count = children.count(); i < count && !moving; i++) { // we want to stop if any one child IS moving.
			AMControl *child = children.at(i);

			if (child && child->isMoving())
				moving = true;
			else
				moving = false;
		}

		setIsMoving(moving);
	}
}

bool BioXASBiStateGroup::addBiStateControl(AMControl *control, double state1Value, double state2Value)
{
	bool result = false;

	if (control) {
		controlState1ValueMap_.insert(control, state1Value);
		controlState2ValueMap_.insert(control, state2Value);

		addChildControl(control);

		result = true;
	}

	return result;
}

bool BioXASBiStateGroup::removeBiStateControl(AMControl *control)
{
	bool result = false;

	if ( control && (controlState1ValueMap_.contains(control) || controlState2ValueMap_.contains(control)) ) {
		controlState1ValueMap_.remove(control);
		controlState2ValueMap_.remove(control);

		removeChildControl(control);

		result = true;
	}

	return result;
}

bool BioXASBiStateGroup::clearBiStateControls()
{
	bool result = false;

	QList<AMControl*> children = childControls();

	if (children.count() > 0) {
		foreach (AMControl *child, childControls())
			removeBiStateControl(child);

		result = true;
	}

	return result;
}

bool BioXASBiStateGroup::areChildrenState1() const
{
	bool result = false;

	// Iterate through list of children, finding out if
	// all are are state 1.

	QList<AMControl*> children = childControls();

	if (children.count() > 0) {
		bool stateOK = true;

		for (int i = 0, count = children.count(); i < count && stateOK; i++) { // we want to stop if any one child isn't ok.
			AMControl *child = children.at(i);

			if (child && isChildState1(child))
				stateOK = true;
			else
				stateOK = false;
		}

		result = stateOK;
	}

	return result;
}

bool BioXASBiStateGroup::areChildrenState2() const
{
	bool result = false;

	// Iterate through list of children, finding out if
	// all are are state 2.

	QList<AMControl*> children = childControls();

	if (children.count() > 0) {
		bool stateOK = true;

		for (int i = 0, count = children.count(); i < count && stateOK; i++) { // we want to stop if any one child isn't ok.
			AMControl *child = children.at(i);

			if (child && isChildState2(child))
				stateOK = true;
			else
				stateOK = false;
		}

		result = stateOK;
	}

	return result;
}

bool BioXASBiStateGroup::isChildState1(AMControl *child) const
{
	bool result = false;

	if (child && controlState1ValueMap_.keys().contains(child)) {
		if (child->value() == controlState1ValueMap_.value(child))
			result = true;
	}

	return result;
}

bool BioXASBiStateGroup::isChildState2(AMControl *child) const
{
	bool result = false;

	if (child && controlState2ValueMap_.keys().contains(child)) {
		if (child->value() == controlState2ValueMap_.value(child))
			result = true;
	}

	return result;
}

AMAction3* BioXASBiStateGroup::createMoveChildrenToState1Action()
{
	AMListAction3 *moveChildren = new AMListAction3(new AMListActionInfo3("Moving bistate group.", "Moving bistate group."), AMListAction3::Parallel);

	foreach (AMControl *child, children_) {
		if (child)
			moveChildren->addSubAction(createMoveChildToState1Action(child));
	}

	return moveChildren;
}

AMAction3* BioXASBiStateGroup::createMoveChildrenToState2Action()
{
	AMListAction3 *moveChildren = new AMListAction3(new AMListActionInfo3("Moving bistate group.", "Moving bistate group."), AMListAction3::Parallel);

	foreach (AMControl *child, children_) {
		if (child)
			moveChildren->addSubAction(createMoveChildToState2Action(child));
	}

	return moveChildren;
}

AMAction3* BioXASBiStateGroup::createMoveChildToState1Action(AMControl *child)
{
	AMAction3 *result = 0;

	if (child && controlState1ValueMap_.contains(child))
		result = AMActionSupport::buildControlMoveAction(child, controlState1ValueMap_.value(child));

	return result;
}

AMAction3* BioXASBiStateGroup::createMoveChildToState2Action(AMControl *child)
{
	AMAction3 *result = 0;

	if (child && controlState2ValueMap_.contains(child))
		result = AMActionSupport::buildControlMoveAction(child, controlState2ValueMap_.value(child));

	return result;
}
