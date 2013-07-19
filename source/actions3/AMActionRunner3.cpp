/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMActionRunner3.h"

#include <QStringBuilder>
#include <QPixmapCache>
#include <QTimer>

#include "actions3/AMListAction3.h"
#include "actions3/AMLoopAction3.h"
#include "actions3/AMActionRegistry3.h"
#include "actions3/AMAction3.h"
#include "actions3/AMActionLog3.h"

#include "util/AMErrorMonitor.h"
#include "acquaman/AMScanController.h"

#include "application/AMAppControllerSupport.h"
#include "ui/actions3/AMActionHistoryModel.h"

AMActionRunner3* AMActionRunner3::workflowInstance_ = 0;
AMActionRunner3* AMActionRunner3::scanActionRunnerInstance_ = 0;

AMActionRunner3::AMActionRunner3(AMDatabase *loggingDatabase, QObject *parent) :
	QObject(parent)
{
	loggingDatabase_ = loggingDatabase;
	cachedLogCount_ = 0;
	currentAction_ = 0;
	isPaused_ = true;
	queueModel_ = new AMActionRunnerQueueModel3(this, this);
}

AMActionRunner3::~AMActionRunner3() {
	while(queuedActionCount() != 0) {
		deleteActionInQueue(queuedActionCount()-1);
	}
}

AMActionRunner3 * AMActionRunner3::workflow()
{
	if(!workflowInstance_)
		workflowInstance_ = new AMActionRunner3(AMDatabase::database("actions"));
	return workflowInstance_;
}

void AMActionRunner3::releaseWorkflow()
{
	delete workflowInstance_;
	workflowInstance_ = 0;
}

AMActionRunner3* AMActionRunner3::scanActionRunner(){
	if(!scanActionRunnerInstance_)
		scanActionRunnerInstance_ = new AMActionRunner3(AMDatabase::database("scanActions"));
	return scanActionRunnerInstance_;
}

void AMActionRunner3::releaseScanActionRunner(){
	delete scanActionRunnerInstance_;
	scanActionRunnerInstance_ = 0;
}

bool AMActionRunner3::isScanAction() const
{
	return qobject_cast<const AMScanAction *>(currentAction_) ? true : false;
}

void AMActionRunner3::onCurrentActionStateChanged(int state, int previousState)
{
	emit currentActionStateChanged(state, previousState);

	if(state == AMAction3::Starting){

		AMListAction3* listAction = qobject_cast<AMListAction3*>(currentAction_);

		if(listAction){

			int parentLogId = -1;
			AMListAction3* parentAction = qobject_cast<AMListAction3*>(listAction->parentAction());

			if(parentAction)
				parentLogId = parentAction->logActionId();

			AMActionHistoryModel3 *historyModel = AMAppControllerSupport::actionHistoryModelFromDatabaseName(loggingDatabase()->connectionName());
			//if(!AMActionLog3::logUncompletedAction(currentAction_, loggingDatabase_, parentLogId)) {
			if(!historyModel || !historyModel->logUncompletedAction(currentAction_, loggingDatabase_, parentLogId)){
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -200, "There was a problem logging the uncompleted action to your database.  Please report this problem to the Acquaman developers."));
			}
		}

		if (isScanAction())
			emit scanActionCreated((AMScanAction *)currentAction());
	}

	if ( (state == AMAction3::Running) && (previousState != AMAction3::Resuming) ){

		if (isScanAction())
			emit scanActionStarted((AMScanAction *)currentAction());
	}

	if(state == AMAction3::Failed) {
		// What should we do?
		int failureResponse = currentAction_->failureResponseInActionRunner();
		if(failureResponse == AMAction3::PromptUserResponse)
			failureResponse = internalAskUserWhatToDoAboutFailedAction(currentAction_);

		if(failureResponse == AMAction3::AttemptAnotherCopyResponse) {
			// make a fresh copy of this action and put it at the front of the queue to run again.
			insertActionInQueue(currentAction_->createCopy(), 0);
		}
		// other failure response is to MoveOn, which we'll do anyway.
	}

	if(state == AMAction3::Cancelled) {
		// Usability guess for now: If the user intervened to cancel, they probably want to make some changes to something. Let's pause the workflow for now to let them do that, rather than automatically go on to the next.
		setQueuePaused(true);
	}

	// for all three final states, this is how we wrap things up for the current action:
	if(state == AMAction3::Failed ||
			state == AMAction3::Cancelled ||
			state == AMAction3::Succeeded) {

		// log it, unless it's a list action that wants to take care of logging its sub-actions itself.
		AMListAction3* listAction = qobject_cast<AMListAction3*>(currentAction_);
		int parentLogId = -1;
		AMListAction3* parentAction = qobject_cast<AMListAction3*>(currentAction_->parentAction());
		if(parentAction)
			parentLogId = parentAction->logActionId();
		if(!(listAction && listAction->shouldLogSubActionsSeparately())) {
			AMActionHistoryModel3 *historyModel = AMAppControllerSupport::actionHistoryModelFromDatabaseName(loggingDatabase()->connectionName());
			//if(!AMActionLog3::logCompletedAction(currentAction_, loggingDatabase_, parentLogId)) {
			if(!historyModel || !historyModel->logCompletedAction(currentAction_, loggingDatabase_, parentLogId)){
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -200, "There was a problem logging the completed action to your database.  Please report this problem to the Acquaman developers."));
			}
		}
		else if(listAction){
			AMActionHistoryModel3 *historyModel = AMAppControllerSupport::actionHistoryModelFromDatabaseName(loggingDatabase()->connectionName());
			//if(!AMActionLog3::updateCompletedAction(currentAction_, loggingDatabase_)) {
			if(!historyModel || !historyModel->updateCompletedAction(currentAction_, loggingDatabase_)){
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -200, "There was a problem updating the log of the completed action to your database.  Please report this problem to the Acquaman developers."));
			}
		}

		if (isScanAction())
			emit scanActionFinished((AMScanAction *)currentAction());

		// move onto the next, if there is one, and disconnect and delete the old one.
		internalDoNextAction();
	}
}

