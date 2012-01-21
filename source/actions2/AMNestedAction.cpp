#include "AMNestedAction.h"

bool AMNestedAction::insertSubAction(AMAction *action, int index)
{
	if(state() != Constructed)
		return false;

	if(index < 0 || index > subActionCount())
		index = subActionCount();

	emit subActionAboutToBeAdded(index);
	insertSubActionImplementation(action, index);
	emit subActionAdded(index);

	return true;
}

bool AMNestedAction::deleteSubAction(int index)
{
	if(state() != Constructed)
		return false;

	if(index < 0 || index >= subActionCount())
		return false;

	emit subActionAboutToBeRemoved(index);
	deleteSubActionImplementation(index);
	emit subActionRemoved(index);

	return true;
}

bool AMNestedAction::duplicateSubActions(const QList<int> &indexesToCopy)
{
	if(state() != Constructed)
		return false;

	if(indexesToCopy.isEmpty())
		return true;	// done

	// sort the list, so we know the highest index in it.
	QList<int> sIndexesToCopy = indexesToCopy;
	qSort(sIndexesToCopy);

	// any indexes out of range?
	if(sIndexesToCopy.first() < 0)
		return false;
	if(sIndexesToCopy.last() >= subActionCount())
		return false;

	// insert at the position after last existing subAction to copy
	int insertionIndex = sIndexesToCopy.last() + 1;

	// insert copies of all, using regular insertSubAction().
	foreach(int i, sIndexesToCopy)
		insertSubAction(subActionAt(i)->createCopy(), insertionIndex++);

	return true;
}
