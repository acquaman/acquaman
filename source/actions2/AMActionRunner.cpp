#include "AMActionRunner.h"
#include "actions2/AMAction.h"
#include "actions2/AMActionLog.h"

#include "util/AMErrorMonitor.h"

#include <QStringBuilder>
#include <QPixmapCache>
#include <QTimer>

#include "actions2/AMNestedAction.h"

#include <QDebug>

AMActionRunner* AMActionRunner::instance_ = 0;

AMActionRunner::AMActionRunner(QObject *parent) :
	QObject(parent)
{
	currentAction_ = 0;
	isPaused_ = true;
	queueModel_ = new AMActionQueueModel(this, this);
}

AMActionRunner::~AMActionRunner() {
	while(queuedActionCount() != 0) {
		deleteActionInQueue(queuedActionCount()-1);
	}
}

AMActionRunner * AMActionRunner::s()
{
	if(!instance_)
		instance_ = new AMActionRunner();
	return instance_;
}

void AMActionRunner::releaseActionRunner()
{
	delete instance_;
	instance_ = 0;
}

void AMActionRunner::onCurrentActionStateChanged(int state, int previousState)
{
	emit currentActionStateChanged(state, previousState);

	if(state == AMAction::Failed) {
		// What should we do?
		int failureResponse = currentAction_->failureResponseInActionRunner();
		if(failureResponse == AMAction::PromptUserResponse)
			failureResponse = internalAskUserWhatToDoAboutFailedAction(currentAction_);

		if(failureResponse == AMAction::AttemptAnotherCopyResponse) {
			// make a fresh copy of this action and put it at the front of the queue to run again.
			insertActionInQueue(currentAction_->createCopy(), 0);
		}
		// other failure response is to MoveOn, which we'll do anyway.
	}

	if(state == AMAction::Cancelled) {
		// Usability guess for now: If the user intervened to cancel, they probably want to make some changes to something. Let's pause the workflow for now to let them do that, rather than automatically go on to the next.
		setQueuePaused(true);
	}

	// for all three final states, this is how we wrap things up for the current action:
	if(state == AMAction::Failed ||
			state == AMAction::Cancelled ||
			state == AMAction::Succeeded) {

		// log it:
		if(!AMActionLog::logCompletedAction(currentAction_)) {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -200, "There was a problem logging the completed action to your database.  Please report this problem to the Acquaman developers."));
		}

		// move onto the next, if there is one, and disconnect and delete the old one.
		internalDoNextAction();
	}
}

void AMActionRunner::insertActionInQueue(AMAction *action, int index)
{
	if(index < 0 || index > queueModel_->rowCount())
		index = queueModel_->rowCount();

	// emit actionAboutToBeAdded(index);
	AMActionQueueModelItem* item = new AMActionQueueModelItem(action);
	queueModel_->insertRow(index, item);
	internalBuildNestedActionItemsIfRequired(item);
	// emit actionAdded(index);

	// was this the first action inserted into a running but empty queue? Start it up!
	if(!isPaused_ && !actionRunning())
		internalDoNextAction();
}

bool AMActionRunner::deleteActionInQueue(int index)
{
	if(index<0 || index>=queueModel_->rowCount())
		return false;

	// emit actionAboutToBeRemoved(index);
	AMActionQueueModelItem* item = static_cast<AMActionQueueModelItem*>(queueModel_->item(index));
	AMAction* action = item->action();
	internalCleanUpNestedActionIndexes(item);	// disconnect and remove our tracking of a nested action, if this is one.
	queueModel_->removeRow(index);
	// emit actionRemoved(index);

	delete action;

	return true;
}

bool AMActionRunner::duplicateActionInQueue(int index)
{
	if(index<0 || index>=queueModel_->rowCount())
		return false;

	insertActionInQueue(static_cast<AMActionQueueModelItem*>(queueModel_->item(index))->action()->createCopy(), index+1);
	return true;
}

