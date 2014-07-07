#ifndef AMSCANDATAVIEW_H
#define AMSCANDATAVIEW_H

#include <QWidget>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QButtonGroup>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QUrl>
#include <QHeaderView>

class AMLightweightScanInfoModel;
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
	QTabWidget* tabWidget_;
	/// The layout containing the buttons to switch between child views
	QHBoxLayout* buttonLayout_;
	/// The model containing the scan info
	AMLightweightScanInfoModel* model_;
	/// The proxy model which controls sorting and filtering of the scans
	QSortFilterProxyModel* proxyModel_;
	/// A list of child views added to the scan data view
	QList<QAbstractItemView*> childViews_;
public:
	/// Creates an instance of an AMScanDataView, loaded scans from the provided AMDatabase
	explicit AMScanDataView(AMDatabase* database, QWidget *parent = 0);
protected:
	/// Adds an AMAbstractScanDataChildView to the ScanDataView
	void addChildView(QAbstractItemView* childView, const QString &title);
	/// Initializes all the child views that will be shown in the ScanDataView
	void initializeChildViews();
	/// The child view which currently is showing. If there are no child views, 0 is returned.
	QAbstractItemView* currentView();
	/// A list of all the items selected in the currently displayed child view in the standard
	/// URL format: amd://databaseConnectionName/tableName/objectId
	QList<QUrl> selectedItems();
	/// The number if items selected in the currently displayed child view
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
	/// that all runs should be shown.
	void showRun(int runId = -1);
	/// setup this view to show a specific experiment (or use \c experimentId = -1 to see all experiments)
	void showExperiment(int experimentId = -1);
	/// Set the headers used to separate and organize scans within a view. The \c mode is one of AMDataViews::OrganizeMode.
	void setOrganizeMode(int mode);
	/// Set the view mode, where \c mode is one of AMDataViews::ViewMode (ThumbnailView, ListView, FlowView, or DetailView)
	void setViewMode(int mode);
	/// Called when the user wants to expand all the sections in the view
	void expandAll();
	/// Called when the user wants to collapse all the sections in the view
	void collapseAll();
	/// Called when the item size slider is moved. It's up to each data view section to decide what item sizes mean, but they should all adjust their item sizes based on the new user value (from 1 to 100).
	void setItemSize(int newItemSize);

protected slots:
	/// Handles the search button being clicked. Shows the sortFilterWidget
	void onSearchButtonClicked();

	void onSortFilterWidgetLostFocus();
	
};

#endif // AMSCANDATAVIEW_H