// This is a non-GUI class, and we're popping up a QMessageBox. Not ideal; hope you'll let us get away with that.

#include <QMessageBox>
#include <QPushButton>
void AMActionRunner3::insertActionInQueue(AMAction3 *action, int index)
{
	if(!action)
		return;

	if(index < 0 || index > queuedActions_.count())
		index = queuedActions_.count();

	AMAction3::ActionValidity actionValidity = action->isValid();
	if(actionValidity != AMAction3::ActionCurrentlyValid){
		QString windowTitle;
		QString informationText;
		if(actionValidity == AMAction3::ActionNotCurrentlyValid){
			windowTitle = "The Action You've Added May Not Be Valid";
			informationText = "\n\nYou can ignore this warning (maybe something will happen between now and the time the action is run to make it valid) or you can cancel adding the action.";
		}
		else if(actionValidity == AMAction3::ActionNeverValid){
			windowTitle = "The Action You've Added Is Not Valid";
			informationText = "\n\nYou cannot ignore this warning, this type of action is not supported.";
		}

		QMessageBox box;
		box.setWindowTitle(windowTitle);
		box.setText("The '" % action->info()->typeDescription() % "' action you've just added reports:\n" % action->notValidWarning());
		box.setInformativeText(informationText);

		QPushButton* ignoreAndAddButton = new QPushButton("Ignore this warning and add");
		QPushButton* cancelAddingButton = new QPushButton("Cancel adding this action");

		if(actionValidity == AMAction3::ActionNotCurrentlyValid)
			box.addButton(ignoreAndAddButton, QMessageBox::RejectRole);
		box.addButton(cancelAddingButton, QMessageBox::AcceptRole);
		box.setDefaultButton(cancelAddingButton);

		box.exec();
		if(box.clickedButton() == cancelAddingButton)
			return;
	}

	emit queuedActionAboutToBeAdded(index);
	queuedActions_.insert(index, action);
	emit queuedActionAdded(index);

	// was this the first action inserted into a running but empty queue? Start it up!
	if(!isPaused_ && !actionRunning())
		internalDoNextAction();
}

bool AMActionRunner3::deleteActionInQueue(int index)
{
	if(index<0 || index>=queuedActions_.count())
		return false;

	emit queuedActionAboutToBeRemoved(index);
	AMAction3* action= queuedActions_.takeAt(index);
	emit queuedActionRemoved(index);

	delete action;

	return true;
}

bool AMActionRunner3::duplicateActionInQueue(int index)
{
	if(index<0 || index>=queuedActions_.count())
		return false;

	insertActionInQueue(queuedActionAt(index)->createCopy(), index+1);
	return true;
}

bool AMActionRunner3::duplicateActionsInQueue(const QList<int> &indexesToCopy)
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

	// todozzz use sort method?

	// note: because we're inserting at higher indexes than any of indexesToCopy, we know that any that we're supposed to copy won't change positions as we start inserting.
	foreach(int index, indexesToCopy) {
		insertActionInQueue(queuedActionAt(index)->createCopy(),
					++maxIndex);
	}
	return true;
}

bool AMActionRunner3::moveActionInQueue(int currentIndex, int finalIndex)
{
	if(currentIndex == finalIndex)
		return false;

	if(currentIndex < 0 || currentIndex >= queuedActionCount())
		return false;

	if(finalIndex <0 || finalIndex >= queuedActionCount())
		finalIndex = queuedActionCount()-1;

	emit queuedActionAboutToBeRemoved(currentIndex);
	AMAction3* moveAction = queuedActions_.takeAt(currentIndex);
	emit queuedActionRemoved(currentIndex);

	emit queuedActionAboutToBeAdded(finalIndex);
	queuedActions_.insert(finalIndex, moveAction);
	emit queuedActionAdded(finalIndex);

	return true;
}

