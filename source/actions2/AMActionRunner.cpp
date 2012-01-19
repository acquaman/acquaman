#include "AMActionRunner.h"
#include "actions2/AMAction.h"
#include "actions2/AMActionLog.h"

#include "util/AMErrorMonitor.h"

#include <QStringBuilder>
#include <QPixmapCache>

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
	queueModel_->insertRow(index, new AMActionQueueModelItem(action));
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
	AMAction* action = static_cast<AMActionQueueModelItem*>(queueModel_->item(index))->action();
	queueModel_->removeRow(index);
	delete action;
	// emit actionRemoved(index);

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
	queueModel_->insertRow(finalIndex, item);
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

		// todozzzz Handle special situation if newAction is a LoopAction: don't remove it.
		// emit actionAboutToBeRemoved(0);
		AMAction* newAction = queuedActionAt(0);
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

		currentAction_->start();
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

	// todozzz For loop items, should be drop-enabled to accept dropping actions into them?);
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

