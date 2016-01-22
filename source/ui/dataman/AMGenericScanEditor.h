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


#ifndef AMGENERICSCANEDITOR_H
#define AMGENERICSCANEDITOR_H

#include <QWidget>
#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListView>

#include "AMQPlainTextEdit.h"

#include <QPushButton>
#include <QSpacerItem>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>
#include <QMetaType>

#include "dataman/AMScanSetModel.h"
#include "dataman/AMAxisInfo.h"
#include "ui/AMRegExpLineEdit.h"

class AMScan;
class AMScanView;
class AM2DScanView;
class AMVerticalStackWidget;
class AMRunSelector;
class AMSamplePre2013Editor;
class AMSampleBriefView;
class AMDataSourcesEditor;
class AMChooseScanDialog;
class AMControlInfoListTableView;
class AMDetailedItemDelegate;

class AMGenericScanEditor : public QWidget
{
Q_OBJECT
public:

	/// Construct an empty editor.  This builds an editor using the default AMScanView.
	explicit AMGenericScanEditor(QWidget *parent = 0);
	/// Overloaded.  Constructs an empty editor using the scan view chosen by \param use2DScanView.  If use2DscanView is true then a valid AM2DScanConfiguration must also be provided.
	explicit AMGenericScanEditor(bool use2DScanView, QWidget *parent = 0);

	/// Deletes self and releases all scan objects that were added
	virtual ~AMGenericScanEditor();

	/// Add a scan to an editor. The editor retains an interest in \c newScan, keeping it in memory as long as required. \see AMScan::retain().
	void addScan(AMScan* newScan);

	/// Remove a scan, but ask the user for confirmation if it's been modified.  Returns true if the scan was deleted, and false if the user 'cancelled' the process.
	bool removeScanWithModifiedCheck(AMScan* scan);

	/// Remove a scan from the editor.  The editor releases its interest in \c scan, which might cause it to be deleted if nothing else is using it. \see AMScan::release().
	/*! To prevent this from happening (for example, because you want to re-use the scan instance outside of this editor), you need to retain it first:
	  \code
	  scan->retain(newOwner);
	  editor->removeScan(scan);
	  \endcode
	  */
	void removeScan(AMScan* scan);

	/// Returns the number of scans open in the editor.
	int scanCount() const {
		return scanSetModel_->scanCount();
	}

	/// Returns a particular scan pointer, by index. Returns 0 if \c index is out of bounds.
	AMScan* scanAt(int index) const {
		if(index<0 || index>=scanCount())
			return 0;
		return scanSetModel_->scanAt(index);
	}

	/// Returns the current scan that the generic scan editor is looking at.
	AMScan *currentScan() const { return currentScan_; }

	/// Call this function to find out if this editor can be closed. Will prompt the user to save with a message box if promptToSave if true (which is the default). Checks for scans in progress and prompts the user for what to do with modified scans.  Returns true if the editor can be closed, returns false if any scans are acquiring or if the user responded "cancel" to a save-request.
	bool canCloseEditor(bool promptToSave = true);

	/// Returns the current exclusive data source name for the model.
	QString exclusiveDataSourceName() const { return scanSetModel_->exclusiveDataSourceName(); }
	/// Sets which data source should be viewed by exclusive views.  Returns true if the data source was found and set.  Returns false otherwise.
	bool setExclusiveDataSourceByName(const QString &name) { return scanSetModel_->setExclusiveDataSourceByName(name); }

	/// Exposing scan set model method that returns a list of all data source names that exist (over all scans in the model). Warning: this is slow.  O(n), where n is the total number of data sources in all scans.
	QStringList allDataSourceNames() const { return scanSetModel_->allDataSourceNames(); }

	/// Exposing scan set model method that returns a list of all data source names that exist and are visible in at least one scan. Warning: this is slow.  O(n), where n is the total number of data sources in all scans.
	QStringList visibleDataSourceNames() const { return scanSetModel_->visibleDataSourceNames(); }

	// Methods for 2D scans only.
	/////////////////////////////////////////////////

