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


#include "AMGenericScanEditor.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QList>
#include <QUrl>

#include "acquaman.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMScan.h"

#include "ui/AMDetailedItemDelegate.h"
#include "ui/dataman/AMScanView.h"
#include "ui/AMVerticalStackWidget.h"
#include "ui/dataman/AMRunSelector.h"
#include "ui/dataman/AMSampleEditor.h"
#include "ui/dataman/AMDataSourcesEditor.h"
#include "ui/dataman/AMChooseScanDialog.h"

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
	QWidget* sampleEditorHolder = new QWidget();	// just used to add a margin around the sample editor itself, which has no margins.
	sampleEditorHolder->setLayout(new QVBoxLayout);
	sampleEditor_ = new AMSampleEditor(AMDatabase::userdb());
	sampleEditorHolder->layout()->addWidget(sampleEditor_);
	stackWidget_->addItem("Sample Information", sampleEditorHolder);

	dataSourcesEditor_ = new AMDataSourcesEditor(scanSetModel_);
	stackWidget_->addItem("Data Sets", dataSourcesEditor_);

	QWidget* temp3 = new QWidget();
	temp3->setMinimumHeight(200);
	stackWidget_->addItem("Beamline Information", temp3);
	stackWidget_->collapseItem(2);







	connect(ui_.notesEdit, SIGNAL(textChanged()), this, SLOT(onNotesTextChanged()));

	// watch for changes in the selected/deselected scans:
	connect(ui_.scanListView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentChanged(QModelIndex,QModelIndex)));

	/// \todo make this whole window accept drops of scans, with the 'amd://database/table/id' uri-list mime type
	setAcceptDrops(true);

	currentScan_ = 0;


	/// disable drops on text fields that we don't want to accept drops
	ui_.scanName->setAcceptDrops(false);
	ui_.notesEdit->setAcceptDrops(false);


	// Connect open,save,close buttons
	connect(ui_.openScanButton, SIGNAL(clicked()), this, SLOT(onOpenScanButtonClicked()));
	connect(ui_.closeScanButton, SIGNAL(clicked()), this, SLOT(onCloseScanButtonClicked()));
	connect(ui_.saveScanButton, SIGNAL(clicked()), this, SLOT(onSaveScanButtonClicked()));

	// close button and save buttons are initially disabled; there's no scan to act on
	ui_.closeScanButton->setEnabled(false);
	ui_.saveScanButton->setEnabled(false);

	chooseScanDialog_ = 0;

}


AMGenericScanEditor::~AMGenericScanEditor() {
	while(scanSetModel_->scanCount()) {
		AMScan* s = scanSetModel_->scanAt(scanSetModel_->scanCount()-1);
		scanSetModel_->removeScan(s);
		delete s;
	}
}


void AMGenericScanEditor::addScan(AMScan* newScan) {
	scanSetModel_->addScan(newScan);
	ui_.scanListView->setCurrentIndex(scanSetModel_->indexForScan(scanSetModel_->indexOf(newScan)));
	if(scanSetModel_->exclusiveDataSourceName().isEmpty() && newScan->dataSourceCount() > 0)
		scanSetModel_->setExclusiveDataSourceByName(newScan->dataSourceAt(0)->name());

	refreshWindowTitle();
}

void AMGenericScanEditor::refreshWindowTitle() {

	int numScans = scanSetModel_->scanCount();

	if(numScans == 0) {
		setWindowTitle("Scan Editor");
		return;
	}

	QStringList scanNames;
	for(int i=0; i<numScans; i++)
		scanNames << scanSetModel_->scanAt(i)->fullName();

	QString windowTitle = scanNames.join(", ");
	if(numScans > 1) {
		windowTitle.prepend(QString("%1 scans: (").arg(numScans));
		windowTitle.append(")");
	}
	setWindowTitle(windowTitle);
}

