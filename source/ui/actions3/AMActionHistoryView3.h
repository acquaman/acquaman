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


#ifndef AMACTIONHISTORYVIEW3_H
#define AMACTIONHISTORYVIEW3_H

#include <QWidget>
#include <QItemSelection>

#include "dataman/database/AMDatabase.h"
#include "util/AMDeferredFunctionCall.h"
#include "actions3/AMActionInfo3.h"

#define AMACTIONHISTORYVIEW_ACTIONLOG_DB_LOAD_FAILED 212001
#define AMACTIONHISTORYVIEW_ACTIONINFO_DB_LOAD_FAILED 212002
#define AMACTIONHISTORYVIEW_COULD_NOT_CREATE_ACTION 212003
#define AMACTIONHISTORYVIEW_COULD_NOT_LOAD_CHILD 212004
#define AMACTIONHISTORYVIEW_COULD_RERUN_ACTIONS 212005
#define AMACTIONHISTORYVIEW_DBLOADWARNING_IN_INFO 212006

class AMActionHistoryModel3;
class AMActionHistoryTreeView3;

class AMActionRunner3;
class AMListAction3;

class QFrame;
class QComboBox;
class QLabel;
class QPushButton;

/// This UI class provides a view of a user's completed actions that were run with AMActionRunner. It is part of the overall AMWorkflowView.
/*! The view can be used to browse completed actions, as well as re-instantiate copies of them in the AMActionRunner queue (as long as the specific AMAction/AMActionInfo pair has been registered with AMActionRegistry).*/
class AMActionHistoryView3 : public QWidget
{
	Q_OBJECT
public:
	/// Constructor. If you want this history view to be able to re-instantiate and re-queue actions, pass in valid AMActionRunner, ie: AMActionRunner::s().  The default (\c actionRunner = 0) is suitable for a Dataman-only program, and can only browse the user's action history.
 	virtual ~AMActionHistoryView3();
	AMActionHistoryView3(AMActionRunner3* actionRunner = 0, AMDatabase* db = 0, QWidget *parent = 0);

	/// Returns true if the view is currently collapsed to show only the header bar, and false if it is fully shown.
	bool isCollapsed() const { return isCollapsed_; }

	AMActionHistoryModel3* model() const { return model_; }

signals:
	/// This signal is emitted with \c true when the view widget is collapsed to show only the header bar, and \c false when it is restored.
	void collapsed(bool isCollapsed);

public slots:
	/// Collapses the view to show only the header bar.
	void collapse(bool doCollapse);

protected slots:
	/// Called when the "show more actions" button is clicked. Doubles the model's maximumActionsToDisplay().
	void onShowMoreActionsButtonClicked();
	// /// Called when one of the preset time-range filters is clicked. Calls the model's setVisibleTimeRange().
	// void onRangeComboBoxActivated(int rangeIndex);
	/// Called when the "Re-run this action" button is clicked. If we were constructed with access to an AMActionRunner, and the AMActionInfo associated with the selected items have been registered in AMActionRegistry, then we will re-queue copies of these actions in the AMActionRunner.
	void onReRunActionButtonClicked();
	/// Called when the selection changes... We use this to determine whether to enable the reRunActionButton_
	void onSelectionChanged();
	/// Called when the tree view tells us that an index was actually clicked. We use this to keep track of the index list and mark children with the parentSelected mapping flags.
	void onSelectedByClicked(const QModelIndex &index, bool othersCleared);
	/// Called when the tree view tells us that an index was actually un-clicked. We use this to keep track of the index list and mark children with the parentSelected mapping flags.
	void onDeselectedByClicked(const QModelIndex &index, bool othersCleared);
	/// Called when the tree view tells us that no index was clicked but a clear took place. We use this to keep track of the index list.
	void onClearedByClicked();

	/// Called right before the model is refreshed. Remember if we're scrolled right to the bottom, so we can go back there.
	void onModelAboutToBeRefreshed();
	/// Called right after the model is refreshed.  If we were scrolled to the bottom, let's go back to the new bottom. This lets users always see the most recently-completed actions.
	void onModelRefreshed();

	/// A custom context menu that offers the same functionality as "Re-run this/these action(s)".
	void onCustomContextMenuRequested(QPoint point);

protected:
	/// If we should be scrolled to the bottom of the treeView_, sometimes the scrolling doesn't happen if we're not visible. Here we make sure it does.
	void showEvent(QShowEvent *);

	/// Helper function for reloading actions (recursive for lists and loops)
	bool recurseDbLoadIndex(const QModelIndex &index, AMListAction3 *parentAction);

protected:
	/// The action runner we get logged actions from
	AMActionRunner3* actionRunner_;
	/// The database we're logging to
	AMDatabase* db_;

	/// The model we're using for the log items (which will be visualized by the AMActionHistoryTreeView)
	AMActionHistoryModel3* model_;

	/// The custom tree view for viewing the action log items
	AMActionHistoryTreeView3* treeView_;
	QComboBox* rangeComboBox_;
	QLabel* headerTitle_, *headerSubTitle_;
	QPushButton* showMoreActionsButton_;
	QPushButton* reRunActionButton_;

	bool scrolledToBottom_;
	bool isCollapsed_;

	/// The list of indexes that have actually been clicked
	QModelIndexList actuallyBeenClicked_;

	/// Used to latch when the showing more actions button has been clicked (need to hold onto this because the refresh is a deferred call)
	bool showingMoreActions_;
	/// Used to monitor if more actions were actually able to load (sometimes you could increase the number of actions, but not get anymore because of a big list or loop)
	int countBeforeShowMoreActions_;
};

#endif // AMACTIONHISTORYVIEW3_H
