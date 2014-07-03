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


#ifndef AMACTIONHISTORYMODEL_H
#define AMACTIONHISTORYMODEL_H

#include <QDateTime>
#include <QAbstractItemModel>
#include <QAbstractItemView>

#include "acquaman.h"
#include "util/AMDeferredFunctionCall.h"

#define AMACTIONHISTORYMODEL_MODELINDEX_OUT_OF_BOUNDS 213001

#define AMACTIONHISTORYMODEL_ROWCOUNT_BAD_PARENT_ITEM 213006
#define AMACTIONHISTORYMODEL_MODELDATA_BAD_ITEM 213007
#define AMACTIONHISTORYMODEL_HASCHILDREN_BAD_PARENT_ITEM 213008
#define AMACTIONHISTORYMODEL_INDEXFORLOGITEM_ITEM_NOT_IN_TREE 213009
#define AMACTIONHISTORYMODEL_INDEXFORNODE_NODE_NOT_IN_TREE 213010
#define AMACTIONHISTORYMODEL_REFRESHFROMDB_FAILED_TO_CREATE_TREE 213011
#define AMACTIONHISTORYMODEL_FAILED_TO_EXECUTE_DB_QUERY 213013
#define AMACTIONHISTORYMODEL_FAILED_TO_EXECUTE_DB_QUERY_NUMBER_OF_ITEMS 213014
#define AMACTIONHISTORYMODEL_FAILED_TO_EXECUTE_DB_QUERY_ALL_TOP_LEVELS 213015

class AMDatabase;
class AMAction3;
class AMActionInfo3;
class AMActionLog3;

class AMPointerTreeNode;
class AMPointerTree;

/// typedef for using a QMap of QAbstractItemView pointers and bools. Used to map out which items have selected parents and should show a different color even though they're not selected. Mapping is to ensure that multiple views can look at the model. Introduces some coupling.
typedef QMap<QAbstractItemView*, bool> ParentSelectMap;
Q_DECLARE_METATYPE(ParentSelectMap)

/// This item class is used to cache the details of a completed workflow action inside AMActionHistoryModel. You should never need to use this class directly.
/*! We use it instead of full AMActionLog instances, so that we don't have to load the complete AMActionInfo (which, depending on the subclass, could be a lot to load.)*/
class AMActionLogItem3 {
public:
	/// Constructor requires the \c id of the AMActionLog and the database \c db where it is stored.
 	virtual ~AMActionLogItem3();
	AMActionLogItem3(AMDatabase* db, int id);
	AMActionLogItem3(const AMActionLog3 &actionLog);

	/// Call this to refresh the item's content from the database
	void refresh() { loadedFromDb_ = false; }

	/// Returns the id of the corresponding AMActionLog in the database
	int id() const { return id_; }
	/// Returns the database that the corresponding AMActionLog is in
	AMDatabase* database() const { return db_; }
	/// Returns the endDateTime of the corresponding AMActionLog, loading it from the database if required.
	QDateTime endDateTime() const;
	/// Returns the startDateTime of the corresponding AMActionLog, loading it from the database if required.
	QDateTime startDateTime() const;
	/// Returns the shortDescription, loading if from the database if required.
	QString shortDescription() const;
	/// Returns the longDescription, loading it from the database if required.
	QString longDescription() const;
	/// Returns the failure message for the Action
	QString failureMessage() const;
	/// Returns the final state of the action, loading it from the database if required.
	int finalState() const;
	/// Returns the iconFileName of the action, loading it from the database if required.
	QString iconFileName() const;
	/// Returns the copy-ability of the action, loading it from the database if required.
	bool canCopy() const;
	/// Returns the database id of the parent log action, if none then returns -1
	int parentId() const;
	/// Returns whether or not the action logged here inhereted the loop action
	bool actionInheritedLoop() const;
	/// Returns the number of loops this action performed or -1 is this was not a loop action
	int numberOfLoops() const;

	int numberOfChildren() const;

	void updateNumberOfChildren(int numberOfChildren) const;

	/// Returns true if the this log item is to show a seleced-like color because its parent is selected. Pass in the item view you're working with.
	bool parentSelected(QAbstractItemView *viewer) const;
	/// Returns the full map of item views and whether this log item should show the selected-like color
	ParentSelectMap allParentSelected() const;