void AMActionRunner3::setQueuePaused(bool isPaused) {
	if(isPaused == isPaused_)
		return;

	emit queuePausedChanged(isPaused_ = isPaused);

	// if we've gone from paused to running, and there is no action running right now, and we have actions we can run... Start the next.
	if(!isPaused_ && queuedActionCount() != 0 && !actionRunning())
		internalDoNextAction();
}

void AMActionRunner3::internalDoNextAction()
{
	// signal that no action is running? (queue paused, or we're out of actions in the queue to run...)
	if(isPaused_ || queuedActionCount() == 0) {
		if(currentAction_) {
			AMAction3* oldAction = currentAction_;
			emit currentActionChanged(currentAction_ = 0);
			oldAction->scheduleForDeletion();
			//oldAction->deleteLater(); // the action might have sent notifyFailed() or notifySucceeded() in the middle a deep call stack... In that case, we might actually still be executing inside the action's code, so better not delete it until that all has a chance to finish. (Otherwise... Crash!)

			// If we are done with all the actions inside the queue then we should pause the queue so the next action doesn't start right away.
			if (queuedActionCount() == 0)
				setQueuePaused(true);
		}
	}

	// Otherwise, keep on keepin' on. Disconnect and delete the old current action (if there is one... If we're resuming from pause or starting for the first time, there won't be)., and connect and start the next one.
	else {
		AMAction3* oldAction = currentAction_;

		emit queuedActionAboutToBeRemoved(0);
		AMAction3* newAction = queuedActions_.takeAt(0);
		emit queuedActionRemoved(0);

		emit currentActionChanged(currentAction_ = newAction);

		if(oldAction) {
			disconnect(oldAction, 0, this, 0);
			oldAction->scheduleForDeletion();
			//oldAction->deleteLater();	// the action might have sent notifyFailed() or notifySucceeded() in the middle a deep call stack... In that case, we might actually still be executing inside the action's code, so better not delete it until that all has a chance to finish. (Otherwise... Crash!)
		}

		connect(currentAction_, SIGNAL(stateChanged(int,int)), this, SLOT(onCurrentActionStateChanged(int,int)));
		connect(currentAction_, SIGNAL(progressChanged(double,double)), this, SIGNAL(currentActionProgressChanged(double,double)));
		connect(currentAction_, SIGNAL(statusTextChanged(QString)), this, SIGNAL(currentActionStatusTextChanged(QString)));
		connect(currentAction_, SIGNAL(expectedDurationChanged(double)), this, SIGNAL(currentActionExpectedDurationChanged(double)));

		AMAction3::ActionValidity actionValidity = currentAction_->isValid();
		if(actionValidity != AMAction3::ActionCurrentlyValid){
			QString windowTitle;
			QString informationText;
			if(actionValidity == AMAction3::ActionNotCurrentlyValid){
				windowTitle = "The Action You've Added May Not Be Valid";
				informationText = "\n\nYou can ignore this warning (maybe something will happen between now and the time the action is run to make it valid) or you can cancel adding the action.\nConsidering the action is about to run right now, this seems very unlikely.";
			}
			else if(actionValidity == AMAction3::ActionNeverValid){
				windowTitle = "The Action You've Added Is Not Valid";
				informationText = "\n\nYou cannot ignore this warning, this type of action is not supported.";
			}

			QMessageBox box;
			box.setWindowTitle(windowTitle);
			box.setText("The '" % currentAction_->info()->typeDescription() % "' action you're about to run reports:\n" % currentAction_->notValidWarning());
			box.setInformativeText(informationText);

			QPushButton* ignoreAndRunButton = new QPushButton("Ignore this warning and run");
			QPushButton* cancelRunningButton = new QPushButton("Cancel running this action");

			if(actionValidity == AMAction3::ActionNotCurrentlyValid)
				box.addButton(ignoreAndRunButton, QMessageBox::RejectRole);
			box.addButton(cancelRunningButton, QMessageBox::AcceptRole);
			box.setDefaultButton(cancelRunningButton);

			box.exec();
			if(box.clickedButton() == cancelRunningButton){
				cancelCurrentAction();
				return;
			}
		}

		AMListAction3 *listAction = qobject_cast<AMListAction3 *>(currentAction_);
		if (listAction){
			listAction->setLoggingDatabase(loggingDatabase_);

			connect(listAction, SIGNAL(scanActionCreated(AMScanAction*)), this, SIGNAL(scanActionCreated(AMScanAction*)));
			connect(listAction, SIGNAL(scanActionStarted(AMScanAction*)), this, SIGNAL(scanActionStarted(AMScanAction*)));
			connect(listAction, SIGNAL(scanActionFinished(AMScanAction*)), this, SIGNAL(scanActionFinished(AMScanAction*)));
		}

		// to avoid a growing call stack if a long series of actions are all failing inside their start() method... We wait to run the next one until we get back to the even loop.
		QTimer::singleShot(0, currentAction_, SLOT(start()));
	}
}