bool AMActionRunner::duplicateActionsInQueue(const QList<int> &indexesToCopy)
{
	if(indexesToCopy.isEmpty())
		return false;

	// find max index, and return false if any are out of range.
	int maxIndex = indexesToCopy.at(0);
	if(maxIndex < 0 || maxIndex >= queuedActionCount())
		return false;

	for(int i=1, cc=indexesToCopy.count(); i<cc; i++) {
		int index = indexesToCopy.at(i);
		if(index < 0 || index >= queuedActionCount())
			return false;
		if(index > maxIndex)
			maxIndex = index;
	}

	// note: because we're inserting at higher indexes than any of indexesToCopy, we know that any that we're supposed to copy won't change positions as we start inserting.
	foreach(int index, indexesToCopy) {
		insertActionInQueue(static_cast<AMActionQueueModelItem*>(queueModel_->item(index))->action()->createCopy(),
							++maxIndex);
	}
	return true;
}

bool AMActionRunner::moveActionInQueue(int currentIndex, int finalIndex)
{
	if(currentIndex == finalIndex)
		return false;

	if(currentIndex < 0 || currentIndex >= queuedActionCount())
		return false;

	if(finalIndex <0 || finalIndex >= queuedActionCount())
		finalIndex = queuedActionCount()-1;

	// emit actionAboutToBeRemoved(currentIndex);
	QStandardItem* item = queueModel_->item(currentIndex);
	queueModel_->removeRow(currentIndex);
	// emit actionRemoved(currentIndex);

	// emit actionAboutToBeAdded(finalIndex);
	queueModel_->insertRow(finalIndex, item);	// note: in the case of nested actions, we're taking the whole item with us, so we should be OK... If it does represent a nested action, it will keep its whole tree with it.
	// emit actionAdded(finalIndex);

	return true;
}

void AMActionRunner::setQueuePaused(bool isPaused) {
	if(isPaused == isPaused_)
		return;

	emit queuePausedChanged(isPaused_ = isPaused);

	// if we've gone from paused to running, and there is no action running right now, and we have actions we can run... Start the next.
	if(!isPaused_ && queuedActionCount() != 0 && !actionRunning())
		internalDoNextAction();
}

void AMActionRunner::internalDoNextAction()
{
	// signal that no action is running? (queue paused, or we're out of actions in the queue to run...)
	if(isPaused_ || queuedActionCount() == 0) {
		if(currentAction_) {
			AMAction* oldAction = currentAction_;
			emit currentActionChanged(currentAction_ = 0);
			oldAction->deleteLater(); // the action might have sent notifyFailed() or notifySucceeded() in the middle a deep call stack... In that case, we might actually still be executing inside the action's code, so better not delete it until that all has a chance to finish. (Otherwise... Crash!)
		}
	}

	// Otherwise, keep on keepin' on. Disconnect and delete the old current action (if there is one... If we're resuming from pause or starting for the first time, there won't be)., and connect and start the next one.
	else {
		AMAction* oldAction = currentAction_;

		// emit actionAboutToBeRemoved(0);
		AMAction* newAction = queuedActionAt(0);
		internalCleanUpNestedActionIndexes(static_cast<AMActionQueueModelItem*>(queueModel_->item(0)));
		queueModel_->removeRow(0);
		// emit actionRemoved(0);

		emit currentActionChanged(currentAction_ = newAction);

		if(oldAction) {
			disconnect(oldAction, 0, this, 0);
			oldAction->deleteLater();	// the action might have sent notifyFailed() or notifySucceeded() in the middle a deep call stack... In that case, we might actually still be executing inside the action's code, so better not delete it until that all has a chance to finish. (Otherwise... Crash!)
		}

		connect(currentAction_, SIGNAL(stateChanged(int,int)), this, SLOT(onCurrentActionStateChanged(int,int)));
		connect(currentAction_, SIGNAL(progressChanged(double,double)), this, SIGNAL(currentActionProgressChanged(double,double)));
		connect(currentAction_, SIGNAL(statusTextChanged(QString)), this, SIGNAL(currentActionStatusTextChanged(QString)));
		connect(currentAction_, SIGNAL(expectedDurationChanged(double)), this, SIGNAL(currentActionExpectedDurationChanged(double)));

		// to avoid a growing call stack if a long series of actions are all failing inside their start() method... We wait to run the next one until we get back to the even loop.
		QTimer::singleShot(0, currentAction_, SLOT(start()));
	}
}

