/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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
#include <QTimer>
#include <QStringBuilder>

#include "acquaman.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMScan.h"
#include "dataman/AMSample.h"

#include "ui/AMDetailedItemDelegate.h"
#include "ui/dataman/AMScanView.h"
#include "ui/AMVerticalStackWidget.h"
#include "ui/dataman/AMRunSelector.h"
#include "ui/dataman/AMSamplePre2013Editor.h"
#include "ui/dataman/AMSampleBriefView.h"
#include "ui/dataman/AMDataSourcesEditor.h"
#include "ui/dataman/AMChooseScanDialog.h"
#include "ui/dataman/AMControlInfoListTableView.h"
#include "ui/dataman/AM2DScanView.h"
#include "ui/dataman/AMSampleEditor.h"

#include "util/AMFontSizes.h"

#ifndef ACQUAMAN_NO_ACQUISITION
// needed to stop scans in progress, if requested from here during close events.
#include "acquaman/AMScanController.h"
#endif

AMGenericScanEditor::AMGenericScanEditor(QWidget *parent) :
	QWidget(parent)
{
	ui_.setupUi(this);
	ui_.topFrameTitle->setStyleSheet("font: " AM_FONT_LARGE_ "pt \"Lucida Grande\";\ncolor: rgb(79, 79, 79);");
	ui_.statusTextLabel->setStyleSheet("color: white;\nfont: bold " AM_FONT_SMALL_ "pt \"Lucida Grande\"");
	setWindowTitle("Scan Editor");

	// Add extra UI components:
	stackWidget_ = new AMVerticalStackWidget();
	stackWidget_->setMinimumWidth(200);
	stackWidget_->setMaximumWidth(360);
	ui_.rightVerticalLayout->addWidget(stackWidget_);

	// Add scan view (plots)
	scanView_ = new AMScanView();
	scanView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui_.leftVerticalLayout->insertWidget(0, scanView_, 2);

	// Ensure that the 2D scan view is pointing to nowhere.
	scanView2D_ = 0;

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
	runSelector_ = new AMRunSelector(AMDatabase::database("user"));
	ui_.scanInfoLayout->insertWidget(1, runSelector_);


	// Add detailed editor widgets:
	QWidget* sampleEditorHolder = new QWidget();	// just used to add a margin around the sample editor itself, which has no margins.
	sampleEditorHolder->setLayout(new QVBoxLayout);
	sampleEditor_ = new AMSamplePre2013Editor(AMDatabase::database("user"));
	sampleEditorHolder->layout()->addWidget(sampleEditor_);
	sampleEditor_->hide();
	//sampleEditorHolder->layout()->addWidget(new AMSampleEditor(AMDatabase::database("user")));
	sampleBriefView_ = new AMSampleBriefView();
	sampleEditorHolder->layout()->addWidget(sampleBriefView_);
	sampleBriefView_->hide();
	stackWidget_->addItem("Sample Information", sampleEditorHolder);



	dataSourcesEditor_ = new AMDataSourcesEditor(scanSetModel_);
	stackWidget_->addItem("Data Sets", dataSourcesEditor_);

	conditionsTableView_ = new AMControlInfoListTableView();
	conditionsTableView_->setMinimumHeight(200);
	stackWidget_->addItem("Beamline Information", conditionsTableView_);
	stackWidget_->collapseItem(2);

	connect(ui_.notesEdit, SIGNAL(textChanged()), this, SLOT(onNotesTextChanged()));

	// watch for changes in the selected/deselected scans:
	connect(ui_.scanListView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentChanged(QModelIndex,QModelIndex)));

	// \todo make this whole window accept drops of scans, with the 'amd://database/table/id' uri-list mime type
	setAcceptDrops(true);

	currentScan_ = 0;

	// disable drops on text fields that we don't want to accept drops
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

	QTimer* oneSecondTimer = new QTimer(this);
	connect(oneSecondTimer, SIGNAL(timeout()), this, SLOT(onOneSecondTimer()));
	oneSecondTimer->start(1000);
}