// Again, this is a non-GUI class, and we're popping up a QMessageBox. Not ideal; hope you'll let us get away with that.
int AMActionRunner3::internalAskUserWhatToDoAboutFailedAction(AMAction3* action)
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
		return AMAction3::AttemptAnotherCopyResponse;
	else
		return AMAction3::MoveOnResponse;
}

void AMActionRunner3::runActionImmediately(AMAction3 *action)
{
	connect(action, SIGNAL(stateChanged(int,int)), this, SLOT(onImmediateActionStateChanged(int,int)));
	immediateActions_ << action;
	action->start();
}

void AMActionRunner3::onImmediateActionStateChanged(int state, int previousState)
{
	Q_UNUSED(previousState)

	AMAction3* action = qobject_cast<AMAction3*>(sender());
	if(!action)
		return;

	if(state == AMAction3::Failed) {
		// What should we do?
		int failureResponse = action->failureResponseInActionRunner();
		if(failureResponse == AMAction3::PromptUserResponse)
			failureResponse = internalAskUserWhatToDoAboutFailedAction(action);

		if(failureResponse == AMAction3::AttemptAnotherCopyResponse) {
			// make a fresh copy of this action and run it now
			AMAction3* retryAction = action->createCopy();
			connect(retryAction, SIGNAL(stateChanged(int,int)), this, SLOT(onImmediateActionStateChanged(int,int)));
			immediateActions_ << retryAction;
			QTimer::singleShot(0, retryAction, SLOT(start()));
		}
	}

	// for all three final states, this is how we wrap things up for the current action:
	if(state == AMAction3::Failed ||
			state == AMAction3::Cancelled ||
			state == AMAction3::Succeeded) {

		// remove from the list of current immediate actions
		immediateActions_.removeAll(action);

		int parentLogId = -1;
		AMListAction3* parentAction = qobject_cast<AMListAction3*>(action->parentAction());
		if(parentAction)
			parentLogId = parentAction->logActionId();
		// log it:
		AMActionHistoryModel3 *historyModel = AMAppControllerSupport::actionHistoryModelFromDatabaseName(loggingDatabase()->connectionName());
		//if(!AMActionLog3::logCompletedAction(action, loggingDatabase_, parentLogId)) {
		if(!historyModel || !historyModel->logCompletedAction(action, loggingDatabase_, parentLogId)){
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -201, "There was a problem logging the completed action to your database.  Please report this problem to the Acquaman developers."));
		}

		// disconnect and delete it
		disconnect(action, 0, this, 0);
		action->scheduleForDeletion();
		//action->deleteLater();
	}
}

AMAction3 * AMActionRunner3::immediateActionAt(int index)
{
	if(index < 0 || index >= immediateActions_.count()) {
		return 0;
	}
	return immediateActions_.at(index);
}

bool AMActionRunner3::cancelCurrentAction()
{
	if(currentAction_) {
		currentAction_->cancel();
		return true;
	}
	return false;
}

bool AMActionRunner3::pauseCurrentAction()
{
	if(currentAction_ && currentAction_->canPause()) {
		return currentAction_->pause();
	}
	return false;
}

bool AMActionRunner3::resumeCurrentAction()
{
	if(currentAction_ && currentAction_->state() == AMAction3::Paused) {
		return currentAction_->resume();
	}
	return false;
}

bool AMActionRunner3::cancelImmediateActions()
{
	if(immediateActions_.isEmpty())
		return false;

	foreach(AMAction3* action, immediateActions_)
		action->cancel();
	return true;
}

void AMActionRunner3::incrementCachedLogCount(){
	cachedLogCount_++;
}

void AMActionRunner3::resetCachedLogCount(){
	cachedLogCount_ = 0;
}

int AMActionRunner3::indexOfQueuedAction(const AMAction3 *action) {
	for(int i=0,cc=queuedActions_.count(); i<cc; i++) {
		if(action == queuedActions_.at(i))
			return i;
	}
	return -1;
}

AMActionRunnerQueueModel3::AMActionRunnerQueueModel3(AMActionRunner3 *actionRunner, QObject *parent) : QAbstractItemModel(parent)
{
	actionRunner_ = actionRunner;
	connect(actionRunner_, SIGNAL(queuedActionAboutToBeAdded(int)), this, SLOT(onActionAboutToBeAdded(int)));
	connect(actionRunner_, SIGNAL(queuedActionAdded(int)), this, SLOT(onActionAdded(int)));
	connect(actionRunner_, SIGNAL(queuedActionAboutToBeRemoved(int)), this, SLOT(onActionAboutToBeRemoved(int)));
	connect(actionRunner_, SIGNAL(queuedActionRemoved(int)), this, SLOT(onActionRemoved(int)));
}

