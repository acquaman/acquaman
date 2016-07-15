#include "CLSTriStateGroup.h"

#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

CLSTriStateGroup::CLSTriStateGroup(const QString &name, QObject *parent) :
	CLSBiStateGroup(name, parent)
{

}

CLSTriStateGroup::~CLSTriStateGroup()
{

}

bool CLSTriStateGroup::addTriStateControl(AMControl *control, double state1Value, double state2Value)
{
	bool result = false;

	if (addBiStateControl(control, state1Value)) {
		controlState2ValueMap_.insert(control, state2Value);
		result = true;
	}

	return result;
}

bool CLSTriStateGroup::removeTriStateControl(AMControl *control)
{
	bool result = false;

	if (removeBiStateControl(control)) {
		controlState2ValueMap_.remove(control);
		result = true;
	}

	return result;
}

bool CLSTriStateGroup::clearTriStateControls()
{
	bool result = false;

	if (!children_.isEmpty()) {
		clearBiStateControls();
		controlState2ValueMap_.clear();
		result = true;
	}

	return result;
}

QList<AMControl*> CLSTriStateGroup::childrenInState2() const
{
	QList<AMControl*> result;

	// Iterate through list of children, finding those that
	// are in state 2.

	QList<AMControl*> children = childControls();

	foreach (AMControl *child, children) {
		if (isChildState2(child))
			result << child;
	}

	return result;
}

QList<AMControl*> CLSTriStateGroup::childrenNotInState2() const
{
	QList<AMControl*> result;

	// Iterate through list of children, finding those that
	// are not in state 2.

	QList<AMControl*> children = childControls();

	foreach (AMControl *child, children) {
		if (!isChildState2(child))
			result << child;
	}

	return result;
}

bool CLSTriStateGroup::areAnyChildrenState2() const
{
	bool result = false;

	// Iterate through list of children, finding out if
	// any are are state 2.

	QList<AMControl*> children = childControls();

	if (children.count() > 0) {
		bool childFound = false;

		for (int i = 0, count = children.count(); i < count && !childFound; i++) { // we want to stop if any one child is in state 2.
			AMControl *child = children.at(i);

			if (child && isChildState2(child))
				childFound = true;
		}

		// If a child in state 2 was found, then there is
		// at least one child in state 2.

		if (childFound)
			result = true;
	}

	return result;
}

bool CLSTriStateGroup::areAllChildrenState2() const
{
	bool result = false;

	// Iterate through list of children, finding out if
	// all are are state 2.

	QList<AMControl*> children = childControls();

	if (children.count() > 0) {
		bool childFound = false;

		for (int i = 0, count = children.count(); i < count && !childFound; i++) { // we want to stop if any one child isn't in state 2.
			AMControl *child = children.at(i);

			if (child && !isChildState2(child))
				childFound = true;
		}

		// If no children were found that were in another state,
		// all children are in state 2.

		if (!childFound)
			result = true;
	}

	return result;
}

#include <QDebug>
bool CLSTriStateGroup::isChildState2(AMControl *child) const
{
	bool result = false;

	if (child && child->isConnected() && controlState2ValueMap_.keys().contains(child)) {
		qDebug() << "CLSTriStateGroup::isChildState2() " << child << child->value() << controlState2ValueMap_.value(child);
		if (child->value() == controlState2ValueMap_.value(child))
			result = true;
	}

	return result;
}

AMAction3* CLSTriStateGroup::createMoveChildrenToState2Action()
{
	AMListAction3 *moveChildren = new AMListAction3(new AMListActionInfo3("Moving bistate group.", "Moving bistate group."), AMListAction3::Parallel);

	foreach (AMControl *child, children_) {
		AMAction3 *action = 0;

		if (child) {
			if (child->canMove())
				action = createMoveChildToState2Action(child);
			else
				action = createCheckChildAtState2Action(child);
		}
	}

	return moveChildren;
}

AMAction3* CLSTriStateGroup::createMoveChildToState2Action(AMControl *child)
{
	AMAction3 *result = 0;

	if (child && child->isConnected() && controlState2ValueMap_.contains(child))
		result = AMActionSupport::buildControlMoveAction(child, controlState2ValueMap_.value(child));

	return result;
}

AMAction3* CLSTriStateGroup::createCheckChildAtState2Action(AMControl *child, double timeoutSec)
{
	AMAction3 *result = 0;

	if (child && child->isConnected() && controlState2ValueMap_.contains(child))
		result = AMActionSupport::buildControlMoveAction(child, controlState2ValueMap_.value(child), timeoutSec);

	return result;
}
