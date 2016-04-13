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
		// any can be moved.

		QList<AMControl*> children = childControls();

		if (children.count() > 0) {
			bool movable = false;

			for (int i = 0, count = children.count(); i < count && !movable; i++) { // we want to stop if we find one child that can be moved.
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
		// those that can move can also be stopped.

		QList<AMControl*> children = childControls();

		if (children.count() > 0) {
			bool childrenValid = true;
			bool childrenStoppable = true;

			for (int i = 0, count = children.count(); i < count && childrenValid && childrenStoppable; i++) { // we want to stop if we come across any invalid children or children that 'should' be stoppable but aren't.
				AMControl *child = children.at(i);

				if (child) {
					if (child->canMove() && !child->canStop()) // we want to know if we have any children that can move but can't be stopped. Potential problem!
						childrenStoppable = false;

				} else {
					childrenValid = false; // we want to know if we have any children that aren't valid. Another potential problem.
				}
			}

			result = childrenValid && childrenStoppable;
		}
	}

	return result;
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

bool BioXASBiStateGroup::addBiStateControl(AMControl *control, double state1Value)
{
	bool result = false;

	if (control && !children_.contains(control)) {
		addChildControl(control);
		controlState1ValueMap_.insert(control, state1Value);

		result = true;
	}

	return result;
}

bool BioXASBiStateGroup::removeBiStateControl(AMControl *control)
{
	bool result = false;

	if (control && children_.contains(control)) {
		removeChildControl(control);
		controlState1ValueMap_.remove(control);

		result = true;
	}

	return result;
}

bool BioXASBiStateGroup::clearBiStateControls()
{
	bool result = false;

	if (!children_.isEmpty()) {
		clearChildControls();
		controlState1ValueMap_.clear();

		result = true;
	}

	return result;
}

QList<AMControl*> BioXASBiStateGroup::childrenInState1() const
{
	QList<AMControl*> result;

	// Iterate through list of children, finding those that
	// are in state 1.

	QList<AMControl*> children = childControls();

	foreach (AMControl *child, children) {
		if (isChildState1(child))
			result << child;
	}

	return result;
}

QList<AMControl*> BioXASBiStateGroup::childrenNotInState1() const
{
	QList<AMControl*> result;

	// Iterate through list of children, finding those that
	// are not in state 1.

	QList<AMControl*> children = childControls();

	foreach (AMControl *child, children) {
		if (!isChildState1(child))
			result << child;
	}

	return result;
}

bool BioXASBiStateGroup::areAnyChildrenState1() const
{
	bool result = false;

	// Iterate through list of children, finding out if
	// any are in state 1.

	QList<AMControl*> children = childControls();

	if (children.count() > 0) {
		bool childFound = false;

		for (int i = 0, count = children.count(); i < count && !childFound; i++) { // we want to stop if any one child is in state 1.
			AMControl *child = children.at(i);

			if (child && isChildState1(child))
				childFound = true;
		}

		// If a child in state 1 was found, then there is
		// at least one child in state 1.

		if (childFound)
			result = true;
	}

	return result;
}

bool BioXASBiStateGroup::areAllChildrenState1() const
{
	bool result = false;

	// Iterate through list of children, finding out if
	// all are in state 1.

	QList<AMControl*> children = childControls();

	if (children.count() > 0) {
		bool childFound = false;

		for (int i = 0, count = children.count(); i < count && !childFound; i++) { // we want to stop if any one child isn't in state 1.
			AMControl *child = children.at(i);

			if (child && !isChildState1(child))
				childFound = true;
		}

		// If no children were found that were in another state,
		// all children are in state 1.

		if (!childFound)
			result = true;
	}

	return result;
}

bool BioXASBiStateGroup::isChildState1(AMControl *child) const
{
	bool result = false;

	if (child && child->isConnected() && controlState1ValueMap_.keys().contains(child)) {
		if (child->value() == controlState1ValueMap_.value(child))
			result = true;
	}

	return result;
}

AMAction3* BioXASBiStateGroup::createMoveChildrenToState1Action()
{
	AMListAction3 *moveChildren = new AMListAction3(new AMListActionInfo3("Moving bistate group.", "Moving bistate group."), AMListAction3::Parallel);

	foreach (AMControl *child, children_) {
		AMAction3 *action = 0;

		if (child) {
			if (child->canMove())
				action = createMoveChildToState1Action(child);
			else
				action = createCheckChildAtState1Action(child);
		}

		moveChildren->addSubAction(action);
	}

	return moveChildren;
}

AMAction3* BioXASBiStateGroup::createMoveChildToState1Action(AMControl *child)
{
	AMAction3 *result = 0;

	if (child && child->isConnected() && controlState1ValueMap_.contains(child))
		result = AMActionSupport::buildControlMoveAction(child, controlState1ValueMap_.value(child));

	return result;
}

AMAction3* BioXASBiStateGroup::createCheckChildAtState1Action(AMControl *child, double timeoutSec)
{
	AMAction3 *result = 0;

	if (child && child->isConnected() && controlState1ValueMap_.contains(child))
		result = AMActionSupport::buildControlWaitAction(child, controlState1ValueMap_.value(child), timeoutSec);

	return result;
}
