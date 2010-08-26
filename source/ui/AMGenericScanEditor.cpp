#include "AMGenericScanEditor.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QList>
#include <QUrl>


#include "dataman/AMDbLoader.h"
#include "dataman/AMDatabaseDefinition.h"

#include "ui/AMDetailedItemDelegate.h"

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

	ui_.scanListView->setItemDelegate(new AMDetailedItemDelegate(this));
	ui_.scanListView->setAlternatingRowColors(true);
	ui_.scanListView->setAttribute( Qt::WA_MacShowFocusRect, false);

	connect(ui_.notesEdit, SIGNAL(textChanged()), this, SLOT(onNotesTextChanged()));

	// watch for changes in the selected/deselected scans:
	connect(ui_.scanListView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentChanged(QModelIndex,QModelIndex)));

	/// \todo make this whole window accept drops of scans, with the 'amd://database/table/id' uri-list mime type
	setAcceptDrops(true);

	currentScan_ = 0;


	/// disable drops on text fields that we don't want to accept drops
	ui_.scanName->setAcceptDrops(false);
	ui_.notesEdit->setAcceptDrops(false);

}


AMGenericScanEditor::~AMGenericScanEditor() {
	while(scanSetModel_->numScans()) {
		AMScan* s = scanSetModel_->scanAt(scanSetModel_->numScans()-1);
		scanSetModel_->removeScan(s);
		delete s;
	}
}


void AMGenericScanEditor::onCurrentChanged ( const QModelIndex & selected, const QModelIndex & deselected ) {

	Q_UNUSED(deselected)

	AMScan* newScan = scanSetModel_->scanAt(selected.row());

	// disconnect the old scan:
	if(currentScan_) {
		disconnect(currentScan_, SIGNAL(metaDataChanged(QString)), this, SLOT(onScanMetaDataChanged(QString)));

		disconnect(ui_.scanName, 0, currentScan_, 0);
		disconnect(ui_.scanNumber, 0, currentScan_, 0);
		// disconnect(ui_.scanRun, 0, currentScan_, 0);
		disconnect(this, SIGNAL(notesChanged(QString)), currentScan_, SLOT(setNotes(QString)));
	}

	// it becomes now the new scan:
	currentScan_ = newScan;


	// update all widgets to match
	updateEditor(currentScan_);

	if(currentScan_) {
		connect(currentScan_, SIGNAL(metaDataChanged(QString)), this, SLOT(onScanMetaDataChanged(QString)));

		// make connections to widgets, so that widgets edit this scan:
		connect(ui_.scanName, SIGNAL(textChanged(QString)), currentScan_, SLOT(setName(QString)));
		connect(ui_.scanNumber, SIGNAL(valueChanged(int)), currentScan_, SLOT(setNumber(int)));
		connect(this, SIGNAL(notesChanged(QString)), currentScan_, SLOT(setNotes(QString)));
	}

}

void AMGenericScanEditor::onScanMetaDataChanged(const QString &key) {

	/// hmmm... should we change the editor values? What if the scan is altered elsewhere...
	// what if they changed it first? But haven't saved yet?
}

void AMGenericScanEditor::updateEditor(AMScan *scan) {
	if(scan) {

	ui_.scanName->setText(scan->name());
	ui_.scanNumber->setValue(scan->number());
	ui_.scanDate->setText( scan->dateTime().date().toString("MMM d (yyyy)") );
	ui_.scanTime->setText( scan->dateTime().time().toString("h:mmap") );
	ui_.notesEdit->setPlainText( scan->notes() );
	/// \todo change to run box selector
	// ui_.scanRun->setText();

	ui_.topFrameTitle->setText(QString("Editing %1 #%2").arg(scan->name()).arg(scan->number()));
	}

	else {
		ui_.scanName->setText( QString() );
		ui_.scanNumber->setValue(0);
		ui_.scanDate->setText( QString() );
		ui_.scanTime->setText( QString() );
		ui_.notesEdit->clear();
		/// \todo change to run box selector
		// ui_.scanRun->setText();

		ui_.topFrameTitle->setText("Scan Editor");
	}
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

