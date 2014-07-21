#ifndef AMSCANDATAVIEW_H
#define AMSCANDATAVIEW_H

#include <QWidget>
#include <QTableView> // Can be removed after testing
#include <QButtonGroup>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QUrl>
#include <QStackedWidget>
#include <QMenu>

class AMLightweightScanInfoModel;
class AMLightweightScanInfoFilterProxyModel;
class AMDatabase;
class AMSortFilterWidget;
/// Class which represents the main display view for scans loaded from the database
class AMScanDataView : public QWidget
{
	Q_OBJECT
private:
	/// A widget which controls the filtering of data within all the contained views
	AMSortFilterWidget* sortFilterWidget_;
	/// A tool button for displaying the sortFilterWidget
	QToolButton* searchButton_;
	/// A tab widget containing the child views
	QStackedWidget* stackedWidget_;
	/// The layout containing the buttons to switch between child views
	QHBoxLayout* buttonLayout_;
	/// The model containing the scan info
	AMLightweightScanInfoModel* model_;
	/// The proxy model which controls sorting and filtering of the scans
	AMLightweightScanInfoFilterProxyModel* proxyModel_;
	/// A list of child views added to the scan data view
	QList<QAbstractItemView*> childViews_;
	/// Abstract container for the buttons used to switch views
	QButtonGroup* viewButtons_;
	/// Label displaying the title of the scan view
	QLabel* titleLabel_;
	/// Context menu to allow user to open scans, configs etc.
	QMenu* contextMenu_;
public:
	/// Creates an instance of an AMScanDataView, loaded scans from the provided AMDatabase
	explicit AMScanDataView(AMDatabase* database, QWidget *parent = 0);
protected:
	/// Adds an AMAbstractScanDataChildView to the ScanDataView
	void addChildView(QAbstractItemView* childView, const QIcon &icon);
	/// Initializes all the child views that will be shown in the ScanDataView
	void initializeChildViews();
	/// The child view which currently is showing. If there are no child views, 0 is returned.
	QAbstractItemView* currentView();
	/// A list of all the items selected in the currently displayed child view in the standard
	/// URL format: amd://databaseConnectionName/tableName/objectId
	QList<QUrl> selectedItems();
	/// The number of items selected in the currently displayed child view
	int numberOfSelectedItems();
signals:
	/// Emitted whenever the selected scans change.  You can then use selectedItems() to get a list of the scan URLs, or numberOfSelectedItems() to find out how many there are.
	void selectionChanged();
	/// Emitted when the user double-clicks anywhere in the view.
	void viewDoubleClicked();
	/// Emitted when the user chooses to open a scan configuration from the database.
	void launchScanConfigurationsFromDb();
	/// Emitted when the user chooses to edit scans.
	void editScansFromDb();
	/// Emitted when the user chooses to compare scans.
	void compareScansFromDb();
	/// Emitted when the user wants to export scans.
	void exportScansFromDb();
	/// Emitted when the user wants to fix a scan that uses CDF.
	void fixCDF();
	/// Emitted when the user attempts to open the selected scans
	void selectionActivated(const QList<QUrl>&);
	/// Emitted when the user attempts to open the selected scans in separate windows
	void selectionActivatedSeparateWindows(const QList<QUrl> &);
	/// Emitted when the user attempts to export the selected scans. (This action is (hopefully) completed elsewhere, so that we don't couple the AMDataView to the export system)
	void selectionExported(const QList<QUrl>&);
	/// Emitted when the user wants to open scan configurations of the selected scans from the database.
	void launchScanConfigurationsFromDb(const QList<QUrl> &);
	/// Emitted when the user wants to fix a scan that uses CDF files.
	void fixCDF(const QUrl &);
public slots:
	/// Specifies that the AMScanDataView should filter for all runs that match the passed id. -1 indicates
	/// that all scans should be shown.
	void showRun(int runId = -1);

	/// Specifies that the AMScanDataView should filter for all experiments that match the passed if. -1 indicates
	/// that all scans should be shown
	void showExperiment(int experimentId = -1);

protected slots:
	/// Handles the search button being clicked. Shows the sortFilterWidget
	void onSearchButtonClicked(bool);

	/// Handles the SortFilterWidget changing state from filtered, to not filtered
	void onFilterStateChanged(bool);

	/// Handles the user double-clicking on the current view
	void onChildViewDoubleClicked();

	/// Handles the current child view signaling that a custom context menu has been requested
	void onCustomContextMenuRequested(const QPoint &position);

	void onEditScan();

	void onCompareScans();

	void onExportScans();

	void onShowScanConfiguration();

	void onFixCDF();
};

#endif // AMSCANDATAVIEW_H
