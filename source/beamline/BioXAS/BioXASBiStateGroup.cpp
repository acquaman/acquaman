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

bool BioXASBiStateGroup::isOpen() const
{
	bool result = false;

	// Iterate through list of children, finding out if
	// all are open.

	QList<AMControl*> children = childControls();

	if (children.count() > 0) {
		bool open = true;

		for (int i = 0, count = children.count(); i < count && open; i++) { // we want to stop if any one child isn't open.
			AMControl *child = children.at(i);

			if (child && isChildOpen(child))
				open = true;
			else
				open = false;
		}

		result = open;
	}

	return result;
}

bool BioXASBiStateGroup::isClosed() const
{
	bool result = false;

	// Iterate through list of children, finding out if
	// any are closed.

	QList<AMControl*> children = childControls();

	if (children.count() > 0) {
		bool closed = true;

		for (int i = 0, count = children.count(); i < count && closed; i++) { // we want to stop if any one child isn't closed.
			AMControl *child = children.at(i);

			if (child && isChildClosed(child))
				closed = true;
			else
				closed = false;
		}

		result = closed;
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

bool BioXASBiStateGroup::addBiStateControl(AMControl *control, double openValue, double closeValue)
{
	bool result = false;

	if (control) {
		controlOpenValueMap_.insert(control, openValue);
		controlCloseValueMap_.insert(control, closeValue);

		addChildControl(control);

		result = true;
	}

	return result;
}

bool BioXASBiStateGroup::removeBiStateControl(AMControl *control)
{
	bool result = false;

	if ( control && (controlOpenValueMap_.contains(control) || controlCloseValueMap_.contains(control)) ) {
		controlOpenValueMap_.remove(control);
		controlCloseValueMap_.remove(control);

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

bool BioXASBiStateGroup::isChildState1(AMControl *child) const
{
	bool result = false;

	if (child && controlOpenValueMap_.keys().contains(child)) {
		if (child->value() == controlOpenValueMap_.value(child))
			result = true;
	}

	return result;
}

bool BioXASBiStateGroup::isChildState2(AMControl *child) const
{
	bool result = false;

	if (child && controlCloseValueMap_.keys().contains(child)) {
		if (child->value() == controlCloseValueMap_.value(child))
			result = true;
	}

	return result;
}

//AMAction3* BioXASBiStateGroup::createMoveAction(double setpoint)
//{
//	AMAction3 *result = 0;

//	switch (int(setpoint)) {
//	case Open:
//		result = createMoveToOpenAction();
//		break;
//	case Closed:
//		result = createMoveToClosedAction();
//		break;
//	default:
//		break;
//	}

//	return result;
//}

//AMAction3* BioXASBiStateGroup::createMoveToOpenAction()
//{
//	AMListAction3 *actionList = new AMListAction3(new AMListActionInfo3("Open bistate group", "Open bistate group"), AMListAction3::Parallel);

//	foreach (AMControl *child, childControls())
//		actionList->addSubAction(createOpenChildAction(child));

//	return actionList;
//}

//AMAction3* BioXASBiStateGroup::createMoveToClosedAction()
//{
//	AMListAction3 *actionList = new AMListAction3(new AMListActionInfo3("Close bistate group", "Close bistate group"), AMListAction3::Parallel);

//	foreach (AMControl *child, childControls())
//		actionList->addSubAction(createCloseChildAction(child));

//	return actionList;
//}

AMAction3* BioXASBiStateGroup::createMoveChildToState1Action(AMControl *child)
{
	AMAction3 *result = 0;

	if (child && controlOpenValueMap_.contains(child))
		result = AMActionSupport::buildControlMoveAction(child, controlOpenValueMap_.value(child));

	return result;
}

AMAction3* BioXASBiStateGroup::createMoveChildToState2Action(AMControl *child)
{
	AMAction3 *result = 0;

	if (child && controlCloseValueMap_.contains(child))
		result = AMActionSupport::buildControlMoveAction(child, controlCloseValueMap_.value(child));

	return result;
}

int BioXASBiStateGroup::currentIndex() const
{
	int result = indicesNamed("Unknown").first();

//	if (isOpen())
//		result = Open;
//	else if (isClosed())
//		result = Closed;

	return result;
}