void AMGenericScanEditor::onCurrentChanged ( const QModelIndex & selected, const QModelIndex & deselected ) {

	Q_UNUSED(deselected)

	AMScan* newScan = 0;
	if(selected.isValid())
		newScan = scanSetModel_->scanAt(selected.row());

	// disconnect the old scan:
	if(currentScan_) {
		// removed: disconnect(currentScan_, SIGNAL(metaDataChanged()), this, SLOT(onScanMetaDataChanged()));

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
	dataSourcesEditor_->setCurrentScan(currentScan_);


	if(currentScan_) {
		// removed: connect(currentScan_, SIGNAL(metaDataChanged()), this, SLOT(onScanMetaDataChanged()));

		// make connections to widgets, so that widgets edit this scan:
		connect(ui_.scanName, SIGNAL(textChanged(QString)), currentScan_, SLOT(setName(QString)));
		connect(ui_.scanNumber, SIGNAL(valueChanged(int)), currentScan_, SLOT(setNumber(int)));
		connect(this, SIGNAL(notesChanged(QString)), currentScan_, SLOT(setNotes(QString)));
		connect(runSelector_, SIGNAL(currentRunIdChanged(int)), currentScan_, SLOT(setRunId(int)));
		connect(sampleEditor_, SIGNAL(currentSampleChanged(int)), currentScan_, SLOT(setSampleId(int)));

		/// \todo When migrating to multiple scan selection, this will need to be changed:
		ui_.saveScanButton->setEnabled(true);
		ui_.closeScanButton->setEnabled(true);
	}
	else {
		/// \todo When migrating to multiple scan selection, this will need to be changed:
		ui_.saveScanButton->setEnabled(false);
		ui_.closeScanButton->setEnabled(false);
	}


	if(scanSetModel_->scanCount() == 0)
		scanSetModel_->setExclusiveDataSourceByName(QString());

}

/* removed:
void AMGenericScanEditor::onScanMetaDataChanged() {

	/// hmmm... should we change the editor values? What if the scan is altered elsewhere...
	// what if they changed it first? But haven't saved yet?
}*/

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
bool AMGenericScanEditor::deleteScanWithModifiedCheck(AMScan* scan) {
	// easy way first:
	if(!scan->modified()) {
		deleteScan(scan);
		return true;
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
		return true;
	}

	else if(result == QMessageBox::Save) {
		bool saveSuccess;
		if(scan->database())
			saveSuccess = scan->storeToDb(scan->database());
		else
			saveSuccess = scan->storeToDb(AMDatabase::userdb());

		if(saveSuccess) {
			deleteScan(scan);
			return true;
		}
	}

	return false;
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

	if(	!( event->possibleActions() & Qt::CopyAction
		&& event->mimeData()->hasUrls()) )
		return;


	if(dropScanURLs(event->mimeData()->urls()))
		event->acceptProposedAction();
}

bool AMGenericScanEditor::dropScanURLs(const QList<QUrl>& urls) {

	if(	!urls.count() )
		return false;

	bool accepted = false;

	foreach(QUrl url, urls) {
		// scheme correct?
		if(url.scheme() != "amd")
			break;

		// Can we connect to the database?
		AMDatabase* db = AMDatabase::dbByName(url.host());
		if(!db)
			break;
		/// \bug This does not verify that the incoming scans came from the user database. In fact, it happily accepts scans from other databases. Check if we assume anywhere else inside AMGenericScanEditor that we're using the AMDatabase::userdb() database. (If we do, this could cause problems.)

		QStringList path = url.path().split('/', QString::SkipEmptyParts);
		if(path.count() != 2)
			break;

		QString tableName = path.at(0);
		bool idOkay;
		int id = path.at(1).toInt(&idOkay);
		if(!idOkay || id < 1)
			break;

		/// \todo Evaluate if this is still necessary: Only store things that belong in the scans table for now.
		if(tableName != AMDbObjectSupport::tableNameForClass<AMScan>())
			break;

		/// Dynamically create and load a detailed subclass of AMDbObject from the database... whatever type it is.
		AMDbObject* dbo = AMDbObjectSupport::createAndLoadObjectAt(db, tableName, id);
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

	return accepted;
}

void AMGenericScanEditor::onCloseScanButtonClicked() {
	// this function is ready for when multiple scan selection is enabled
	QModelIndexList scanIndexes = ui_.scanListView->selectionModel()->selectedIndexes();
	foreach(QModelIndex i, scanIndexes) {
		deleteScanWithModifiedCheck(scanSetModel_->scanAt(i.row()));
	}
}

void AMGenericScanEditor::onSaveScanButtonClicked() {
	// this function is ready for when multiple scan selection is enabled
	QModelIndexList scanIndexes = ui_.scanListView->selectionModel()->selectedIndexes();
	foreach(QModelIndex i, scanIndexes) {
		AMScan* scan = scanSetModel_->scanAt(i.row());
		if(scan->database())
			scan->storeToDb(scan->database());
		else
			scan->storeToDb(AMDatabase::userdb());
	}
}

void AMGenericScanEditor::onOpenScanButtonClicked() {
	if(!chooseScanDialog_) {
		chooseScanDialog_ = new AMChooseScanDialog(AMDatabase::userdb(), "Add an existing scan", "Choose one or more existing scans to open in this editor.", true, this);
		connect(chooseScanDialog_, SIGNAL(accepted()), this, SLOT(onChooseScanDialogAccepted()));
	}

	chooseScanDialog_->show();
}

void AMGenericScanEditor::deleteScan(AMScan *scan) {
	scanSetModel_->removeScan(scan);
	delete scan;
	refreshWindowTitle();
}

void AMGenericScanEditor::onChooseScanDialogAccepted()
{
	dropScanURLs(chooseScanDialog_->getSelectedScans());
}