// This is a non-GUI class, and we're popping up a QMessageBox. Not ideal; hope you'll let us get away with that.

#include <QMessageBox>
#include <QPushButton>
int AMActionRunner::internalAskUserWhatToDoAboutFailedAction(AMAction* action)
{
	QMessageBox box;
	box.setWindowTitle("Sorry! An action failed. What should we do?");
	box.setText("A '" % action->info()->typeDescription() % "' action in the workflow didn't complete successfully. What should we do?");
	box.setInformativeText("Action: " % action->info()->shortDescription() % "\n\nYou can try it again, or give up and move on to the next action in the workflow.");

	QPushButton* moveOnButton = new QPushButton("Move on to next action");
	QPushButton* tryAgainButton = new QPushButton("Try action again");

	box.addButton(moveOnButton, QMessageBox::RejectRole);
	box.addButton(tryAgainButton, QMessageBox::AcceptRole);
	box.setDefaultButton(tryAgainButton);

	box.exec();
	if(box.clickedButton() == tryAgainButton)
		return AMAction::AttemptAnotherCopyResponse;
	else
		return AMAction::MoveOnResponse;
}

void AMActionRunner::runActionImmediately(AMAction *action)
{
	connect(action, SIGNAL(stateChanged(int,int)), this, SLOT(onImmediateActionStateChanged(int,int)));
	immediateActions_ << action;
	action->start();
}

void AMActionRunner::onImmediateActionStateChanged(int state, int previousState)
{
	Q_UNUSED(previousState)

	AMAction* action = qobject_cast<AMAction*>(sender());
	if(!action)
		return;

	if(state == AMAction::Failed) {
		// What should we do?
		int failureResponse = action->failureResponseInActionRunner();
		if(failureResponse == AMAction::PromptUserResponse)
			failureResponse = internalAskUserWhatToDoAboutFailedAction(action);

		if(failureResponse == AMAction::AttemptAnotherCopyResponse) {
			// make a fresh copy of this action and run it now
			AMAction* retryAction = action->createCopy();
			connect(retryAction, SIGNAL(stateChanged(int,int)), this, SLOT(onImmediateActionStateChanged(int,int)));
			retryAction->start();
		}
	}

	// for all three final states, this is how we wrap things up for the current action:
	if(state == AMAction::Failed ||
			state == AMAction::Cancelled ||
			state == AMAction::Succeeded) {

		// remove from the list of current immediate actions
		immediateActions_.removeAll(action);

		// log it:
		if(!AMActionLog::logCompletedAction(action)) {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -201, "There was a problem logging the completed action to your database.  Please report this problem to the Acquaman developers."));
		}

		// disconnect and delete it
		disconnect(action, 0, this, 0);
		delete action;
	}
}


AMActionQueueModelItem::AMActionQueueModelItem(AMAction *action) : QStandardItem()
{
	action_ = action;
	setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);

}


QVariant AMActionQueueModelItem::data(int role) const
{
	if(role == Qt::DisplayRole) {
		return action_->info()->shortDescription();
	}
	else if(role == Qt::DecorationRole) {
		QPixmap p;
		QString iconFileName = action_->info()->iconFileName();
		if(QPixmapCache::find("AMActionIcon" % iconFileName, &p))
			return p;
		else {
			p.load(iconFileName);
			p = p.scaledToHeight(32, Qt::SmoothTransformation);
			QPixmapCache::insert("AMActionIcon" % iconFileName, p);
			return p;
		}
	}
	else if(role == Qt::SizeHintRole) {
		return QSize(-1, 48);
	}
	return QStandardItem::data(role);
}