AMGenericScanEditor::AMGenericScanEditor(bool use2DScanView, QWidget *parent)
	: QWidget(parent)
{
	ui_.setupUi(this);
	ui_.topFrameTitle->setStyleSheet("font: " AM_FONT_LARGE_ "pt \"Lucida Grande\";\ncolor: rgb(79, 79, 79);");
	ui_.statusTextLabel->setStyleSheet("color: white;\nfont: bold " AM_FONT_SMALL_ "pt \"Lucida Grande\"");
	setWindowTitle("Scan Editor");

	// Add extra UI components:
	stackWidget_ = new AMVerticalStackWidget();
	stackWidget_->setMinimumWidth(200);
	stackWidget_->setMaximumWidth(360);
	ui_.rightVerticalLayout->addWidget(stackWidget_);

	// Add scan view (plots)
	if (use2DScanView){

		scanView_ = 0;
		scanView2D_ = new AM2DScanView();
		scanView2D_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		ui_.leftVerticalLayout->insertWidget(0, scanView2D_, 2);

		// share the scan set model with the AMScanView
		scanSetModel_ = scanView2D_->model();

		connect(scanView2D_, SIGNAL(dataPositionChanged(QPoint)), this, SLOT(onDataPositionChanged(QPoint)));
		ui_.openScanButton->setEnabled(false);
	}

	else {

		scanView2D_ = 0;
		scanView_ = new AMScanView();
		scanView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		ui_.leftVerticalLayout->insertWidget(0, scanView_, 2);

		// share the scan set model with the AMScanView
		scanSetModel_ = scanView_->model();
	}

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
	runSelector_ = new AMRunSelector(AMDatabase::database("user"));
	ui_.scanInfoLayout->insertWidget(1, runSelector_);


	// Add detailed editor widgets:
	QWidget* sampleEditorHolder = new QWidget();	// just used to add a margin around the sample editor itself, which has no margins.
	sampleEditorHolder->setLayout(new QVBoxLayout);
	sampleEditor_ = new AMSamplePre2013Editor(AMDatabase::database("user"));
	sampleEditorHolder->layout()->addWidget(sampleEditor_);
	sampleEditor_->hide();
	//sampleEditorHolder->layout()->addWidget(new AMSampleEditor(AMDatabase::database("user")));
	sampleBriefView_ = new AMSampleBriefView();
	sampleEditorHolder->layout()->addWidget(sampleBriefView_);
	sampleBriefView_->hide();
	stackWidget_->addItem("Sample Information", sampleEditorHolder);

	dataSourcesEditor_ = new AMDataSourcesEditor(scanSetModel_);
	stackWidget_->addItem("Data Sets", dataSourcesEditor_);

	conditionsTableView_ = new AMControlInfoListTableView();
	conditionsTableView_->setMinimumHeight(200);
	stackWidget_->addItem("Beamline Information", conditionsTableView_);
	stackWidget_->collapseItem(2);

	connect(ui_.notesEdit, SIGNAL(textChanged()), this, SLOT(onNotesTextChanged()));

	// watch for changes in the selected/deselected scans:
	connect(ui_.scanListView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentChanged(QModelIndex,QModelIndex)));

	// \todo make this whole window accept drops of scans, with the 'amd://database/table/id' uri-list mime type
	setAcceptDrops(true);

	currentScan_ = 0;


	// disable drops on text fields that we don't want to accept drops
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

	QTimer* oneSecondTimer = new QTimer(this);
	connect(oneSecondTimer, SIGNAL(timeout()), this, SLOT(onOneSecondTimer()));
	oneSecondTimer->start(1000);
}

AMGenericScanEditor::~AMGenericScanEditor() {
	while(scanSetModel_->scanCount()) {
		AMScan* s = scanSetModel_->scanAt(scanSetModel_->scanCount()-1);
		scanSetModel_->removeScan(s);
	}
}

QPointF AMGenericScanEditor::dataPosition() const
{
	if (scanView2D_)
		return scanView2D_->dataPosition();

	return QPointF();
}

QRectF AMGenericScanEditor::selectedRect() const
{
	if (scanView2D_)
		return scanView2D_->selectedRect();

	return QRectF();
}

void AMGenericScanEditor::setAxisInfoForSpectrumView(const AMAxisInfo &info, bool propogateToPlotRange)
{
	if (scanView_)
		scanView_->setAxisInfoForSpectrumView(info, propogateToPlotRange);

	else if (scanView2D_)
		scanView2D_->setAxisInfoForSpectrumView(info, propogateToPlotRange);
}

void AMGenericScanEditor::setPlotRange(double low, double high)
{
	if (scanView_)
		scanView_->setPlotRange(low, high);

	else if (scanView2D_)
		scanView2D_->setPlotRange(low, high);
}

void AMGenericScanEditor::setSingleSpectrumViewDataSourceName(const QString &name)
{
	if (scanView_)
		scanView_->setSingleSpectrumDataSource(name);

	else if (scanView2D_)
		scanView2D_->setSingleSpectrumDataSource(name);
}