	/// Returns the data position inside a two dimensional scan.  This returns a null point if AMGenericScanEditor is not using AM2DScanView, or if no valid point was been chosen yet.
	QPointF dataPosition() const;
	/// Returns the selected rectangle inside a two dimensional scan.  This returns a null rect if AMGenericScanEditor is not using AM2DScanView, or if there is no valid rectangle displayed.
	QRectF selectedRect() const;
	/// Returns whether the generic scan editor is using AMScanView or AM2DScanView.
	bool using2DScanView() const { return scanView2D_ ? true : false; }
	/// Sets the axis information for the spectrum view. Does nothing if not using 2D scan view. This will automatically set the plot range for markers as well. If you need to customize the plot range to something more specific, set \param propogateToPlotRange to false and call setPlotRange().
	void setAxisInfoForSpectrumView(const AMAxisInfo &info, bool propogateToPlotRange = true);
	/// Sets the plot range for markers to be displayed on the spectrum view.  Does nothing if not using 2D scan view.
	void setEnergyRange(double low, double high);
	/// Sets the emission line name filter for the single spectrum view.
	void addSingleSpectrumEmissionLineNameFilter(const QRegExp &newNameFilter);
	/// Sets the emission line name filter for the single spectrum view.
	void removeSingleSpectrumEmissionLineNameFilter(int index);
	/// Sets the emission line name filter for the single spectrum view.
	void removeSingleSpectrumEmissionLineNameFilter(const QRegExp &filter);
	/// Sets the emission line name filter for the single spectrum view.
	void addSingleSpectrumPileUpPeakNameFilter(const QRegExp &newNameFilter);
	/// Sets the emission line name filter for the single spectrum view.
	void removeSingleSpectrumPileUpPeakNameFilter(int index);
	/// Sets the emission line name filter for the single spectrum view.
	void removeSingleSpectrumPileUpPeakNameFilter(const QRegExp &filter);
	/// Sets the emission line name filter for the single spectrum view.
	void addSingleSpectrumCombinationPileUpPeakNameFilter(const QRegExp &newNameFilter);
	/// Sets the emission line name filter for the single spectrum view.
	void removeSingleSpectrumCombinationPileUpPeakNameFilter(int index);
	/// Sets the emission line name filter for the single spectrum view.
	void removeSingleSpectrumCombinationPileUpPeakNameFilter(const QRegExp &filter);
	/// Sets the data source name that should be visualized when using AMSingleSpectrumView inside of AM2DScanView.  Does nothing if not using AM2DScanView.
	void setSingleSpectrumViewDataSourceName(const QString &name);

signals:
	/// Internal signal to forward the textChanged() from ui_.notesEdit
	void notesChanged(const QString&);
	/// Emits a signal that the data position tool has changed positions.  This is only emitted if AMGenericScanEditor is using AM2DScanView.  Passes a reference to the scan editor and also the position of the mouse.
	void dataPositionChanged(AMGenericScanEditor *, const QPoint &);
	/// Notifier that a scan has been added for visualization to the editor.  Passes a reference to the editor for when there are many editors open at a time.
	void scanAdded(AMGenericScanEditor *, AMScan *);

public slots:

	/// Call this to export the currently-visible plot to a graphics file. (Currently, the only supported format is a vector PDF.) This routine will prompt the user to choose a file name for the plot, and confirm on overwrite.
	void exportGraphicsToFile();
	/// Call this to print the currently-visible plot.
	void printGraphics();
	/// Call this to update the Scan Conditions Table
	void refreshScanConditions();

	/// Call this to update the window title when a scan is added or removed
	void refreshWindowTitle();

	/// This helper function refreshes the editor widgets with the values from the current scan
	void refreshScanInfo();


protected slots:
	///  This catches changes in the scan that is currently selected, and hooks it up to the editor widgets. \todo Ultimately, we might handle more than one scan being "selected" at once.
	void onCurrentChanged ( const QModelIndex & selected, const QModelIndex & deselected );

	/// internal signal to forward the textChanged() signal from ui_.notesEdit
	void onNotesTextChanged() {
		emit notesChanged(notesEdit_->toPlainText());
	}

	/// called when the close buttons in the list of scans are clicked
	void onScanModelCloseClicked(const QModelIndex& index);

	/// Called when the 'Open...' button is clicked
	void onOpenScanButtonClicked();

	/// Called when the 'Save' scan button is clicked
	void onSaveScanButtonClicked();

	/// Called when the 'Close' scan button is clicked
	void onCloseScanButtonClicked();

	/// Called when the open scan dialog is accepted with one or more new scans to open.
	void onChooseScanDialogAccepted();
	/// Called when the open scan dialog is rejected.
	void onChooseScanDialogRejected();


	/// Call this function to open a set of scans from the database. The scan information is contained inside a list of "amd://..." URLs.  For more information on the format, see dropEvent().   Returns true if the list contains at least one valid scan that was added.
	/*! This function is used as an internal helper function by dropEvent(); Normally you should use the dropScanURLs function in AMDatamanAppController() since it can check for scans being open in other editors*/
	bool dropScanURLs(const QList<QUrl>& urls);

	/// Called on a one-second timer: Right now, we only use this to update the duration display for currentlyAcquiring() scans
	void onOneSecondTimer();

	/// Helper slot that emits the dataPositionChanged signal.
	void onDataPositionChanged(const QPoint &pos) { emit dataPositionChanged(this, pos); }

	/// Slot which is called when the scan is saved to the database (sets ScanID to database value)
	void onScanSavedToDatabase();

	void onScanDetailsChanged();

protected:

