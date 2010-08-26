#ifndef AMGENERICSCANEDITOR_H
#define AMGENERICSCANEDITOR_H

#include <QWidget>
#include "ui_AMGenericScanEditor.h"

#include "acquaman.h"
#include "dataman/AMScan.h"
#include "dataman/AMScanSetModel.h"
#include "ui/AMScanView.h"

class AMGenericScanEditor : public QWidget
{
Q_OBJECT
public:
	/// Construct an empty editor:
	explicit AMGenericScanEditor(QWidget *parent = 0);

	/// Deletes self and all scan objects that were added
	virtual ~AMGenericScanEditor();

	/// Add a scan to an editor. The editor takes ownership of the scan, and will delete it when closed or the scan is removed.
	void addScan(AMScan* newScan) { /// \todo
		scanSetModel_->addScan(newScan);
		ui_.scanListView->setCurrentIndex(scanSetModel_->indexForScan(newScan));
	}





signals:
	/// Internal signal to forward the textChanged() from ui_.notesEdit
	void notesChanged(const QString&);

public slots:

protected slots:
	/// Ultimately, we might handle more than one scan being "selected" at once. This catches changes in the scans that are currently selected, and hooks them up to the editor widgets
	void onCurrentChanged ( const QModelIndex & selected, const QModelIndex & deselected );

	/// This slot responds to meta-data changes in the current scan
	void onScanMetaDataChanged(const QString& key);

	/// internal signal to forward the textChanged() signal from ui_.notesEdit
	void onNotesTextChanged() {
		emit notesChanged(ui_.notesEdit->toPlainText());
	}


protected:
	/// This is a model containing the current open scans
	AMScanSetModel* scanSetModel_;
	/// This is the currently-selected scan, or 0 non-existent
	AMScan* currentScan_;

	// UI Components

	/// UI object container
	Ui::AMGenericScanEditor ui_;

	/// Run selector


	/// Plot view capable of holding multiple scans.
	AMScanView* scanView_;

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

};

#endif // AMGENERICSCANEDITOR_H