AMActionQueueModel::AMActionQueueModel(AMActionRunner *actionRunner, QObject *parent) :
	QStandardItemModel(parent)
{
	actionRunner_ = actionRunner;
}
bool AMActionQueueModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	Q_UNUSED(column)

	// Option 1: Handle internal move actions, where the \c data is a list of the qmodelindexes to move
	if(action == Qt::MoveAction) {
		const AMModelIndexListMimeData* milData = qobject_cast<const AMModelIndexListMimeData*>(data);
		if(milData) {
			QList<QPersistentModelIndex> mil = milData->modelIndexList();
			// go through and make sure all indexes are valid, and have the same parent index. If we're asked to move multiple indexes from different levels of the hierarchy, I have no idea how we'd interpret that.
			if(mil.isEmpty())
				return false;	// nothing to do!
			QPersistentModelIndex first = mil.at(0);
			foreach(const QPersistentModelIndex& mi, mil) {
				if(!mi.isValid()) {
					return false;	// this item was removed from the model while the drag was in progress.  Be faster next time, user.
				}
				if(mi.parent() != first.parent())
					return false;	// we need all indexes to move to be at the same level of the hierarchy.
			}
			// OK, these indexes are OK to use.

			// Subcase A: If these are all top-level actions in the queue, we can move them using the AMActionRunner API.
			if(!first.parent().isValid()) {
				// destination: if they are being moved to another location at the top level:
				if(!parent.isValid()) {
					// get the row (index) of each action, and sort them in ascending order
					QList<int> indexesToMove;
					foreach(const QPersistentModelIndex& pmi, mil) {
						QModelIndex mi = pmi; // convert back to current index to get the row.
						indexesToMove << mi.row();
					}
					qSort(indexesToMove);

					QList<AMAction*> actionsToMove;
					foreach(int i, indexesToMove)
						actionsToMove << actionRunner_->queuedActionAt(i);

					// go from highest index to lowest index, inserting at the destination row
					for(int i=actionsToMove.count()-1; i>=0; i--)
						actionRunner_->insertActionInQueue(actionsToMove.at(i), row);

					// now here's the funny thing: The view's drag action will now delete rows from the model corresponding to where these all came from. (Hopefully it uses persistent indexes for this!) Normally it's not good to delete rows using the model interface, because we haven't deleted the corresponding actions in the AMActionRunner. But here we've actually created duplicate model entries for each action, so this is exactly the behaviour we want.
					return true;
				}
				// todozzzz destination: being moved into a loop action
			}
			// todozzz Subcase B: These are sub-actions inside a loop action... Grab the loop action and rearrange them.
		}
	}

	return false;
}

QMimeData * AMActionQueueModel::mimeData(const QModelIndexList &indexes) const
{
	return new AMModelIndexListMimeData(indexes);
}

QStringList AMActionQueueModel::mimeTypes() const
{
	return QStringList() << "application/octet-stream";
}

bool AMActionQueueModel::removeRows(int row, int count, const QModelIndex &parent)
{
	// qDebug() << "What? someone's calling removeRows on the model" << row << count << parent;
	return QStandardItemModel::removeRows(row, count, parent);
}

AMAction * AMActionRunner::immediateActionAt(int index)
{
	if(index < 0 || index >= immediateActions_.count()) {
		return 0;
	}
	return immediateActions_.at(index);
}

bool AMActionRunner::cancelCurrentAction()
{
	if(currentAction_) {
		currentAction_->cancel();
		return true;
	}
	return false;
}