	// Re-implemented functions
	//////////////////////////
	/// Implement acquiring/modified checks before allowing this window to close.
	void closeEvent(QCloseEvent *);

	// Variables
	///////////////////////////

	/// This is a model containing the current open scans
	AMScanSetModel* scanSetModel_;
	/// This is the currently-selected scan, or 0 non-existent
	AMScan* currentScan_;

	/// This helper function refreshes the editor widgets with the values from a given scan
	void updateEditor(AMScan* scan);

	/// Overloaded to enable drag-dropping scans (when Drag Action = Qt::CopyAction and mime-type = "text/uri-list" with the proper format.)
	void dragEnterEvent(QDragEnterEvent *event);

	/// Overloaded to enable drag-dropping scans.
	/*! The Drag is accepted when:
	  - Drag Action = Qt::CopyAction
	  - One of the MIME types is "text/uri-list"... format is "amd://databaseConnectionName/tableName/id"
	  - There is at least one URL in the uri-list
	  - The URL scheme is "amd://"
	  - The database connection name returns a valid database, according to AMDatabase::dbByName(connectionName)
	  - The table is the main Objects table
	  - The id of the item can be found in the table
	  */
	void dropEvent(QDropEvent * event);


	/// Helper function to ask if a scan should be aborted when trying to close it. Returns true if the scan should be aborted.
	bool shouldStopAcquiringScan(AMScan* scan);
	/// Helper function to ask if a scan should be saved when trying to close it. Returns an integer corresponding to QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel.
	int shouldSaveModifiedScan(AMScan* scan);

	// Widgets
	///////////

	QVBoxLayout *verticalLayout3_;
	QFrame *scanTitleFrame_;
	QHBoxLayout *horizontalLayout3_;
	QLabel *scanTitleIcon_;
	QLabel *scanTitle_;
	QSpacerItem *scanTitleHorizontalSpacer_;
	QFrame *statusFrame_;
	QHBoxLayout *horizontalLayout4_;
	QSpacerItem *statusFrameHorizontalSpacer_;
	QLabel *statusTextLabel_;
	QHBoxLayout *horizontalLayout_;
	QVBoxLayout *leftVerticalLayout_;
	QHBoxLayout *leftHorizontalLayout_;
	QVBoxLayout *openScansLayout_;
	QToolButton *openScansTitleButton_;
	QListView *scanListView_;
	QHBoxLayout *scanButtonLayout_;
	QPushButton *openScanButton_;
	QPushButton *saveScanButton_;
	QPushButton *closeScanButton_;
	QVBoxLayout *scanInformationLayout_;
	QToolButton *scanInformationTitleButton_;
	QFrame *scanInformationFrame_;
	QVBoxLayout *scanInfoLayout;
	QGridLayout *scanInformationGridLayout_;
	QLabel *scanDate_;
	QLabel *scanTimeLabel_;
	QLabel *scanDurationLabel_;
	QLabel *scanRunLabel_;
	QLabel *scanDateLabel_;
	QLabel *scanDuration_;
	QLabel *scanNameLabel_;
	QLabel *scanNumberLabel_;
	QSpinBox *scanNumber_;
	QLabel *scanIdLabel_;
	QLineEdit *scanId_;
	QLabel *scanTime_;
	QLabel *scanEnd_;
	QLabel *scanEndLabel_;
	QSpacerItem *scanInformationSpacer_;
	QVBoxLayout *notesLayout_;
	QToolButton *notesTitleButton_;
	QPlainTextEdit *notesEdit_;
	QSpacerItem *leftVerticaFrameSpacer_;
	QVBoxLayout *rightVerticalLayout_;
	/// stack-widget holder for right-column editors
	AMVerticalStackWidget* stackWidget_;

	/// Run selector
	AMRunSelector* runSelector_;

	/// Data sources editor:
	AMDataSourcesEditor* dataSourcesEditor_;

	/// View of the scans' scanInitialConditions()
	AMControlInfoListTableView* conditionsTableView_;

	/// Plot view capable of holding multiple scans.
	AMScanView* scanView_;
	/// Plot view used for specifically viewing 2D scans.
	AM2DScanView *scanView2D_;

	/// Sample editor
	AMSamplePre2013Editor* sampleEditor_;
	/// Alternative sample brief view for current AMSample class
	AMSampleBriefView *sampleBriefView_;

	/// Dialog to choose an existing scan to open/add.  Will be 0 until it is required/created.
	AMChooseScanDialog* chooseScanDialog_;
	/// Customized LineEdit which allows for validation of scan names
	AMRegExpLineEdit* scanNameEdit_;

	/// The item delegate for the scan list view
	AMDetailedItemDelegate* del_;

	/// Update timer for when scans are running
	QTimer* oneSecondTimer_;


private:
	void setupUi();

};

Q_DECLARE_METATYPE(AMGenericScanEditor*)

#endif // AMGENERICSCANEDITOR_H
