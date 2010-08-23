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

	/// Add a scan to an editor. The editor takes ownership of the scan, and will delete it when closed or the scan is removed.
	void addScan(AMScan* newScan) { /// \todo
	}


signals:

public slots:

protected:
	/// This is a model containing the current open scans
	AMScanSetModel* scanSetModel_;

	// UI Components

	/// UI object container
	Ui::AMGenericScanEditor ui_;

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

};

#endif // AMGENERICSCANEDITOR_H