bool AMActionRunner::pauseCurrentAction()
{
	if(currentAction_ && currentAction_->canPause()) {
		return currentAction_->pause();
	}
	return false;
}

bool AMActionRunner::resumeCurrentAction()
{
	if(currentAction_ && currentAction_->state() == AMAction::Paused) {
		return currentAction_->resume();
	}
	return false;
}

bool AMActionRunner::cancelImmediateActions()
{
	if(immediateActions_.isEmpty())
		return false;

	foreach(AMAction* action, immediateActions_)
		action->cancel();
	return true;
}

void AMActionRunner::internalBuildNestedActionItemsIfRequired(AMActionQueueModelItem *parentItem)
{
	AMNestedAction* nestedAction = qobject_cast<AMNestedAction*>(parentItem->action());
	if(nestedAction) {
		nestedActions_.insert(nestedAction, QPersistentModelIndex(parentItem->index()));
		connect(nestedAction, SIGNAL(subActionAdded(int)), this, SLOT(onNestedActionSubActionAdded(int)));
		connect(nestedAction, SIGNAL(subActionAboutToBeRemoved(int)), this, SLOT(onNestedActionSubActionAboutToBeRemoved(int)));
		// todozzz More signals?
		for(int i=0, cc=nestedAction->subActionCount(); i<cc; i++) {
			AMActionQueueModelItem* subItem = new AMActionQueueModelItem(nestedAction->subActionAt(i));
			parentItem->appendRow(subItem);
			internalBuildNestedActionItemsIfRequired(subItem);
		}
	}
}

void AMActionRunner::internalCleanUpNestedActionIndexes(AMActionQueueModelItem *parentItem)
{
	AMNestedAction* nestedAction = qobject_cast<AMNestedAction*>(parentItem->action());
	if(nestedAction) {
		nestedActions_.remove(nestedAction);
		disconnect(nestedAction, 0, this, 0);
		for(int i=0, cc=parentItem->rowCount(); i<cc; i++)
			internalCleanUpNestedActionIndexes(static_cast<AMActionQueueModelItem*>(parentItem->child(i)));
	}
}


void AMActionRunner::onNestedActionSubActionAdded(int index)
{
	AMNestedAction* nestedAction = qobject_cast<AMNestedAction*>(sender());
	if(!nestedAction)
		return;

	// find out the index and item representing this nested action
	AMActionQueueModelItem* parentItem = static_cast<AMActionQueueModelItem*>(queueModel_->itemFromIndex(nestedActions_.value(nestedAction)));
	if(!parentItem) {
		// This isn't a user-visible error... It should never happen and is only for developers, which is why it doesn't use AMErrorMon.
		qWarning() << "AMActionRunner: Warning: detected corruption in its tracking of nested actions. No index found for sender when adding a sub-action.";
		return;
	}

	// insert a child item in the model corresponding to the new subAction
	AMAction* subAction = nestedAction->subActionAt(index);
	if(!subAction) {
		qWarning() << "AMActionRunner: Warning: A subclass of AMNestedAction returned us an invalid subAction at index" << index;
		return;
	}
	AMActionQueueModelItem* item = new AMActionQueueModelItem(subAction);
	parentItem->insertRow(index, item);
	internalBuildNestedActionItemsIfRequired(item);	// this will recursively insert the subAction's sub-actions, if it has any.

}

