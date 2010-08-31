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

	// Add extra UI components:
	stackWidget_ = new AMVerticalStackWidget();
	stackWidget_->setMinimumWidth(200);
	stackWidget_->setMaximumWidth(360);
	ui_.rightVerticalLayout->addWidget(stackWidget_);

	// Add scan view (plots)
	scanView_ = new AMScanView();
	scanView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui_.leftVerticalLayout->insertWidget(0, scanView_, 2);

	// share the scan set model with the AMScanView
	scanSetModel_ = scanView_->model();

	// And set this model on the list view of scans:
	ui_.scanListView->setModel(scanSetModel_);
	ui_.scanListView->setSelectionMode(QAbstractItemView::SingleSelection);

	AMDetailedItemDelegate* del = new AMDetailedItemDelegate(this);
	del->setCloseButtonsEnabled(true);
	connect(del, SIGNAL(closeButtonClicked(QModelIndex)), this, SLOT(onScanModelCloseClicked(QModelIndex)));
	ui_.scanListView->setItemDelegate(del);
	ui_.scanListView->setAlternatingRowColors(true);
	ui_.scanListView->setAttribute( Qt::WA_MacShowFocusRect, false);

	// Add run selector:
	runSelector_ = new AMRunSelector(AMDatabase::userdb());
	ui_.scanInfoLayout->insertWidget(1, runSelector_);


	// Add detailed editor widgets:
	sampleEditor_ = new AMSampleEditor(AMDatabase::userdb());
	stackWidget_->addItem(sampleEditor_, "Sample Information");

	channelEditor_ = new AMChannelEditor(scanSetModel_);
	stackWidget_->addItem(channelEditor_, "Plot Data");

	QWidget* temp3 = new QWidget();
	temp3->setMinimumHeight(200);
	stackWidget_->addItem(temp3, "Beamline Information");







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

	AMScan* newScan = 0;
	if(selected.isValid())
		newScan = scanSetModel_->scanAt(selected.row());

	// disconnect the old scan:
	if(currentScan_) {
		disconnect(currentScan_, SIGNAL(metaDataChanged(QString)), this, SLOT(onScanMetaDataChanged(QString)));

		disconnect(ui_.scanName, 0, currentScan_, 0);
		disconnect(ui_.scanNumber, 0, currentScan_, 0);
		disconnect(this, SIGNAL(notesChanged(QString)), currentScan_, SLOT(setNotes(QString)));
		disconnect(runSelector_, SIGNAL(currentRunIdChanged(int)), currentScan_, SLOT(setRunId(int)));
		disconnect(sampleEditor_, SIGNAL(currentSampleChanged(int)), currentScan_, SLOT(setSampleId(int)));
	}

	// it becomes now the new scan:
	currentScan_ = newScan;


	// update all widgets to match
	updateEditor(currentScan_);
	channelEditor_->setCurrentScan(currentScan_);


	if(currentScan_) {
		connect(currentScan_, SIGNAL(metaDataChanged(QString)), this, SLOT(onScanMetaDataChanged(QString)));

		// make connections to widgets, so that widgets edit this scan:
		connect(ui_.scanName, SIGNAL(textChanged(QString)), currentScan_, SLOT(setName(QString)));
		connect(ui_.scanNumber, SIGNAL(valueChanged(int)), currentScan_, SLOT(setNumber(int)));
		connect(this, SIGNAL(notesChanged(QString)), currentScan_, SLOT(setNotes(QString)));
		connect(runSelector_, SIGNAL(currentRunIdChanged(int)), currentScan_, SLOT(setRunId(int)));
		connect(sampleEditor_, SIGNAL(currentSampleChanged(int)), currentScan_, SLOT(setSampleId(int)));
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
	ui_.scanDate->setText( AMDateTimeUtils::prettyDate(scan->dateTime()));
	ui_.scanTime->setText( scan->dateTime().time().toString("h:mmap") );
	ui_.notesEdit->setPlainText( scan->notes() );
	runSelector_->setCurrentRunId(scan->runId());
	sampleEditor_->setCurrentSample(scan->sampleId());

	ui_.topFrameTitle->setText(QString("Editing %1 #%2").arg(scan->name()).arg(scan->number()));
	}

	else {
		ui_.scanName->setText( QString() );
		ui_.scanNumber->setValue(0);
		ui_.scanDate->setText( QString() );
		ui_.scanTime->setText( QString() );
		ui_.notesEdit->clear();

		// what to set run selector to?

		sampleEditor_->setCurrentSample(-1);

		ui_.topFrameTitle->setText("Scan Editor");
	}
}


/// called when the close buttons in the list of scans are clicked
void AMGenericScanEditor::onScanModelCloseClicked(const QModelIndex& index) {
	/// Just a quick check to make sure that this is a valid scan index (ie: parent is a top-level index; index.row() is the scan index)
	if(index.parent() == QModelIndex() && index.isValid())
		deleteScanWithModifiedCheck(scanSetModel_->scanAt(index.row()));

}

#include <QMessageBox>
/// Remove a scan and delete it, but ask the user for confirmation if it's been modified.
void AMGenericScanEditor::deleteScanWithModifiedCheck(AMScan* scan) {
	// easy way first:
	if(!scan->modified()) {
		deleteScan(scan);
		return;
	}

	QMessageBox enquiry(this);
	enquiry.setText(QString("Do you want to save the changes you made to the scan '%1' (#%2)?").arg(scan->name()).arg(scan->number()));
	enquiry.setInformativeText("Your changes will be lost if you don't save them.");
	enquiry.setIcon(QMessageBox::Question);
	enquiry.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	enquiry.setDefaultButton(QMessageBox::Save);
	enquiry.setEscapeButton(QMessageBox::Cancel);

	int result = enquiry.exec();

	if(result == QMessageBox::Discard) {
		deleteScan(scan);
	}

	else if(result == QMessageBox::Save) {
		bool saveSuccess;
		if(scan->database())
			saveSuccess = scan->storeToDb(scan->database());
		else
			saveSuccess = scan->storeToDb(AMDatabase::userdb());

		if(saveSuccess)
			deleteScan(scan);
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