void AMGenericScanEditor::addScan(AMScan* newScan) {
	scanSetModel_->addScan(newScan);

	ui_.scanListView->setCurrentIndex(scanSetModel_->indexForScan(scanSetModel_->indexOf(newScan)));

	if(scanSetModel_->exclusiveDataSourceName().isEmpty()) {
		QVector<int> nonHiddenDataSourceIndexes = newScan->nonHiddenDataSourceIndexes();
		if(!nonHiddenDataSourceIndexes.isEmpty())
			scanSetModel_->setExclusiveDataSourceByName(newScan->dataSourceAt(nonHiddenDataSourceIndexes.first())->name());
	}

	emit scanAdded(this, newScan);
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
		disconnect(ui_.scanName, 0, currentScan_, 0);
		disconnect(ui_.scanNumber, 0, currentScan_, 0);
		disconnect(this, SIGNAL(notesChanged(QString)), currentScan_, SLOT(setNotes(QString)));
		disconnect(runSelector_, SIGNAL(currentRunIdChanged(int)), currentScan_, SLOT(setRunId(int)));
		disconnect(sampleEditor_, SIGNAL(currentSampleChanged(int)), currentScan_, SLOT(setSampleId(int)));
		disconnect(sampleEditor_, SIGNAL(currentSamplePointerChanged(const AMSamplePre2013*)), currentScan_, SLOT(setSamplePre2013(const AMSamplePre2013*)));
		disconnect(currentScan_, SIGNAL(numberChanged(int)), this, SLOT(refreshWindowTitle()));
		disconnect(currentScan_, SIGNAL(nameChanged(QString)), this, SLOT(refreshWindowTitle()));
	}

	// it becomes now the new scan:
	currentScan_ = newScan;

	// update all widgets to match the current scan
	updateEditor(currentScan_);


	if(currentScan_) {
		// removed: connect(currentScan_, SIGNAL(metaDataChanged()), this, SLOT(onScanMetaDataChanged()));

		// make connections to widgets, so that widgets edit this scan:
		connect(ui_.scanName, SIGNAL(textChanged(QString)), currentScan_, SLOT(setName(QString)));
		connect(ui_.scanNumber, SIGNAL(valueChanged(int)), currentScan_, SLOT(setNumber(int)));
		connect(this, SIGNAL(notesChanged(QString)), currentScan_, SLOT(setNotes(QString)));
		connect(runSelector_, SIGNAL(currentRunIdChanged(int)), currentScan_, SLOT(setRunId(int)));
		connect(sampleEditor_, SIGNAL(currentSampleChanged(int)), currentScan_, SLOT(setSampleId(int)));
		connect(sampleEditor_, SIGNAL(currentSamplePointerChanged(const AMSamplePre2013*)), currentScan_, SLOT(setSamplePre2013(const AMSamplePre2013*)));
		connect(currentScan_, SIGNAL(numberChanged(int)), this, SLOT(refreshWindowTitle()));
		connect(currentScan_, SIGNAL(nameChanged(QString)), this, SLOT(refreshWindowTitle()));

		// \todo When migrating to multiple scan selection, this will need to be changed:
		ui_.saveScanButton->setEnabled(true);
		ui_.closeScanButton->setEnabled(true);
	}
	else {
		// \todo When migrating to multiple scan selection, this will need to be changed:
		ui_.saveScanButton->setEnabled(false);
		ui_.closeScanButton->setEnabled(false);
	}


	if(scanSetModel_->scanCount() == 0)
		scanSetModel_->setExclusiveDataSourceByName(QString());

}