void AMActionRunner::onNestedActionSubActionAboutToBeRemoved(int index)
{
	AMNestedAction* nestedAction = qobject_cast<AMNestedAction*>(sender());
	if(!nestedAction)
		return;

	// find out the index and item representing this nested action
	AMActionQueueModelItem* parentItem = static_cast<AMActionQueueModelItem*>(queueModel_->itemFromIndex(nestedActions_.value(nestedAction)));
	if(!parentItem) {
		// This isn't a user-visible error... It should never happen and is only for developers, which is why it doesn't use AMErrorMon.
		qWarning() << "AMActionRunner: Warning: detected corruption in its tracking of nested actions. No index found for sender when removing a sub-action.";
		return;
	}

	// remove the child item from the model. If _it_ is a nested action, need to do some cleanup too.
	AMActionQueueModelItem* subItem = static_cast<AMActionQueueModelItem*>(parentItem->child(index));
	if(!subItem) {
		qWarning() << "AMActionRunner: Warning: No sub-action to remove at the index " << index;
		return;
	}
	// We're going to remove subItem, so if it represents a nested action, we'll have to clean those out recursively.
	internalCleanUpNestedActionIndexes(subItem);
	// and finally, take this item out of the model.
	parentItem->removeRow(index);

	// We trust that the Nested Action will take care of deleting its now-removed sub-action, since it was the one that notified us of this deletion in the first place.
}

void AMActionRunner::deleteActionsInQueue(QModelIndexList indexesToDelete)
{
	QList<int> topLevelIndexesToDelete;
	QList<QPersistentModelIndex> nestedIndexesToDelete;

	foreach(QModelIndex i, indexesToDelete) {
		// Sort out the top-level actions...
		if(!i.parent().isValid()) {
			topLevelIndexesToDelete << i.row();
		}
		// ... from those that are inside an AMNestedAction
		else {
			nestedIndexesToDelete << QPersistentModelIndex(i);
		}
	}
	// Delete the top-level actions. Need to delete from largest index to smallest index, otherwise the indexes will change as we go. [We could use persistent indexes to avoid this problem, but the performance is better if we don't.
	qSort(topLevelIndexesToDelete);
	for(int i=topLevelIndexesToDelete.count()-1; i>=0; i--)
		deleteActionInQueue(topLevelIndexesToDelete.at(i));

	// Now, do we have any nested actions left to delete? They might have been deleted if they were inside parents that were deleted, so make sure to check for valid persistent indexes.
	foreach(QPersistentModelIndex i, nestedIndexesToDelete) {
		if(i.isValid()) {
			// get their parent model item... which should represent a nested action
			AMActionQueueModelItem* nestedItem = static_cast<AMActionQueueModelItem*>(queueModel_->itemFromIndex(i.parent()));
			if(nestedItem) {
				AMNestedAction* nestedAction = qobject_cast<AMNestedAction*>(nestedItem->action());
				if(nestedAction) {
					nestedAction->deleteSubAction(i.row());
				}
			}
		}
	}
}


void AMActionRunner::duplicateActionsInQueue(const QModelIndexList &indexesToCopy) {
	// nothing to do:
	if(indexesToCopy.isEmpty())
		return;

	// This function works if all the indexes are at the same level of the model tree hierarchy.  Let's confirm that first.
	QModelIndex firstParent = indexesToCopy.at(0).parent();
	foreach(QModelIndex i, indexesToCopy)
		if(i.parent() != firstParent)
			return;

	// OK.  Are they all top-level indexes?
	if(!firstParent.isValid()) {
		QList<int> topLevelIndexes;
		foreach(QModelIndex i, indexesToCopy)
			topLevelIndexes << i.row();
		// call the top-level version:
		duplicateActionsInQueue(topLevelIndexes);
	}
	// Otherwise, these indexes are sub-actions of some AMNestedAction
	else {
		// find the nested action:
		AMActionQueueModelItem* parentItem = static_cast<AMActionQueueModelItem*>(queueModel_->itemFromIndex(firstParent));
		if(parentItem) {
			AMNestedAction* nestedAction = qobject_cast<AMNestedAction*>(parentItem->action());
			if(nestedAction) {
				QList<int> subActionIndexes;
				foreach(QModelIndex i, indexesToCopy)
					subActionIndexes << i.row();
				// use the AMNestedAction API to duplicate these subactions.
				nestedAction->duplicateSubActions(subActionIndexes);
			}
		}
	}
}

