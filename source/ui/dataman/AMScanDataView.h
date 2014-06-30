#ifndef AMSCANDATAVIEW_H
#define AMSCANDATAVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QSortFilterProxyModel>
#include <QButtonGroup>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "dataman/AMLightweightScanInfoModel.h"
#include "AMAbstractScanDataChildView.h"
#include "ui/util/AMSortFilterWidget.h"
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
public:
	/// Creates an instance of an AMScanDataView, loaded scans from the provided AMDatabase
	explicit AMScanDataView(AMDatabase* database, QWidget *parent = 0);
protected:
	/// Adds an AMAbstractScanDataChildView to the ScanDataView
	void addChildView(AMAbstractScanDataChildView* childView);
signals:
	
public slots:
	/// Specifies that the AMScanDataView should filter for all runs that match the passed id. -1 indicates
	/// that all runs should be shown.
	void showRun(int runId = -1);

protected slots:
	/// Handles the search button being clicked. Shows the sortFilterWidget.
	void onSearchButtonClicked();

	void onSortFilterWidgetLostFocus();
	
};

#endif // AMSCANDATAVIEW_H
