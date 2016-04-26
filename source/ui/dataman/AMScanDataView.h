#ifndef AMSCANDATAVIEW_H
#define AMSCANDATAVIEW_H

#include <QWidget>
#include <QButtonGroup>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QUrl>
#include <QMenu>
#include <QMessageBox>

#define AMSCANDATAVIEW_CANT_OPEN_EXPORT_FOLDER 103114

class AMDatabase;
class AMBrowseScansView;
/// Class which represents the main display view for scans loaded from the database
class AMScanDataView : public QWidget
{
	Q_OBJECT

public:
	/// Creates an instance of an AMScanDataView, loaded scans from the provided AMDatabase
	explicit AMScanDataView(AMDatabase* database, QWidget *parent = 0);
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
	/// Emitted when the user attempts to open the selected scans
	void selectionActivated(const QList<QUrl>&);
	/// Emitted when the user attempts to open the selected scans in separate windows
	void selectionActivatedSeparateWindows(const QList<QUrl> &);
	/// Emitted when the user attempts to export the selected scans. (This action is (hopefully) completed elsewhere, so that we don't couple the AMDataView to the export system)
	void selectionExported(const QList<QUrl>&);
	/// Emitted when the user wants to open scan configurations of the selected scans from the database.
	void launchScanConfigurationsFromDb(const QList<QUrl> &);

public slots:
	/// Specifies that the AMScanDataView should filter for all runs that match the passed id. -1 indicates
	/// that all scans should be shown.
	void showRun(int runId = -1);
	/// Specifies that the AMScanDataView should filter for all experiments that match the passed if. -1 indicates
	/// that all scans should be shown
	void showExperiment(int experimentId = -1);

protected slots:

	/// Handles the SortFilterWidget changing its filter
	void onFilterChanged(bool);
	/// Handles the user double-clicking on the current view
	void onChildViewDoubleClicked();
	/// Handles the current child view signaling that a custom context menu has been requested
	void onCustomContextMenuRequested(const QPoint &position);
	/// Handles the user choosing to edit scans in separate windows
	void onEditScan();
	/// Handles the user choosing to edit scans in the same window
	void onCompareScans();
	/// Handles the user choosing to export selected scans
	void onExportScans();
	/// Handles the user choosing to show selected scan configurations
	void onShowScanConfiguration();
	/// Handles the user choosing to open user export data folder
	void onexportDataFolderButtonClicked();
	/// Handles the selected items in the current child view changing
	void onChildViewSelectionChanged();

protected:
	/// A list of all the items selected in the currently displayed child view in the standard
	/// URL format: amd://databaseConnectionName/tableName/objectId
	QList<QUrl> selectedItems();
	/// The number of items selected in the currently displayed child view
	int numberOfSelectedItems();

private:
	/// Widget which displays and filters all the current scan info in the passed database
	AMBrowseScansView* browseScansView_;
	/// Label displaying the title of the scan view
	QLabel* titleLabel_;
	/// Context menu to allow user to open scans, configs etc.
	QMenu* contextMenu_;
	/// Action button to allow user to edit selected scans in individual windows
	QToolButton* editButton_;
	/// Action button to allow user to open selected scans in the same window
	QToolButton* compareButton_;
	/// Action button to allow user to export all the selected scans
	QToolButton* exportButton_;
	/// Action button to allow user to open the scan configuration of the selected scans
	QToolButton* configButton_;
	/// Action button to allow user to open their expotData folder
	QToolButton* exportDataFolderButton_;

};

#endif // AMSCANDATAVIEW_H
