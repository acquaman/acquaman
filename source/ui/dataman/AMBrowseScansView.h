#ifndef AMBROWSESCANSVIEW_H
#define AMBROWSESCANSVIEW_H

#include <QWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAbstractItemView>
#include <QButtonGroup>
#include <QLabel>
#include <QUrl>
#include <QIcon>
#include <QToolButton>

class AMSortFilterWidget;
class AMDatabase;
class AMLightweightScanInfoModel;
class AMLightweightScanInfoFilterProxyModel;
/**
 * A widget class which allows users to browse scans from the database in a number of different ways:
 * - A table view
 * - A thumbnail view
 * - A tree view
 * Also includes a SortFilter widget to allow for the filtering of the scans
 */
class AMBrowseScansView : public QWidget
{
	Q_OBJECT
public:
	explicit AMBrowseScansView(AMDatabase* database, QWidget *parent = 0);
	/// A list of all the items selected in the currently displayed child view in the standard
	/// URL format: amd://databaseConnectionName/tableName/objectId
	QList<QUrl> selectedItems();
	/// The number of items selected in the currently displayed child view
	int numberOfSelectedItems();
	/// Adds a top level filter to the currently viewed scans by RunId
	void setRunId(int runId);
	/// Adds a top level filter to the currently viewed scans by ExperimentId
	void setExperimentId(int experimentId);
	/// Returns the number of displayed roles in the model
	int numberOfDisplayedRows();
signals:
	/// Emitted whenever the user changes the selected scans in the current child view
	void selectionChanged();
	/// Emitted whenever the user double clicks on the current child view
	void childViewDoubleClicked(const QModelIndex& index);
	/// Emitted whenever a context menu is requested for one of the child views
	void childContextMenuRequested(const QPoint& point);
	/// Emitted whenever the user changes the filter
	void filterChanged(bool);
public slots:
	/// Clears the current selection from all child views.
	void clearSelection();
	/// Selects all items in the current child view.
	void selectAll();
protected slots:
	/// Slot which handles the user changing the selected items in the current child view
	void onChildViewSelectionChanged();
	/// Slot which handles the user clicking one of the change view buttons
	void onChildViewChanged(int newIndex);
	/// Slot which handles a request for a context menu from one of the child views
	void onContextMenuRequested(const QPoint& point);
protected:
	/// Adds an QAbstractItemView to the ScanDataView
	void addChildView(QAbstractItemView* childView, const QIcon &icon);
	/// Initializes all the child views that will be shown in the ScanDataView
	void initializeChildViews();
	/// The child view which currently is showing. If there are no child views, 0 is returned.
	QAbstractItemView* currentView();
private slots:
private:
	QLabel* selectedItemCount_;
	QStackedWidget* childViewDisplayArea_;
	QVector<QAbstractItemView*> childViews_;
	AMSortFilterWidget* sortFilterWidget_;
	QButtonGroup* viewButtonGroup_;
	QHBoxLayout* viewButtonLayout_;
	AMLightweightScanInfoModel* model_;
	AMLightweightScanInfoFilterProxyModel* proxyModel_;

};

#endif // AMBROWSESCANSVIEW_H