QModelIndex AMActionRunnerQueueModel3::index(int row, int column, const QModelIndex &parent) const
{
	if(column != 0)
		return QModelIndex();

	// if no parent: this is top-level. use AMActionRunner API.
	if(!parent.isValid()) {
		// check for range
		if(row < 0 || row >= actionRunner_->queuedActionCount())
			return QModelIndex();
		return createIndex(row, 0, actionRunner_->queuedActionAt(row));
	}
	// otherwise this is an index for a sub-action of an AMNestedAction.
	else {
		AMListAction3* parentAction = qobject_cast<AMListAction3*>(actionAtIndex(parent));
		if(!parentAction) {
			AMErrorMon::debug(this, AMACTIONRUNNER_MODELINDEX_REQUESTED_CHILD_OF_NON_LIST_TYPE, "Requested a child index when the parent was not an AMListAction or subclass.");
			return QModelIndex();
		}
		if(row < 0 || row >= parentAction->subActionCount())
			return QModelIndex();
		return createIndex(row, 0, parentAction->subActionAt(row));
	}
}

QModelIndex AMActionRunnerQueueModel3::parent(const QModelIndex &child) const
{
	if(!child.isValid())
		return QModelIndex();

	AMAction3* childAction = actionAtIndex(child);
	if(!childAction)
		return QModelIndex();

	// if childAction->parentAction() returns 0 (ie: no parent -- its at the top level) then this will return an invalid index, meaning it has no parent.
	return indexForAction(childAction->parentAction());
}

int AMActionRunnerQueueModel3::rowCount(const QModelIndex &parent) const
{
	if(!parent.isValid()) {
		return actionRunner_->queuedActionCount();
	}

	AMListAction3* listAction = qobject_cast<AMListAction3*>(actionAtIndex(parent));
	if(listAction)
		return listAction->subActionCount();
	else
		return 0;

}

int AMActionRunnerQueueModel3::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return 1;
}

