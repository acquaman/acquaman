#ifndef AMACTIONHISTORYVIEW_H
#define AMACTIONHISTORYVIEW_H

#include <QWidget>
#include <QDateTime>
#include <QAbstractItemModel>

#include "dataman/database/AMDatabase.h"
#include "util/AMDeferredFunctionCall.h"

class AMActionLog;

/// This item class is used to cache the details of a completed workflow action inside AMActionHistoryModel. You should never need to use this class directly.
/*! We use it instead of full AMActionLog instances, so that we don't have to load the complete AMActionInfo (which, depending on the subclass, could be a lot to load.)*/
class AMActionLogItem {
public:
	/// Constructor requires the \c id of the AMActionLog and the database \c db where it is stored.
	AMActionLogItem(AMDatabase* db, int id);

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
	/// Returns the final state of the action, loading it from the database if required.
	int finalState() const;
	/// Returns the iconFileName of the action, loading it from the database if required.
	QString iconFileName() const;

protected:

	/// Helper function to retrieve the log information from the database (without loading the full AMActionLog, which would require loading its entire AMActionInfo as well.) Returns true and sets loadedFromDb_ = true on success.
	bool loadLogDetailsFromDb() const;

	/// The database that stores this action log:
	AMDatabase* db_;
	/// The id of this action log in the database:
	int id_;
	/// True if we've already loaded and cached the remaining information from the database
	mutable bool loadedFromDb_;

	// These variables cache the content that was loaded from the database
	/////////////////
	mutable QString shortDescription_;
	mutable QString longDescription_;
	mutable int finalState_;
	mutable QString iconFileName_;
	mutable QDateTime startDateTime_, endDateTime_;
};

/// This QAbstractItemModel implements a model for completed workflow actions, used by AMActionHistoryView. You should never need to use this class directly.
/*! In contrast to AMActionRunner's AMActionQueueModel, this model is flat and non-hierarchichal, since we're storing only completed actions -- no loop actions. */
class AMActionHistoryModel : public QAbstractItemModel
{
	Q_OBJECT
public:

	/// Constructor: \c db is the user database to show completed actions from.  Does not refresh the model automatically on creation; you will need to call refreshFromDb().
	AMActionHistoryModel(AMDatabase* db, QObject* parent = 0);
	/// Destructor: deletes all the AMActionItemLog instances in items_
	virtual ~AMActionHistoryModel();

	// Public accessor methods
	////////////////////

	/// Returns the limit on the number of actions that will be pulled from the database and displayed. This might be larger than rowCount() if there aren't enough actions in the visible range to hit the limit.  Caveat: sometimes it will also be smaller than rowCount(), if we've appended new recently-finished actions since a full database refresh -- In that operation, the limit is not obeyed.
	int maximumActionsToDisplay() const { return maximumActionsLimit_; }
	/// Returns the oldest dateTime limit of our visible range. Actions older than this will not be shown.  Returns an invalid QDateTime if there is no oldest limit.
	QDateTime visibleRangeOldestDateTime() const { return visibleRangeOldest_; }
	/// Returns the newest (ie: most recent) dateTime limit of our visible range.  Actions more recent than this will not be shown. Returns an invalid QDateTime if there is no newest limit.
	QDateTime visibleRangeNewestDateTime() const { return visibleRangeNewest_; }
	/// Returns the total number of actions that would be included in the visible range, if maximumActionsToDisplay() was not restricting anything.
	int visibleActionsCount() const { return visibleActionsCount_; }

	// Re-implemented public functions from QAbstractItemModel
	/////////////////////
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	/// Returns the AMActionLogItem at \c index
	AMActionLogItem* logItem(const QModelIndex& index) const;

public slots:
	/// Set the date/time range of actions that should be shown. Specificy an invalid QDateTime for \c oldest to show all.  Specify an invalid QDateTime for \c newest to always be the current date time.
	/*! Automatically calls refreshFromDb() if the time range has changed, when control returns to the event loop.*/
	void setVisibleDateTimeRange(const QDateTime& oldest, const QDateTime& newest = QDateTime());

	/// Set the default maximum number of actions to show. If more actions exist within the visibleDateTimeRange, only the most recent \c maximumActionsCount will be shown.
	/*! Automatically calls refreshFromDb() if the maximumActionsCount has changed, when control returns to the event loop.*/
	void setMaximumActionsToDisplay(int maximumActionsLimit);

	/// Refreshes the entire model immediately by reading from the database.
	void refreshFromDb();

signals:
	/// This signal is emitted before the model is refreshed or updated. Views might want to use it to remember their scrolling position, etc.
	void modelAboutToBeRefreshed();
	/// This signal is emitted after the model is refreshed or updated. Views might want to use it to restore their scrolling position, etc.
	void modelRefreshed();

protected slots:
	/// Called when the database table has an actionLog added. If possible, simply appends that action to the model instead of doing a full refresh.
	void onDatabaseItemCreated(const QString& tableName, int id);
	/// Called when the database table has an actionLog changed. Simply updates the action instead of doing a full refresh.
	void onDatabaseItemUpdated(const QString& tableName, int id);
	/// Called when the database table has an actionLog removed. Triggers a full refresh of the model.
	void onDatabaseItemRemoved(const QString& tableName, int id);