void AMGenericScanEditor::updateEditor(AMScan *scan) {
	if(scan) {

		ui_.scanId->setText(QString("%1").arg(scan->id()));
		connect(scan, SIGNAL(storedToDb()), this, SLOT(onScanSavedToDatabase()));
		ui_.scanName->setText(scan->name());
		ui_.scanNumber->setValue(scan->number());
		ui_.scanDate->setText( AMDateTimeUtils::prettyDate(scan->dateTime()));
		ui_.scanDuration->setText(scan->currentlyScanning() ? ("Acquiring " % AMDateTimeUtils::prettyDuration(currentScan_->dateTime(), QDateTime::currentDateTime(), true))
															: AMDateTimeUtils::prettyDuration(scan->dateTime(), scan->endDateTime()));
		ui_.scanTime->setText( scan->dateTime().time().toString("h:mmap") );
		ui_.notesEdit->setPlainText( scan->notes() );
		runSelector_->setCurrentRunId(scan->runId());
		if(scan->samplePre2013()){
			sampleEditor_->setCurrentSampleFromId(scan->sampleId());
			if(sampleEditor_->isHidden()){
				sampleEditor_->show();
				sampleBriefView_->hide();
			}
		}
		else{
			sampleBriefView_->setSample(scan->sample());
			if(sampleBriefView_->isHidden()){
				sampleBriefView_->show();
				sampleEditor_->hide();
			}
		}
		dataSourcesEditor_->setCurrentScan(scan);
		conditionsTableView_->setFromInfoList(scan->scanInitialConditions());

		// Only the 2D scan view currently uses a current scan.  But only pass it if it exists.
		if (scanView2D_)
			scanView2D_->setCurrentScan(scan);

		//ui_.topFrameTitle->setText(QString("Editing %1 #%2").arg(scan->name()).arg(scan->number()));
		ui_.topFrameTitle->setText(QString("Editing %1").arg(scan->fullName()));
	}

	else {
		ui_.scanId->setText(QString("Pending..."));
		ui_.scanName->setText( QString() );
		ui_.scanNumber->setValue(0);
		ui_.scanDate->setText( QString() );
		ui_.scanDuration->setText( QString() );
		ui_.scanTime->setText( QString() );
		ui_.notesEdit->clear();

		// what to set run selector to?

		sampleEditor_->setCurrentSampleFromId(-1);
		sampleBriefView_->setSample(0);
		dataSourcesEditor_->setCurrentScan(0);
		conditionsTableView_->setFromInfoList(0);

		// Only the 2D scan view currently uses a current scan.  But only pass it if it exists.
		if (scanView2D_)
			scanView2D_->setCurrentScan(0);

		ui_.topFrameTitle->setText("Scan Editor");
	}
}


// called when the close buttons in the list of scans are clicked
void AMGenericScanEditor::onScanModelCloseClicked(const QModelIndex& index) {
	// Just a quick check to make sure that this is a valid scan index (ie: parent is a top-level index; index.row() is the scan index)
	if(index.parent() == QModelIndex() && index.isValid())
		removeScanWithModifiedCheck(scanSetModel_->scanAt(index.row()));

}

#include <QMessageBox>
#include <QApplication>
// Remove a scan, but ask the user for confirmation if it's been modified.
bool AMGenericScanEditor::removeScanWithModifiedCheck(AMScan* scan) {

#ifndef ACQUAMAN_NO_ACQUISITION
	// Potential problem 1: Is the scan acquiring?
	if(scan->scanController()) {	// look for a valid scanController().
		shouldStopAcquiringScan(scan);
		return false;
//		if(shouldStopAcquiringScan(scan)) {
//			// stop the scan. Warning: this won't happen instantly... there's event driven handling in the scan controller. However, since we won't delete the scan (only release our interest), we can remove it.
//			scan->scanController()->cancel();
//			removeScan(scan);
//			return true;
//		}
//		else	// Note: it is possible to close the editor and let the scan keep acquiring. Is that something we want to let users do? For now, no.
//			return false;
	}
#endif


	// Potential problem 2: Is the scan modified? Do they want to save their changes?
	if(scan->modified()) {
		int result = shouldSaveModifiedScan(scan);

		if(result == QMessageBox::Discard) {
			removeScan(scan);
			return true;
		}
		else if(result == QMessageBox::Save) {
			bool saveSuccess;
			if(scan->database())
				saveSuccess = scan->storeToDb(scan->database());
			else
				saveSuccess = scan->storeToDb(AMDatabase::database("user"));

			if(saveSuccess) {
				removeScan(scan);
				return true;
			}
			else {
				return false;
			}
		}
		else {	// User chose "cancel"
			return false;
		}
	}

	// No issues, just remove and return success.

	removeScan(scan);
	return true;
}