QVariant AMActionRunnerQueueModel3::data(const QModelIndex &index, int role) const
{
	AMAction3* action = actionAtIndex(index);
	if(!action) {
		AMErrorMon::debug(this, AMACTIONRUNNER_MODELDATA_NO_ACTION_AT_INDEX, QString("No action at index (row: %1 column: %2) in AMActionRunnerQueueModel.").arg(index.row()).arg(index.column()));
		return QVariant();
	}

	if(role == Qt::DisplayRole) {
		return action->info()->shortDescription();
	}
	else if(role == Qt::DecorationRole) {
		QPixmap p;
		QString iconFileName = action->info()->iconFileName();
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

	return QVariant();
}

Qt::ItemFlags AMActionRunnerQueueModel3::flags(const QModelIndex &index) const
{
	// need to be able to drop onto the top-level index.
	if(!index.isValid())
		return Qt::ItemIsDropEnabled;

	Qt::ItemFlags rv = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;

	// List actions can accept drops onto them (to insert actions)
	AMListAction3* listAction = qobject_cast<AMListAction3*>(actionAtIndex(index));
	if(listAction)
		rv |= Qt::ItemIsDropEnabled;

	// Actions are editable if they have an editor registered for their ActionInfo.
	if(AMActionRegistry3::s()->editorRegisteredForInfo(actionAtIndex(index)->info()))
		rv |= Qt::ItemIsEditable;

	return rv;
}

bool AMActionRunnerQueueModel3::hasChildren(const QModelIndex &parent) const
{
	if(!parent.isValid())
		return true;	// top level: must have children.
	else {
		// other levels: have children if its a list action.
		AMListAction3* listAction = qobject_cast<AMListAction3*>(actionAtIndex(parent));
		return (listAction != 0);
	}
}

AMAction3 * AMActionRunnerQueueModel3::actionAtIndex(const QModelIndex &index) const
{
	if(!index.isValid())
		return 0;
	return static_cast<AMAction3*>(index.internalPointer());
}

void AMActionRunnerQueueModel3::onActionAboutToBeAdded(int i)
{
	beginInsertRows(QModelIndex(), i, i);
}

void AMActionRunnerQueueModel3::onActionAdded(int i)
{
	internalConnectListActions(actionRunner_->queuedActionAt(i));
	endInsertRows();
}

void AMActionRunnerQueueModel3::onActionAboutToBeRemoved(int i)
{
	internalDisconnectListActions(actionRunner_->queuedActionAt(i));
	beginRemoveRows(QModelIndex(), i, i);
}

void AMActionRunnerQueueModel3::onActionRemoved(int index)
{
	Q_UNUSED(index)
	endRemoveRows();
}

QModelIndex AMActionRunnerQueueModel3::indexForAction(AMAction3 *action) const
{
	if(!action)
		return QModelIndex();

	AMAction3* parentAction = action->parentAction();
	if(!parentAction) {
		// action is in the top-level. Do a linear search for it in the actionRunner_ API.
		int row = actionRunner_->indexOfQueuedAction(action);
		if(row == -1) {
			AMErrorMon::debug(this, AMACTIONRUNNER_RETRIEVE_INDEX_FAILED_NO_MATCHING_ACTION_IN_QUEUE, "Action not found in AMActionRunner.");
			return QModelIndex();
		}
		return createIndex(row, 0, action);
	}
	else {
		// we do a have parent action. Do a linear search for ourself in the parent AMNestedAction.
		int row = ((AMListAction3 *)parentAction)->indexOfSubAction(action);
		if(row == -1) {
			AMErrorMon::debug(this, AMACTIONRUNNER_RETRIEVE_INDEX_FAILED_NO_MATCHING_ACTION_IN_LIST_TYPE, "Action not found in list action.");
			return QModelIndex();
		}
		return createIndex(row, 0, action);
	}
}

void AMActionRunnerQueueModel3::internalConnectListActions(AMAction3 *action)
{
	AMListAction3* listAction = qobject_cast<AMListAction3*>(action);
	if(listAction) {
		disconnect(listAction, 0, this, 0);
		connect(listAction, SIGNAL(subActionAboutToBeAdded(int)), this, SLOT(onSubActionAboutToBeAdded(int)));
		connect(listAction, SIGNAL(subActionAdded(int)), this, SLOT(onSubActionAdded(int)));
		connect(listAction, SIGNAL(subActionAboutToBeRemoved(int)), this, SLOT(onSubActionAboutToBeRemoved(int)));
		connect(listAction, SIGNAL(subActionRemoved(int)), this, SLOT(onSubActionRemoved(int)));

		for(int i=0,cc=listAction->subActionCount(); i<cc; i++)
			internalConnectListActions(listAction->subActionAt(i));
	}
}

void AMActionRunnerQueueModel3::internalDisconnectListActions(AMAction3 *action)
{
	AMListAction3* listAction = qobject_cast<AMListAction3*>(action);
	if(listAction) {
		disconnect(listAction, 0, this, 0);

		for(int i=0,cc=listAction->subActionCount(); i<cc; i++)
			internalDisconnectListActions(listAction->subActionAt(i));
	}
}

void AMActionRunnerQueueModel3::onSubActionAboutToBeAdded(int index)
{
	AMListAction3* parentAction = qobject_cast<AMListAction3*>(sender());
	lastSender_ = parentAction;
	if(!parentAction) {
		AMErrorMon::debug(this, AMACTIONRUNNER_CANNOT_ADD_SUBBACTION_WITH_INVALID_PARENT, "Invalid parent sent subActionAboutToBeAdded().");
		return;
	}

	QModelIndex parentIndex = indexForAction(parentAction);
	if(!parentIndex.isValid()) {
		AMErrorMon::debug(this, AMACTIONRUNNER_CANNOT_ADD_SUBACTION_WITH_PARENT_NOT_IN_QUEUE, "List Action (or subclass) parent not found for the added sub-action.");
		return;
	}

	beginInsertRows(parentIndex, index, index);
}

void AMActionRunnerQueueModel3::onSubActionAdded(int index)
{
	AMListAction3* parentAction = qobject_cast<AMListAction3*>(sender());
	if(parentAction != lastSender_) {
		AMErrorMon::debug(this, AMACTIONRUNNER_CANNOT_ADD_SUBACTION_CALL_MISMATCH, "Unmatched calls to onSubActionAboutToBeAdded/onSubActionAdded.");
	}
	else {
		internalConnectListActions(parentAction->subActionAt(index));
	}
	endInsertRows();
}

void AMActionRunnerQueueModel3::onSubActionAboutToBeRemoved(int index)
{
	AMListAction3* parentAction = qobject_cast<AMListAction3*>(sender());
	lastSender_ = parentAction;
	if(!parentAction) {
		AMErrorMon::debug(this, AMACTIONRUNNER_CANNOT_REMOVE_SUBBACTION_WITH_INVALID_PARENT, "Invalid parent sent subActionAboutToBeRemoved().");
		return;
	}

	internalDisconnectListActions(parentAction->subActionAt(index));

	QModelIndex parentIndex = indexForAction(parentAction);
	if(!parentIndex.isValid()) {
		AMErrorMon::debug(this, AMACTIONRUNNER_CANNOT_REMOVE_SUBACTION_WITH_PARENT_NOT_IN_QUEUE, "List Action (or subclass) parent not found for the removed sub-action.");
		return;
	}

	beginRemoveRows(parentIndex, index, index);
}

void AMActionRunnerQueueModel3::onSubActionRemoved(int index)
{
	Q_UNUSED(index)
	if(qobject_cast<AMListAction3*>(sender()) != lastSender_) {
		AMErrorMon::debug(this, AMACTIONRUNNER_CANNOT_REMOVE_SUBACTION_CALL_MISMATCH, "Unmatched calls to onSubActionAboutToBeRemoved/onSubActionRemoved.");
	}
	endRemoveRows();
}

void AMActionRunnerQueueModel3::deleteActionsInQueue(QModelIndexList indexesToDelete)
{
	QList<int> topLevelIndexesToDelete;
	QList<QPersistentModelIndex> listIndexesToDelete;

	foreach(QModelIndex i, indexesToDelete) {
		// Sort out the top-level actions...
		if(!i.parent().isValid()) {
			topLevelIndexesToDelete << i.row();
		}
		// ... from those that are inside an AMNestedAction
		else {
			listIndexesToDelete << QPersistentModelIndex(i);
		}
	}
	// Delete the top-level actions. Need to delete from largest index to smallest index, otherwise the indexes will change as we go. [We could use persistent indexes to avoid this problem, but the performance is better if we don't.]
	qSort(topLevelIndexesToDelete);
	for(int i=topLevelIndexesToDelete.count()-1; i>=0; i--)
		actionRunner_->deleteActionInQueue(topLevelIndexesToDelete.at(i));

	// Now, do we have any list actions left to delete? They might have been deleted if they were inside parents that were deleted, so make sure to check for valid persistent indexes.
	foreach(QPersistentModelIndex i, listIndexesToDelete) {
		if(i.isValid()) {
			// get their parent model item... which should represent a list action
			AMListAction3* listAction = qobject_cast<AMListAction3*>(actionAtIndex(i.parent()));
			if(listAction) {
				listAction->deleteSubAction(i.row());
			}
		}
	}
}


void AMActionRunnerQueueModel3::duplicateActionsInQueue(const QModelIndexList &indexesToCopy) {
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
		actionRunner_->duplicateActionsInQueue(topLevelIndexes);
	}
	// Otherwise, these indexes are sub-actions of some AMNestedAction
	else {
		// find the loop action:
		AMLoopAction3* loopAction = qobject_cast<AMLoopAction3*>(actionAtIndex(firstParent));
		if(loopAction) {
			QList<int> subActionIndexes;
			foreach(QModelIndex i, indexesToCopy)
				subActionIndexes << i.row();
			// use the AMNestedAction API to duplicate these subactions.
			loopAction->duplicateSubActions(subActionIndexes);
		}
	}
}