	/// Sets whether this log item should show the selected-like color because its parent is selected. Pass in the item view in question and whether or not it is to be selected-like.
	void setParentSelected(QAbstractItemView *viewer, bool parentIsSelected);

protected:

	/// Helper function to retrieve the log information from the database (without loading the full AMActionLog, which would require loading its entire AMActionInfo as well.) Returns true and sets loadedFromDb_ = true on success.
	bool loadLogDetailsFromDb() const;

	/// The database that stores this action log:
	AMDatabase* db_;
	/// The id of this action log in the database:
	int id_;
	/// True if we've already loaded and cached the remaining information from the database
	mutable bool loadedFromDb_;

	mutable int numberOfChildren_;

	// These variables cache the content that was loaded from the database
	/////////////////
	mutable QString shortDescription_;
	mutable QString longDescription_;
	mutable int finalState_;
	mutable QString iconFileName_;
	mutable QDateTime startDateTime_, endDateTime_;
	mutable QString failureMessage_;
	mutable bool canCopy_;
	mutable int parentId_;
	mutable bool actionInheritedLoop_;
	mutable int numberOfLoops_;

	/// Hold the mapping for selected-like coloring based on parent selection state for each item view
	ParentSelectMap parentSelected_;
};

/// This QAbstractItemModel implements a model for completed workflow actions, used by AMActionHistoryView. You should never need to use this class directly.
/*! Like AMActionRunner's AMActionQueueModel, this model is hierarchichal, since we're storing completed actions inside their respective loop and list actions. */
class AMActionHistoryModel3 : public QAbstractItemModel
{
	Q_OBJECT
public:
	enum DataRoles { ParentSelectRole = AM::UserRole +27000 };

	/// Constructor: \c db is the actions database to show completed actions from.  Does not refresh the model automatically on creation; you will need to call refreshFromDb().
	AMActionHistoryModel3(AMDatabase* db, QObject* parent = 0);
	/// Destructor: deletes all the AMActionItemLog instances in items_
	virtual ~AMActionHistoryModel3();

	// Public accessor methods
	////////////////////

	/// Returns the limit on the number of actions that will be pulled from the database and displayed. This might be larger than rowCount() if there aren't enough actions in the visible range to hit the limit.  Caveat: sometimes it will also be smaller than rowCount(), if we've appended new recently-finished actions since a full database refresh -- In that operation, the limit is not obeyed.
	int maximumActionsToDisplay() const { return maximumActionsLimit_; }
	/// Returns a good number of actions for a new maximum. This value will either be enough to load the next top level action or 4/3 the current maximum, whichever is bigger.
	int nextGoodMaximumActions() const;
	/// Returns the total number of actions that would be included in the visible range, if maximumActionsToDisplay() was not restricting anything.
	int visibleActionsCount() const { return visibleActionsCount_; }

	// Re-implemented public functions from QAbstractItemModel
	/////////////////////
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	/// Returns whether or not this item has children (determines this from the underlying list)
	bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
	/// Returns the number of children below a certain model index (rowCount and combined rowCount of children recursively)
	int childrenCount(const QModelIndex &parent = QModelIndex()) const;
	/// Returns the number of children below a certain model index that actually succeeded (recursive)
	int successfulChildrenCount(const QModelIndex &parent = QModelIndex()) const;

	/// Returns the AMActionLogItem at \c index
	AMActionLogItem3* logItem(const QModelIndex& index) const;
	/// Returns the model index of a given AMPointerTreeNode
	QModelIndex indexForNode(const AMPointerTreeNode *node) const;
	/// Returns the model index for a given AMActionLogItem
	QModelIndex indexForLogItem(AMActionLogItem3 *logItem) const;

	/// Returns the top level parent of a given item (one level down from the root item)
	QModelIndex topLevelParent(const QModelIndex &child) const;

	/// Returns the indices in between two model indexes. Used in shift-clicking routine.
	const QItemSelection indicesBetween(const QModelIndex &brother, const QModelIndex &sister) const;