	/// Called to refresh only the items belonging to a specific set of AMActionLog ids, found in idsRequiringRefresh_.  Used to optimize our response when one or more actionLogs are updated in the database. Note that these ids may or may not be visible for us.
	void refreshSpecificIds();

protected:
	/// Helper function that returns true if the given \c dateTime is within the visibleDateTimeRange.
	/*! Remember that visibleRangeOldest_ and visibleRangeNewest_ are ignored (no restrictions) if they are invalid. */
	bool insideVisibleDateTimeRange(const QDateTime& dateTime);

	/// Helper function to append a row to the model with a given AMActionLogItem \c item. We don't have a public addRow() / insertRow() interface.
	void appendItem(AMActionLogItem* item);
	/// Helper function to delete a row in the model, and delete the item at that row. We don't have a public removeRow() / removeRows() interface.
	bool deleteRow(int index);

	/// Helper function to clear the whole model. (Deletes all the items)
	void clear();


	AMDatabase* db_;
	QDateTime visibleRangeOldest_, visibleRangeNewest_;
	int maximumActionsLimit_;
	int visibleActionsCount_;

	QString actionLogTableName_;

	/// A list of these holds the actionLog data that we return in data()
	QList<AMActionLogItem*> items_;

	/// Used to schedule a delayed call to refreshFromDb()
	AMDeferredFunctionCall refreshFunctionCall_;
	/// Used to schedule a delayed call to refreshSpecificIds()
	AMDeferredFunctionCall specificRefreshFunctionCall_;

	/// Used to optimize our response when one or more actionLogs are updated in the database. Contains a set of ids that need to be refreshed. Note that these ids may or may not be visible for us.
	QSet<int> idsRequiringRefresh_;

	/// Caches some icons to represent Succeeded, Cancelled, Failed
	QPixmap succeededIcon_, cancelledIcon_, failedIcon_, unknownIcon_;
};

class AMActionRunner;
class QTreeView;
class QFrame;
class QComboBox;
class QLabel;
class QPushButton;

/// This UI class provides a view of a user's completed actions that were run with AMActionRunner. It is part of the overall AMWorkflowView.
/*! The view can be used to browse completed actions, as well as re-instantiate copies of them in the AMActionRunner queue (as long as the specific AMAction/AMActionInfo pair has been registered with AMActionRegistry).*/
class AMActionHistoryView : public QWidget
{
    Q_OBJECT
public:
	/// Constructor. If you want this history view to be able to re-instantiate and re-queue actions, pass in valid AMActionRunner, ie: AMActionRunner::s().  The default (\c actionRunner = 0) is suitable for a Dataman-only program, and can only browse the user's action history.
	AMActionHistoryView(AMActionRunner* actionRunner = 0, AMDatabase* db = AMDatabase::database("user"), QWidget *parent = 0);

	/// Returns true if the view is currently collapsed to show only the header bar, and false if it is fully shown.
	bool isCollapsed() const { return isCollapsed_; }

signals:
	/// This signal is emitted with \c true when the view widget is collapsed to show only the header bar, and \c false when it is restored.
	void collapsed(bool isCollapsed);

public slots:

	/// Collapses the view to show only the header bar.
	void collapse(bool doCollapse);

protected slots:
	/// Called when the "show more actions" button is clicked. Doubles the model's maximumActionsToDisplay().
	void onShowMoreActionsButtonClicked();
	/// Called when one of the preset time-range filters is clicked. Calls the model's setVisibleTimeRange().
	void onRangeComboBoxActivated(int rangeIndex);
	/// Called when the "Re-run this action" button is clicked. If we were constructed with access to an AMActionRunner, and the AMActionInfo associated with the selected items have been registered in AMActionRegistry, then we will re-queue copies of these actions in the AMActionRunner.
	void onReRunActionButtonClicked();
	/// Called when the selection changes... We use this to determine whether to enable the reRunActionButton_
	void onSelectionChanged();


	/// Called right before the model is refreshed. Remember if we're scrolled right to the bottom, so we can go back there.
	void onModelAboutToBeRefreshed();
	/// Called right after the model is refreshed.  If we were scrolled to the bottom, let's go back to the new bottom. This lets users always see the most recently-completed actions.
	void onModelRefreshed();


protected:
	AMActionRunner* actionRunner_;
	AMDatabase* db_;

	AMActionHistoryModel* model_;

	QTreeView* treeView_;
	QComboBox* rangeComboBox_;
	QLabel* headerTitle_, *headerSubTitle_;
	QPushButton* showMoreActionsButton_;
	QPushButton* reRunActionButton_;

	bool scrolledToBottom_;
	bool isCollapsed_;

	/// If we should be scrolled to the bottom of the treeView_, sometimes the scrolling doesn't happen if we're not visible. Here we make sure it does.
	void showEvent(QShowEvent *);

};

#endif // AMACTIONHISTORYVIEW_H