bool AMActionRunnerQueueModel3::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	Q_UNUSED(column)

	// Option 1: Handle internal move actions, where the \c data is a list of the qmodelindexes to move
	if(action == Qt::MoveAction) {
		const AMModelIndexListMimeData3* milData = qobject_cast<const AMModelIndexListMimeData3*>(data);
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
			// OK, these indexes are OK to use.  Sort them:
			qSort(mil);

			// Case A: Source: These are all top-level actions in the queue.
			if(!first.parent().isValid()) {

				// Subcase A1: destination: They are being moved to another location at the top level. We can move them using the AMActionRunner API.
				if(!parent.isValid()) {

					// Get a persistent model index corresponding to the destination. (It might move as we move things around.)
					QPersistentModelIndex destinationIndex(index(row, 0, parent));
					// go backward through the list of indexes to move, inserting at destination. This will ensure that we maintain ordering. The persistent indexes adjust themselves to keep pointing to the same objects as we insert/remove.
					for(int i=0,cc=mil.count(); i<cc; i++) {
						int destinationRow = destinationIndex.row();
						if(destinationRow > mil.at(i).row())
							destinationRow--;
						actionRunner_->moveActionInQueue(mil.at(i).row(), destinationRow);
					}
					return true;
				}
				// Subcase A2: destination: They are being moved to inside a list action.
				else {
					// parent is valid... It represents the list action we're supposed to drop these actions inside of.
					AMListAction3* listAction = qobject_cast<AMListAction3*>(actionAtIndex(parent));
					if(!listAction) {
						AMErrorMon::debug(this, AMACTIONRUNNER_FAILED_TO_DROP_INSIDE_INVALID_LIST_TYPE, "Asked to drop actions inside a list action that wasn't valid.");
						return false;
					}
					int targetRow = row;
					if(targetRow < 0 || targetRow > listAction->subActionCount())
						targetRow = listAction->subActionCount();
					for(int i=0,cc=mil.count(); i<cc; i++) {

						if (listAction->subActionMode() == AMListAction3::Sequential || (listAction->subActionMode() == AMListAction3::Parallel && actionRunner_->queuedActionAt(mil.at(i).row())->canParallelize())){

							listAction->insertSubAction(actionRunner_->queuedActionAt(mil.at(i).row())->createCopy(), targetRow++);
							actionRunner_->deleteActionInQueue(mil.at(i).row());
						}
					}
					return true;
				}
			}
			// Case B: Source: These are sub-actions inside a list action...
			else {
				AMListAction3* sourceParentAction = qobject_cast<AMListAction3*>(actionAtIndex(first.parent()));
				if(!sourceParentAction) {
					AMErrorMon::debug(this, AMACTIONRUNNER_FAILED_TO_MOVE_INSIDE_INVALID_LIST_TYPE, "Asked to move actions from inside a list action that wasn't valid.");
					return false;
				}

				// Subcase B0: The destination is the same as the source. Just rearranging within one AMListAction.
				if(first.parent() == parent) {
					QPersistentModelIndex destinationIndex(index(row, 0, parent));
					for(int i=0,cc=mil.count(); i<cc; i++) {
						AMAction3* moveAction = sourceParentAction->takeSubActionAt(mil.at(i).row());
						if(moveAction)
							sourceParentAction->insertSubAction(moveAction, destinationIndex.row());
						else
							AMErrorMon::debug(this, AMACTIONRUNNER_FAILED_TO_REARRANGE_INVALID_ITEM_FOUND, "Received an invalid item when rearranging actions inside a list action.");
					}
					return true;
				}
				// otherwise, moving to a different parent
				else {
					// Subcase B1: The destination is the top level. Need to add the copied actions directly to AMActionRunner.
					if(!parent.isValid()) {
						int targetRow = row;
						if(targetRow < 0 || targetRow > actionRunner_->queuedActionCount())
							targetRow = actionRunner_->queuedActionCount();
						for(int i=0,cc=mil.count(); i<cc; i++) {
							actionRunner_->insertActionInQueue(sourceParentAction->takeSubActionAt(mil.at(i).row()), targetRow++);
						}
						return true;
					}
					// Subcase B2: The destination is a different sub-action.
					else {
						AMListAction3* destParentAction = qobject_cast<AMListAction3*>(actionAtIndex(parent));
						if(!destParentAction) {
							AMErrorMon::debug(this, AMACTIONRUNNER_FAILED_TO_MOVE_INTO_INVALID_LIST_TYPE, "Asked to move actions into a list action that wasn't valid.");
							return false;
						}
						int targetRow = row;
						if(targetRow < 0 || targetRow > destParentAction->subActionCount())
							targetRow = destParentAction->subActionCount();
						for(int i=0,cc=mil.count(); i<cc; i++) {
							destParentAction->insertSubAction(sourceParentAction->takeSubActionAt(mil.at(i).row()), targetRow++);
						}
						return true;
					}
				}
			}
		}
		return false;
	}

	// if we return false to the DropAction, it might retry with IgnoreAction. We need to accept that one.
	else if(action == Qt::IgnoreAction) {
		//qdebug() << "AMActionRunnerQueueModel: Wow: Qt actually behaved according to spec and offered the IgnoreAction. Too bad this never happens...";
		return true;
	}

	return false;
}