	/// Marks a given index as selected and sets the parentSelected maps for all the children of this index
	void markIndexAsSelected(const QModelIndex &index, QAbstractItemView *viewer);
	/// Marks a given index as not selected and sets the parentSelected maps for all the children of this index
	void markIndexAsDeselected(const QModelIndex &index, QAbstractItemView *viewer);

	/// Used for marking a group of children of a loop as selected (if you select one, you select them all)
	void markIndexGroupAsSelected(const QModelIndex &index, QAbstractItemView *viewer);
	/// Used for marking a group of children of a loop as not selected (if you deselect one, you deselect them all)
	void markIndexGroupAsDeselected(const QModelIndex &index, QAbstractItemView *viewer);

public slots:
	/// Set the default maximum number of actions to show. If more actions exist within the visibleDateTimeRange, only the most recent \c maximumActionsCount will be shown.
	/*! Automatically calls refreshFromDb() if the maximumActionsCount has changed, when control returns to the event loop.*/
	void setMaximumActionsToDisplay(int maximumActionsLimit);

	/// Refreshes the entire model immediately by reading from the database.
	void refreshFromDb();

	bool logUncompletedAction(const AMAction3 *uncompletedAction, AMDatabase* database, int parentLogId = -1);
	bool updateCompletedAction(const AMAction3 *completedAction, AMDatabase* database);
	bool logCompletedAction(const AMAction3* completedAction, AMDatabase* database, int parentLogId = -1);

signals:
	/// This signal is emitted before the model is refreshed or updated. Views might want to use it to remember their scrolling position, etc.
	void modelAboutToBeRefreshed();
	/// This signal is emitted after the model is refreshed or updated. Views might want to use it to restore their scrolling position, etc.
	void modelRefreshed();

protected slots:
	/// Called to refresh only the items belonging to a specific set of AMActionLog ids, found in idsRequiringRefresh_.  Used to optimize our response when one or more actionLogs are updated in the database. Note that these ids may or may not be visible for us.
	void refreshSpecificIds();

protected:
	/// Helper function to append a row to the model with a given AMActionLogItem \c item. We don't have a public addRow() / insertRow() interface.
	void appendItem(AMActionLogItem3* item);

	/// Helper function to clear the whole model. (Deletes all the items)
	void clear();

	/// Helper function to recurse through the actions log database information and populate the list
	bool recurseActionsLogLevelCreate(int parentId, QMap<int, int> parentIdsAndIds);

	/// Helper function to recursively mark children as parentSelected or not for a given item view in the mapping
	void recurseMarkParentSelected(const QModelIndex &index, QAbstractItemView *viewer, bool selected);
	/// Helper function to mark an index group (in a loop) as parentSelected or not
	void markIndexGroup(const QModelIndex &index, QAbstractItemView *viewer, bool selected);

protected:
	/// Database to be used
	AMDatabase* db_;
	/// Maximum number of action logs to show
	int maximumActionsLimit_;
	/// Count of the visible number of actions
	int visibleActionsCount_;
	/// A list of all the current top level ids loaded from the database. Used on refreshes to figure out how many more actions to show.
	QList<int> topLevelIds_;

	/// Database table name of the action log class
	QString actionLogTableName_;

	/// The underlying tree model
	AMPointerTree *itemTree_;
	/// The root item of the underlying tree model
	AMPointerTreeNode *itemTreeRoot_;
	/// A quick mapping of actionLog id to its AMPointerTreeNode
	QHash<int, AMPointerTreeNode*> idsToTreeNodes_;
	QHash<const AMActionInfo3*, AMActionLog3*> infosToLogsForUncompletedActions_;

	/// Used to schedule a delayed call to refreshFromDb()
	AMDeferredFunctionCall refreshFunctionCall_;
	/// Used to schedule a delayed call to refreshSpecificIds()
	AMDeferredFunctionCall specificRefreshFunctionCall_;

	/// Used to optimize our response when one or more actionLogs are updated in the database. Contains a set of ids that need to be refreshed. Note that these ids may or may not be visible for us.
	QSet<int> idsRequiringRefresh_;

	/// Caches some icons to represent Succeeded, Cancelled, Failed
	QPixmap succeededIcon_, cancelledIcon_, failedIcon_, unknownIcon_;
};

#endif // AMACTIONHISTORYMODEL_H