// Overloaded to enable drag-dropping scans (when Drag Action = Qt::CopyAction and mime-type = "text/uri-list" with the proper format.)
void AMGenericScanEditor::dragEnterEvent(QDragEnterEvent *event) {
	if(event->possibleActions() & Qt::CopyAction
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

	if(!( event->possibleActions() & Qt::CopyAction
		   && event->mimeData()->hasUrls()) )
		return;


	if(dropScanURLs(event->mimeData()->urls()))
		event->acceptProposedAction();
}

bool AMGenericScanEditor::dropScanURLs(const QList<QUrl>& urls) {

	if(!urls.count())
		return false;

	bool accepted = false;

	foreach(QUrl url, urls) {

		bool isScanning;
		QString scanName;
		AMScan* scan = AMScan::createFromDatabaseUrl(url, false, &isScanning, &scanName);

		// See if this scan is acquiring, and refuse to create a new instance if so.
		/// \todo With the new AMScan memory management model, we could actually open the existing AMScan* instance in multiple editors if desired... But propagation of changes under editing might be a problem; all editors currently assuming they are the only ones modifying the scan.
		if(isScanning) {
			QMessageBox stillScanningEnquiry;
			stillScanningEnquiry.setWindowTitle("This scan is still acquiring.");
			stillScanningEnquiry.setText(QString("The scan '%1' is currently still acquiring data, so you can't open multiple copies of it yet.")
										 .arg(scanName));
			stillScanningEnquiry.setIcon(QMessageBox::Information);
			stillScanningEnquiry.addButton(QMessageBox::Ok);
			stillScanningEnquiry.exec();
			continue;
		}

		if(!scan)
			continue;

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
		removeScanWithModifiedCheck(scanSetModel_->scanAt(i.row()));
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
			scan->storeToDb(AMDatabase::database("user"));
	}
}

void AMGenericScanEditor::onOpenScanButtonClicked() {
	if(!chooseScanDialog_) {
		chooseScanDialog_ = new AMChooseScanDialog(AMDatabase::database("user"), "Add an existing scan", "Choose one or more existing scans to open in this editor.", true, this);
		connect(chooseScanDialog_, SIGNAL(accepted()), this, SLOT(onChooseScanDialogAccepted()));
	}

	chooseScanDialog_->show();
}

void AMGenericScanEditor::removeScan(AMScan *scan) {
	scanSetModel_->removeScan(scan);
	refreshWindowTitle();
}

void AMGenericScanEditor::onChooseScanDialogAccepted()
{
	dropScanURLs(chooseScanDialog_->getSelectedScans());
}

void AMGenericScanEditor::closeEvent(QCloseEvent* e)
{

	e->setAccepted(canCloseEditor());

	if(e->isAccepted()) {
		// remove all scans
		while(scanSetModel_->scanCount()) {
			AMScan* s = scanSetModel_->scanAt(scanSetModel_->scanCount()-1);
			scanSetModel_->removeScan(s);
		}
	}

}

bool AMGenericScanEditor::shouldStopAcquiringScan(AMScan *scan)
{
	QMessageBox scanningEnquiry;
	scanningEnquiry.setText(QString("The scan '%1' (#%2) is still acquiring.")
							.arg(scan->name())
							.arg(scan->number()));
	scanningEnquiry.setInformativeText("You can't close this scan while it's still acquiring. Use the workflow to stop it first.");
	scanningEnquiry.setIcon(QMessageBox::Information);
	scanningEnquiry.addButton(QMessageBox::Ok);
	scanningEnquiry.setDefaultButton(QMessageBox::Ok);
	scanningEnquiry.setEscapeButton(QMessageBox::Ok);

	scanningEnquiry.exec();
	return false;
}

int AMGenericScanEditor::shouldSaveModifiedScan(AMScan *scan)
{
	QMessageBox savingEnquiry(this);
	savingEnquiry.setText(QString("Do you want to save the changes you made to the scan '%1' (#%2)?").arg(scan->name()).arg(scan->number()));
	savingEnquiry.setInformativeText("Your changes will be lost if you don't save them.");
	savingEnquiry.setIcon(QMessageBox::Question);
	savingEnquiry.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	savingEnquiry.setDefaultButton(QMessageBox::Save);
	savingEnquiry.setEscapeButton(QMessageBox::Cancel);

	return savingEnquiry.exec();
}

bool AMGenericScanEditor::canCloseEditor()
{
	bool canClose = true;

#ifndef ACQUAMAN_NO_ACQUISITION
	// Check for any acquiring scans
	for(int i=0; i<scanCount(); i++) {
		AMScan* scan = scanAt(i);
		if(scan->scanController()) {
			// is still scanning.
			shouldStopAcquiringScan(scan);
			canClose = false;
//			if(shouldStopAcquiringScan(scan)) {
//				scan->scanController()->cancel(); // Since we won't delete the scan ourselves, but we know it's going to stop at some point, we can safely close. Don't set canClose to false.
//			}
//			else {
//				return false;	// they chose to cancel, so don't bother asking about anything else.
//			}
		}
	}
#endif
	// Check for any modified scans?
	for(int i=0; i<scanCount(); i++) {
		AMScan* scan = scanAt(i);
		if(scan->modified()) {
			// find out if we should cancel, or save or discard changes
			int response = shouldSaveModifiedScan(scan);
			if(response == QMessageBox::Cancel) {
				return false;	// they chose to cancel, so don't bother asking about anything else.
			}
			else if(response == QMessageBox::Save) {
				bool saveSuccess;
				if(scan->database())
					saveSuccess = scan->storeToDb(scan->database());
				else
					saveSuccess = scan->storeToDb(AMDatabase::database("user"));

				if(saveSuccess) {
					continue;	// problem resolved here... can still close.
				}
				else {
					canClose = false;
				}
			}
			else {	// user chose to discard changes... can still close.
				continue;
			}
		}
	}

	return canClose;
}

void AMGenericScanEditor::onOneSecondTimer()
{
	if(currentScan_) {
		if(currentScan_->currentlyScanning())
			ui_.scanDuration->setText("Acquiring " % AMDateTimeUtils::prettyDuration(currentScan_->dateTime(), QDateTime::currentDateTime(), true));
		else {
			// this is a cheap easy way to find out if we've switched from acquiring to not acquiring, and therefore should put in the final duration
			if(ui_.scanDuration->text().startsWith("Acqu")) {
				ui_.scanDuration->setText(AMDateTimeUtils::prettyDuration(currentScan_->dateTime(), currentScan_->endDateTime()));
			}
		}
	}

}

void AMGenericScanEditor::onScanSavedToDatabase()
{
	ui_.scanId->setText(QString("%1").arg(currentScan_->id()));
	ui_.scanNumber->setValue(currentScan_->number());
}

#include <QFileDialog>
void AMGenericScanEditor::exportGraphicsToFile()
{
	QString filters = QString("%1;;%2;;%3;;%4;;%5;;%6").arg("PDF Files (*.pdf)")
			.arg("JPEG Files (*.jpg *.jpeg)")
			.arg("PNG Files (*.png)")
			.arg("TIFF Files (*.tiff)")
			.arg("PPM Files (*.ppm)")
			.arg("BMP Files (*.bmp)");

	QFileDialog dialog(this, "Save Graphics As...", QString(), filters);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.exec();
	QString fileName = dialog.selectedFiles().first();

	if(!fileName.isEmpty()) {

		if(dialog.selectedNameFilter().contains("PDF") && !fileName.endsWith(".pdf", Qt::CaseInsensitive))
			fileName.append(".pdf");

		else if(dialog.selectedNameFilter().contains("JPEG") && !fileName.endsWith(".jpg", Qt::CaseInsensitive))
			fileName.append(".jpg");

		else if(dialog.selectedNameFilter().contains("PNG") && !fileName.endsWith(".png", Qt::CaseInsensitive))
			fileName.append(".png");

		else if(dialog.selectedNameFilter().contains("TIFF") && !fileName.endsWith(".tiff", Qt::CaseInsensitive))
			fileName.append(".tiff");

		else if(dialog.selectedNameFilter().contains("PPM") && !fileName.endsWith(".ppm", Qt::CaseInsensitive))
			fileName.append(".ppm");

		else if(dialog.selectedNameFilter().contains("BMP") && !fileName.endsWith(".bmp", Qt::CaseInsensitive))
			fileName.append(".bmp");

		QFileInfo info(fileName);
		if (info.exists() && QMessageBox::Cancel == QMessageBox::warning(this,
													"File already exists...",
													QString("%1 already exists.  Would you like to overwrite it?").arg(info.fileName()),
													QMessageBox::Cancel,
													QMessageBox::Save))
			return;

		if(!using2DScanView()) {
			scanView_->exportGraphicsFile(fileName);
			AMErrorMon::information(this, 0, QString("Exported the current plot to '%1'").arg(fileName));
		}

		else {
			scanView2D_->exportGraphicsFile(fileName);
			AMErrorMon::information(this, 0, QString("Exported the current plot to '%1'").arg(fileName));
		}
	}
}

void AMGenericScanEditor::printGraphics()
{


		if(!using2DScanView()) {
			scanView_->printGraphics();
			AMErrorMon::information(this, 0, QString("Current plot sent to printer."));
		}

		else {
			scanView2D_->printGraphics();
			AMErrorMon::information(this, 0, QString("Current plot sent to printer."));
		}

}
