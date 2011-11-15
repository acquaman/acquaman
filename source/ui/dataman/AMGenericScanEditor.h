/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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
#include <QMetaType>
#include "ui_AMGenericScanEditor.h"

#include "dataman/AMScanSetModel.h"

class AMScan;

class AMScanView;
class AMVerticalStackWidget;
class AMRunSelector;
class AMSampleEditor;
class AMDataSourcesEditor;
class AMChooseScanDialog;

class AMGenericScanEditor : public QWidget
{
Q_OBJECT
public:
	/// Construct an empty editor:
	explicit AMGenericScanEditor(QWidget *parent = 0);

	/// Deletes self and all scan objects that were added
	virtual ~AMGenericScanEditor();

	/// Add a scan to an editor. The editor takes ownership of the scan, and will delete it when closed or the scan is removed.
	void addScan(AMScan* newScan);


	/// Remove a scan from the editor and delete the scan.
	void deleteScan(AMScan* scan);

	/// Remove a scan and delete it, but ask the user for confirmation if it's been modified.  Returns true if the scan was deleted, and false if the user 'cancelled' the process.
	bool deleteScanWithModifiedCheck(AMScan* scan);

	/// Remove a scan from the editor, but don't delete the scan. Ownership becomes the responsibility of the caller.
	void removeScan(AMScan* scan) {
		scanSetModel_->removeScan(scan);
		refreshWindowTitle();
	}

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




	/// Call this function to open a set of scans from the database. The scan information is contained inside a list of "amd://..." URLs.  For more information on the format, see dropEvent().   Returns true if the list contains at least one valid scan that was added.
	/*! This function is used as a helper function by dropEvent(), but you can also call it directly. */
	bool dropScanURLs(const QList<QUrl>& urls);

	/// Call this function to find out if this editor can be closed. Checks for scans in progress and prompts the user for what to do with modified scans.  Returns true if the editor can be closed; returns false if any scans are acquiring or if the user responded "cancel" to a save-request.
	bool canCloseEditor();

signals:
	/// Internal signal to forward the textChanged() from ui_.notesEdit
	void notesChanged(const QString&);

public slots:

protected slots:
	///  This catches changes in the scan that is currently selected, and hooks it up to the editor widgets. \todo Ultimately, we might handle more than one scan being "selected" at once.
	void onCurrentChanged ( const QModelIndex & selected, const QModelIndex & deselected );

	/// internal signal to forward the textChanged() signal from ui_.notesEdit
	void onNotesTextChanged() {
		emit notesChanged(ui_.notesEdit->toPlainText());
	}

	/// called when the close buttons in the list of scans are clicked
	void onScanModelCloseClicked(const QModelIndex& index);

	/// Called when the 'Open...' button is clicked
	void onOpenScanButtonClicked();

	/// Called when the 'Save' scan button is clicked
	void onSaveScanButtonClicked();

	/// Called when the 'Close' scan button is clicked
	void onCloseScanButtonClicked();

	/// Call this to update the window title when a scan is added or removed
	void refreshWindowTitle();

	/// Called when the open scan dialog is accepted with one or more new scans to open.
	void onChooseScanDialogAccepted();


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

	// UI Components

	/// UI object container
	Ui::AMGenericScanEditor ui_;

	/// stack-widget holder for right-column editors
	AMVerticalStackWidget* stackWidget_;

	/// Run selector
	AMRunSelector* runSelector_;

	/// Data sources editor:
	AMDataSourcesEditor* dataSourcesEditor_;

	/// Plot view capable of holding multiple scans.
	AMScanView* scanView_;

	/// Sample editor
	AMSampleEditor* sampleEditor_;

	/// Dialog to choose an existing scan to open/add.  Will be 0 until it is required/created.
	AMChooseScanDialog* chooseScanDialog_;

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

	/// This helper function refreshes the editor widgets with the values from a given scan
	void updateEditor(AMScan* scan);

	/// Helper function to ask if a scan should be aborted when trying to close it. Returns true if the scan should be aborted.
	bool shouldStopAcquiringScan(AMScan* scan);
	/// Helper function to ask if a scan should be saved when trying to close it. Returns an integer corresponding to QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel.
	int shouldSaveModifiedScan(AMScan* scan);



};

Q_DECLARE_METATYPE(AMGenericScanEditor*)

#endif // AMGENERICSCANEDITOR_H
