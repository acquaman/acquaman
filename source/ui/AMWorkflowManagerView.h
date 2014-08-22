/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AMWORKFLOWMANAGERVIEW_H
#define AMWORKFLOWMANAGERVIEW_H

#include <QWidget>

class AMSamplePlatePre2013;
class AMSamplePlatePre2013ItemModel;

class QPushButton;
class QVBoxLayout;
class AMVerticalStackWidget;
class AMTopFrame;

#include "actions/AMBeamlineActionsList.h"

class AMBeamlineActionsListView;

class AMWorkflowManagerView : public QWidget
{
Q_OBJECT
public:
	explicit AMWorkflowManagerView(QWidget *parent = 0);

	/// Attempt to queue up an \c action in the workflow.  The action is added at position \c index (where 0 is immediately following the currently executing action.)  The workflow takes ownership of the action.  If \c startNow is true and the workflow isn't already running, the workflow will start executing immediately (from the first item).
	/*! If \c index is -1 or >= count(), the action is appended to the end of the queue. */
	void insertBeamlineAction(int index, AMBeamlineActionItem* action, bool startNow = false);

	/// Number of actions items left to run (not including the currently running action)
	int actionItemCount() const { return workflowQueue_->count(); }
	/// Returns true if the workflow is running (executing action items) right now
	bool isRunning() const { return workflowQueue_->isRunning(); }
	/// Returns true if it's currently impossible to start the workflow because another program has exclusive access to the beamline
	bool beamlineBusy() const;

	// These are functions required for Darren's addition to the workflow.  They are not used in the general workflow... workflow.

	/// Returns the current head of the workflow queue.
	AMBeamlineActionItem *headOfQueue() { return workflowQueue_->head(); }
	/// Removes a beamline action item from a given index.
	void removeBeamlineAction(int index);

	// End of functions required by Darren.

signals:
	/// Signals changes in workflowItemCount()
	void actionItemCountChanged(int actionItemsLeftToRun);
	/// Signals changes in isRunning()
	void runningChanged(bool isRunning);
	/// Signals changes in the status of the workflow: \c beamlineBusy means that it's impossible to start running the workflow because another program, etc. has exclusive access to it. \c queueEmpty means that there are no items (except for possibly the currently-running action) in the workflow. \c workflowRunning means that we are executing actions right now.
	void workflowStatusChanged(bool beamlineBusy, bool queueEmpty, bool workflowRunning);

	// Darren's addition.

	/// Notifier that the head of the queue has changed.
	void headChanged();

	// End of Darren's addition.

public slots:

	/// Start running the actions in the queue (if not already running)
	void startQueue();

	/// Responds to clicks of the addActionButton_.  Add an action to the workflow queue (\todo This does nothing for now... Make a flexible system for choosing from all available actions)
	void onAddActionRequested();

	/// Responds to requests from existing actions to place a new action (often a copy of themselves) at the end of the workflow
	void onCopyActionRequested(AMBeamlineActionItem *action);

	void onMoveUpActionRequested(AMBeamlineActionItem *aciton);
	void onMoveDownActionRequested(AMBeamlineActionItem *action);

	/// Sets the current sample plate for populating the actions list
	void setCurrentSamplePlate(AMSamplePlatePre2013 *newSamplePlate);

protected slots:
	/// Triggered by changes in the beamline scanning status, queue size, and queue running state. Emits actionItemCountChanged(), runningChanged(), and workflowStatusChanged().
	void reviewWorkflowStatus();

	/// Populate and popup menu of addable actions
	void onAddActionButtonClicked();
	/// Internally sets which sample on the plate is being referred to right now
	void setSampleHoverIndex();
	/// Internally sets which fiducialization mark is being referred to right now
	void setFiducializationHoverIndex();
	/// Handles when a sample from the sample plate is selected for an add action
	void onSamplePlateAddActionClicked();
	/// Handles when a fiducialization makr is selected for an add action
	void onFiducializationMarkAddActionClicked();

protected:
	QPushButton *startWorkflowButton_;
	QPushButton *addActionButton_;
	QMenu *addActionMenu_;
	QMenu *samplePlateAddActionMenu_, *fiducializationMarkAddActionMenu_;
	int samplePlateHoverIndex_, fiducializationMarkHoverIndex_;
	AMTopFrame *topFrame_;
	QVBoxLayout *vl_;

	AMBeamlineActionsList *workflowActions_;
	AMBeamlineActionsQueue *workflowQueue_;
	AMBeamlineActionsListView *workflowView_;

	AMSamplePlatePre2013 *currentSamplePlate_;
	AMSamplePlatePre2013ItemModel *samplePlateModel_;

	// disabled for now: AMBeamlineActionAdder *adder_;
};

#include "AMVerticalStackWidget.h"
class AMBeamlineActionsListView : public QWidget
{
	Q_OBJECT
public:
	AMBeamlineActionsListView(AMBeamlineActionsList *actionsList, AMBeamlineActionsQueue *actionsQueue, QWidget *parent = 0);

	bool swap(int indexOfFirst);

	void forceUpdate();

signals:
	void queueUpdated(int count);
	void copyRequested(AMBeamlineActionItem *itemCopy);
	void moveUpRequested(AMBeamlineActionItem *itemMoveUp);
	void moveDownRequested(AMBeamlineActionItem *itemMoveDown);

protected slots:
	void onActionChanged(int index);
	void onActionAdded(int index);
	void onActionRemoved(int index);

	void onActionRemoveRequested(AMBeamlineActionItem *item);

	void onRunningChanged();
	void onActionStarted();
	void onActionSucceeded();
	void onActionFailed();
	void reindexViews();

protected:
	AMBeamlineActionsList *actionsList_;
	AMBeamlineActionsQueue *actionsQueue_;

	AMVerticalStackWidget *actionsViewList_;
	int focusAction_;
};


#endif // AMWORKFLOWMANAGERVIEW_H
