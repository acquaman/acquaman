#include "AMGenericScanEditor.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QList>
#include <QUrl>

#include "dataman/AMDbLoader.h"
#include "dataman/AMDatabaseDefinition.h"

AMGenericScanEditor::AMGenericScanEditor(QWidget *parent) :
		QWidget(parent)
{
	ui_.setupUi(this);

	// Add scan view (plots)
	scanView_ = new AMScanView();
	ui_.leftVerticalLayout->insertWidget(0, scanView_);

	// share the scan set model with the AMScanView
	scanSetModel_ = scanView_->model();

	// And set this model on the list view of scans:
	ui_.scanListView->setModel(scanSetModel_);
	ui_.scanListView->setSelectionMode(QAbstractItemView::SingleSelection);

	/// \todo make this whole window accept drops of scans, with the 'amd://database/table/id' uri-list mime type
	setAcceptDrops(true);
}




/// Overloaded to enable drag-dropping scans (when Drag Action = Qt::CopyAction and mime-type = "text/uri-list" with the proper format.)
void AMGenericScanEditor::dragEnterEvent(QDragEnterEvent *event) {
	if(	event->possibleActions() & Qt::CopyAction
		&& event->mimeData()->hasUrls()
		&& event->mimeData()->urls().count() > 0
		&& event->mimeData()->urls().at(0).scheme() == "amd"
		) {
		event->accept();
		event->acceptProposedAction();
	}
}




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
void AMGenericScanEditor::dropEvent(QDropEvent * event) {

	if(	!(event->possibleActions() & Qt::CopyAction
		&& event->mimeData()->hasUrls()
		&& event->mimeData()->urls().count() > 0
		&& event->mimeData()->urls().at(0).scheme() == "amd")
		)
		return;


	bool accepted = false;

	foreach(QUrl url, event->mimeData()->urls()) {
		// Can we connect to the database?
		AMDatabase* db = AMDatabase::dbByName(url.host());
		if(!db)
			break;

		QStringList path = url.path().split('/');
		if(path.count() != 3)
			break;

		QString tableName = path.at(1);
		bool idOkay;
		int id = path.at(2).toInt(&idOkay);
		if(!idOkay || id < 1)
			break;

		// Only store things that belong in the objects table for now.
		if(tableName != AMDatabaseDefinition::objectTableName())
			break;

		/// Dynamically create and load a detailed subclass of AMDbObject from the database... whatever type it is.
		AMDbObject* dbo = AMDbLoader::createAndLoad(db, id);
		if(!dbo)
			break;

		AMScan* scan = qobject_cast<AMScan*>( dbo );
		if(!scan) {
			delete dbo;
			break;
		}

		// success!
		addScan(scan);
		accepted = true;
	}


	if(accepted)
		event->acceptProposedAction();
}