QMimeData * AMActionRunnerQueueModel3::mimeData(const QModelIndexList &indexes) const
{
	if(indexes.isEmpty())
		return 0;
	// We only support dragging if all the indexes are at the same level of the hierarchy. Let's check that here, and return 0 if not.
	QModelIndex firstParent = indexes.at(0).parent();
	foreach(const QModelIndex& index, indexes)
		if(index.parent() != firstParent)
			return 0;

	// alright, looks good. Turn them into persistent indexes and pass them off inside our custom (not externally-sharable) mime type container.
	return new AMModelIndexListMimeData3(indexes);
}

QStringList AMActionRunnerQueueModel3::mimeTypes() const
{
	return QStringList() << "application/octet-stream";
}

bool AMActionRunnerQueueModel3::removeRows(int row, int count, const QModelIndex &parent)
{
	Q_UNUSED(row)
	Q_UNUSED(count)
	Q_UNUSED(parent)

	AMErrorMon::debug(this, AMACTIONRUNNER_IGNORING_REQUEST_FROM_VIEW_TO_REMOVE_ROWS, "Ignoring request from view to remove rows.");

	return false;
}

bool AMActionRunner3::runActionImmediatelyInQueue(AMAction3 *action)
{
	if(actionRunning())
		return false;
	if(!action)
		return false;

	bool queueWasPaused = queuePaused();
	insertActionInQueue(action, 0);
	setQueuePaused(false);	// this will start up the first action, but only that one.
	setQueuePaused(queueWasPaused);
	return true;
}

