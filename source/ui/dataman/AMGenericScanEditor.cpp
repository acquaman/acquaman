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


#include "AMGenericScanEditor.h"
#include <QMessageBox>
#include <QApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QList>
#include <QUrl>
#include <QTimer>
#include <QStringBuilder>
#include <QDebug>

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
	setupUi();
	scanNameEdit_ = new AMRegExpLineEdit("/|;|@|#|<|>", Qt::CaseInsensitive, "/;#>@< characters are not allowed.");
	scanNameEdit_->setFrame(false);
	scanNameEdit_->setValidIfMatches(false);
	scanInformationGridLayout_->addWidget(scanNameEdit_, 1, 1);
	scanTitle_->setStyleSheet("font: " AM_FONT_LARGE_ "pt \"Lucida Grande\";\ncolor: rgb(79, 79, 79);");
	statusTextLabel_->setStyleSheet("color: white;\nfont: bold " AM_FONT_SMALL_ "pt \"Lucida Grande\"");
	setWindowTitle("Scan Editor");

	// Add extra UI components:
	stackWidget_ = new AMVerticalStackWidget();
	stackWidget_->setMinimumWidth(200);
	stackWidget_->setMaximumWidth(360);
	rightVerticalLayout_->addWidget(stackWidget_);

	// Add scan view (plots)
	scanView_ = new AMScanView();
	scanView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	leftVerticalLayout_->insertWidget(0, scanView_, 2);


	// Ensure that the 2D scan view is pointing to nowhere.
	scanView2D_ = 0;

	// share the scan set model with the AMScanView
	scanSetModel_ = scanView_->model();

	// And set this model on the list view of scans:
	scanListView_->setModel(scanSetModel_);
	scanListView_->setSelectionMode(QAbstractItemView::SingleSelection);

	del_ = new AMDetailedItemDelegate();
	del_->setCloseButtonsEnabled(true);
	connect(del_, SIGNAL(closeButtonClicked(QModelIndex)), this, SLOT(onScanModelCloseClicked(QModelIndex)));
	scanListView_->setItemDelegate(del_);
	scanListView_->setAlternatingRowColors(true);
	scanListView_->setAttribute( Qt::WA_MacShowFocusRect, false);

	// Add run selector:
	runSelector_ = new AMRunSelector(AMDatabase::database("user"));
	scanInfoLayout->insertWidget(1, runSelector_);



	// Add detailed editor widgets:
	QWidget* sampleEditorHolder = new QWidget();	// just used to add a margin around the sample editor itself, which has no margins.
	sampleEditorHolder->setLayout(new QVBoxLayout);
	sampleEditor_ = new AMSamplePre2013Editor(AMDatabase::database("user"));
	sampleEditorHolder->layout()->addWidget(sampleEditor_);
	sampleEditor_->hide();
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

	connect(notesEdit_, SIGNAL(textChanged()), this, SLOT(onNotesTextChanged()));

	// watch for changes in the selected/deselected scans:
	connect(scanListView_->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentChanged(QModelIndex,QModelIndex)));

	// \todo make this whole window accept drops of scans, with the 'amd://database/table/id' uri-list mime type
	setAcceptDrops(true);

	currentScan_ = 0;




	// disable drops on text fields that we don't want to accept drops
	scanNameEdit_->setAcceptDrops(false);
	notesEdit_->setAcceptDrops(false);


	// Connect open,save,close buttons
	connect(openScanButton_, SIGNAL(clicked()), this, SLOT(onOpenScanButtonClicked()));
	connect(closeScanButton_, SIGNAL(clicked()), this, SLOT(onCloseScanButtonClicked()));
	connect(saveScanButton_, SIGNAL(clicked()), this, SLOT(onSaveScanButtonClicked()));


	// close button and save buttons are initially disabled, there's no scan to act on
	closeScanButton_->setEnabled(false);
	saveScanButton_->setEnabled(false);



	chooseScanDialog_ = 0;

	oneSecondTimer_ = new QTimer(this);
	connect(oneSecondTimer_, SIGNAL(timeout()), this, SLOT(onOneSecondTimer()));

	oneSecondTimer_->start(1000);
}

AMGenericScanEditor::AMGenericScanEditor(bool use2DScanView, QWidget *parent)
	: QWidget(parent)
{
	setupUi();
	scanNameEdit_ = new AMRegExpLineEdit("/|;|@|#|<|>", Qt::CaseInsensitive, "/;#>@< characters are not allowed.");
	scanNameEdit_->setFrame(false);
	scanNameEdit_->setValidIfMatches(false);
	scanInformationGridLayout_->addWidget(scanNameEdit_, 1, 1);
	scanTitle_->setStyleSheet("font: " AM_FONT_LARGE_ "pt \"Lucida Grande\";\ncolor: rgb(79, 79, 79);");
	statusTextLabel_->setStyleSheet("color: white;\nfont: bold " AM_FONT_SMALL_ "pt \"Lucida Grande\"");
	setWindowTitle("Scan Editor");

	// Add extra UI components:
	stackWidget_ = new AMVerticalStackWidget();
	stackWidget_->setMinimumWidth(200);
	stackWidget_->setMaximumWidth(360);
	rightVerticalLayout_->addWidget(stackWidget_);

	// Add scan view (plots)
	if (use2DScanView){

		scanView_ = 0;
		scanView2D_ = new AM2DScanView();
		scanView2D_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		leftVerticalLayout_->insertWidget(0, scanView2D_, 2);

		// share the scan set model with the AMScanView
		scanSetModel_ = scanView2D_->model();

		connect(scanView2D_, SIGNAL(dataPositionChanged(QPoint)), this, SLOT(onDataPositionChanged(QPoint)));
		openScanButton_->setEnabled(false);
	}

	else {

		scanView2D_ = 0;
		scanView_ = new AMScanView();
		scanView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		leftVerticalLayout_->insertWidget(0, scanView_, 2);

		// share the scan set model with the AMScanView
		scanSetModel_ = scanView_->model();
	}

	// And set this model on the list view of scans:
	scanListView_->setModel(scanSetModel_);
	scanListView_->setSelectionMode(QAbstractItemView::SingleSelection);

	del_ = new AMDetailedItemDelegate();
	del_->setCloseButtonsEnabled(true);
	connect(del_, SIGNAL(closeButtonClicked(QModelIndex)), this, SLOT(onScanModelCloseClicked(QModelIndex)));
	scanListView_->setItemDelegate(del_);
	scanListView_->setAlternatingRowColors(true);
	scanListView_->setAttribute( Qt::WA_MacShowFocusRect, false);

	// Add run selector:
	runSelector_ = new AMRunSelector(AMDatabase::database("user"));
	scanInfoLayout->insertWidget(1, runSelector_);


	// Add detailed editor widgets:
	QWidget* sampleEditorHolder = new QWidget();	// just used to add a margin around the sample editor itself, which has no margins.
	sampleEditorHolder->setLayout(new QVBoxLayout);
	sampleEditor_ = new AMSamplePre2013Editor(AMDatabase::database("user"));
	sampleEditorHolder->layout()->addWidget(sampleEditor_);
	sampleEditor_->hide();
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

	connect(notesEdit_, SIGNAL(textChanged()), this, SLOT(onNotesTextChanged()));

	// watch for changes in the selected/deselected scans:
	connect(scanListView_->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentChanged(QModelIndex,QModelIndex)));

	// \todo make this whole window accept drops of scans, with the 'amd://database/table/id' uri-list mime type
	setAcceptDrops(true);

	currentScan_ = 0;


	// disable drops on text fields that we don't want to accept drops
	scanNameEdit_->setAcceptDrops(false);
	notesEdit_->setAcceptDrops(false);


	// Connect open,save,close buttons
	connect(openScanButton_, SIGNAL(clicked()), this, SLOT(onOpenScanButtonClicked()));
	connect(closeScanButton_, SIGNAL(clicked()), this, SLOT(onCloseScanButtonClicked()));
	connect(saveScanButton_, SIGNAL(clicked()), this, SLOT(onSaveScanButtonClicked()));

	// close button and save buttons are initially disabled, there's no scan to act on
	closeScanButton_->setEnabled(false);
	saveScanButton_->setEnabled(false);

	chooseScanDialog_ = 0;

	oneSecondTimer_ = new QTimer(this);
	connect(oneSecondTimer_, SIGNAL(timeout()), this, SLOT(onOneSecondTimer()));
	oneSecondTimer_->start(1000);
}

AMGenericScanEditor::~AMGenericScanEditor() {

	while(scanSetModel_->scanCount()) {
		AMScan* s = scanSetModel_->scanAt(scanSetModel_->scanCount()-1);
		scanSetModel_->removeScan(s);
		s->deleteLater();
	}

	del_->deleteLater();
	oneSecondTimer_->deleteLater();

	if(chooseScanDialog_)
		chooseScanDialog_->deleteLater();
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

void AMGenericScanEditor::setEnergyRange(double low, double high)
{
	if (scanView_)
		scanView_->setEnergyRange(low, high);

	else if (scanView2D_)
		scanView2D_->setEnergyRange(low, high);
}

void AMGenericScanEditor::addSingleSpectrumEmissionLineNameFilter(const QRegExp &newNameFilter)
{
	if (scanView_)
		scanView_->addSingleSpectrumEmissionLineNameFilter(newNameFilter);

	else if (scanView2D_)
		scanView2D_->addSingleSpectrumEmissionLineNameFilter(newNameFilter);
}

void AMGenericScanEditor::removeSingleSpectrumEmissionLineNameFilter(int index)
{
	if (scanView_)
		scanView_->removeSingleSpectrumEmissionLineNameFilter(index);

	else if (scanView2D_)
		scanView2D_->removeSingleSpectrumEmissionLineNameFilter(index);
}

void AMGenericScanEditor::removeSingleSpectrumEmissionLineNameFilter(const QRegExp &filter)
{
	if (scanView_)
		scanView_->removeSingleSpectrumEmissionLineNameFilter(filter);

	else if (scanView2D_)
		scanView2D_->removeSingleSpectrumEmissionLineNameFilter(filter);
}

void AMGenericScanEditor::addSingleSpectrumPileUpPeakNameFilter(const QRegExp &newNameFilter)
{
	if (scanView_)
		scanView_->addSingleSpectrumPileUpPeakNameFilter(newNameFilter);

	else if (scanView2D_)
		scanView2D_->addSingleSpectrumPileUpPeakNameFilter(newNameFilter);
}

void AMGenericScanEditor::removeSingleSpectrumPileUpPeakNameFilter(int index)
{
	if (scanView_)
		scanView_->removeSingleSpectrumPileUpPeakNameFilter(index);

	else if (scanView2D_)
		scanView2D_->removeSingleSpectrumPileUpPeakNameFilter(index);
}

void AMGenericScanEditor::removeSingleSpectrumPileUpPeakNameFilter(const QRegExp &filter)
{
	if (scanView_)
		scanView_->removeSingleSpectrumPileUpPeakNameFilter(filter);

	else if (scanView2D_)
		scanView2D_->removeSingleSpectrumPileUpPeakNameFilter(filter);
}

void AMGenericScanEditor::addSingleSpectrumCombinationPileUpPeakNameFilter(const QRegExp &newNameFilter)
{
	if (scanView_)
		scanView_->addSingleSpectrumCombinationPileUpPeakNameFilter(newNameFilter);

	else if (scanView2D_)
		scanView2D_->addSingleSpectrumCombinationPileUpPeakNameFilter(newNameFilter);
}

void AMGenericScanEditor::removeSingleSpectrumCombinationPileUpPeakNameFilter(int index)
{
	if (scanView_)
		scanView_->removeSingleSpectrumCombinationPileUpPeakNameFilter(index);

	else if (scanView2D_)
		scanView2D_->removeSingleSpectrumCombinationPileUpPeakNameFilter(index);
}

void AMGenericScanEditor::removeSingleSpectrumCombinationPileUpPeakNameFilter(const QRegExp &filter)
{
	if (scanView_)
		scanView_->removeSingleSpectrumCombinationPileUpPeakNameFilter(filter);

	else if (scanView2D_)
		scanView2D_->removeSingleSpectrumCombinationPileUpPeakNameFilter(filter);
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

	scanListView_->setCurrentIndex(scanSetModel_->indexForScan(scanSetModel_->indexOf(newScan)));

	if(scanSetModel_->exclusiveDataSourceName().isEmpty()) {
		QVector<int> nonHiddenDataSourceIndexes = newScan->nonHiddenDataSourceIndexes();
		if(!nonHiddenDataSourceIndexes.isEmpty())
			scanSetModel_->setExclusiveDataSourceByName(newScan->dataSourceAt(nonHiddenDataSourceIndexes.first())->name());
	}

	connect(newScan, SIGNAL(nameChanged(QString)), this, SLOT(onScanDetailsChanged()));
	connect(newScan, SIGNAL(numberChanged(int)), this, SLOT(onScanDetailsChanged()));

	if (newScan->scanConfiguration())
		connect(newScan->scanConfiguration(), SIGNAL(configurationChanged()), this, SLOT(refreshScanInfo()));


	emit scanAdded(this, newScan);
	refreshWindowTitle();
}

void AMGenericScanEditor::refreshWindowTitle() {

	int numScans = scanSetModel_->scanCount();

	if(numScans == 0) {
		setWindowTitle("Scan Editor");
		return;
	}

	if(numScans == 1) {
		setWindowTitle(scanSetModel_->scanAt(0)->fullName());
		scanTitle_->setText(scanSetModel_->scanAt(0)->fullName());
	}
	else
	{
		setWindowTitle(QString("Viewing %1 Scans").arg(numScans));
		scanTitle_->setText(QString("Viewing %1 Scans").arg(numScans));
	}

}

void AMGenericScanEditor::refreshScanConditions() {
	conditionsTableView_->setFromInfoList(currentScan_->scanInitialConditions());
}

void AMGenericScanEditor::onCurrentChanged ( const QModelIndex & selected, const QModelIndex & deselected ) {

	Q_UNUSED(deselected)

	AMScan* newScan = 0;
	if(selected.isValid())
		newScan = scanSetModel_->scanAt(selected.row());

	// disconnect the old scan:
	if(currentScan_) {
		disconnect(scanNameEdit_, 0, currentScan_, 0);
		disconnect(scanNumber_, 0, currentScan_, 0);
		disconnect(this, SIGNAL(notesChanged(QString)), currentScan_, SLOT(setNotes(QString)));
		disconnect(runSelector_, SIGNAL(currentRunIdChanged(int)), currentScan_, SLOT(setRunId(int)));
		disconnect(sampleEditor_, SIGNAL(currentSampleChanged(int)), currentScan_, SLOT(setSampleId(int)));
		disconnect(sampleEditor_, SIGNAL(currentSamplePointerChanged(const AMSamplePre2013*)), currentScan_, SLOT(setSamplePre2013(const AMSamplePre2013*)));
		disconnect(currentScan_, SIGNAL(numberChanged(int)), this, SLOT(refreshWindowTitle()));
		disconnect(currentScan_, SIGNAL(nameChanged(QString)), this, SLOT(refreshWindowTitle()));
		disconnect(currentScan_, SIGNAL(scanInitialConditionsChanged()), this, SLOT(refreshScanConditions()));

		if (currentScan_->scanConfiguration())
			disconnect(currentScan_->scanConfiguration(), SIGNAL(configurationChanged()), this, SLOT(refreshScanInfo()));
	}

	// it becomes now the new scan:
	currentScan_ = newScan;

	// update all widgets to match the current scan
	updateEditor(currentScan_);

	if(currentScan_) {

		// make connections to widgets, so that widgets edit this scan:
		connect(scanNameEdit_, SIGNAL(textChanged(QString)), currentScan_, SLOT(setName(QString)));
		connect(scanNumber_, SIGNAL(valueChanged(int)), currentScan_, SLOT(setNumber(int)));
		connect(this, SIGNAL(notesChanged(QString)), currentScan_, SLOT(setNotes(QString)));
		connect(runSelector_, SIGNAL(currentRunIdChanged(int)), currentScan_, SLOT(setRunId(int)));
		connect(sampleEditor_, SIGNAL(currentSampleChanged(int)), currentScan_, SLOT(setSampleId(int)));
		connect(sampleEditor_, SIGNAL(currentSamplePointerChanged(const AMSamplePre2013*)), currentScan_, SLOT(setSamplePre2013(const AMSamplePre2013*)));
		connect(currentScan_, SIGNAL(numberChanged(int)), this, SLOT(refreshWindowTitle()));
		connect(currentScan_, SIGNAL(nameChanged(QString)), this, SLOT(refreshWindowTitle()));
		connect(currentScan_, SIGNAL(scanInitialConditionsChanged()), this, SLOT(refreshScanConditions()));

		if (currentScan_->scanConfiguration())
			connect(currentScan_->scanConfiguration(), SIGNAL(configurationChanged()), this, SLOT(refreshScanInfo()));


		// \todo When migrating to multiple scan selection, this will need to be changed:
		saveScanButton_->setEnabled(true);
		closeScanButton_->setEnabled(true);
	}
	else {
		// \todo When migrating to multiple scan selection, this will need to be changed:
		saveScanButton_->setEnabled(false);
		closeScanButton_->setEnabled(false);
	}


	if(scanSetModel_->scanCount() == 0)
		scanSetModel_->setExclusiveDataSourceByName(QString());

}

void AMGenericScanEditor::updateEditor(AMScan *scan) {
	if(scan) {

		scanId_->setText(QString("%1").arg(scan->id()));
		connect(scan, SIGNAL(storedToDb()), this, SLOT(onScanSavedToDatabase()));
		scanNameEdit_->setText(scan->name());
		scanNumber_->setValue(scan->number());
		scanDate_->setText( AMDateTimeUtils::prettyDate(scan->dateTime()));
		scanDuration_->setText(scan->currentlyScanning() ? ("Acquiring " % AMDateTimeUtils::prettyDuration(currentScan_->dateTime(), QDateTime::currentDateTime(), true))
															: AMDateTimeUtils::prettyDuration(scan->dateTime(), scan->endDateTime()));
		scanTime_->setText( scan->dateTime().time().toString("h:mmap") );
		scanEnd_->setText(scan->currentlyScanning() ? ("Approx " % (scan->dateTime().addSecs((int)scan->scanConfiguration()->expectedDuration())).time().toString("h:mmap"))
								  : scan->endDateTime().time().toString("h:mmap"));
		notesEdit_->setPlainText( scan->notes() );

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

		scanTitle_->setText(QString("Editing %1").arg(scan->fullName()));
	}

	else {
		scanId_->setText(QString("Pending..."));
		scanNameEdit_->setText( QString() );
		scanNumber_->setValue(0);
		scanDate_->setText( QString() );
		scanDuration_->setText( QString() );
		scanTime_->setText( QString() );
		scanEnd_->setText( QString() );
		notesEdit_->clear();

		// what to set run selector to?

		sampleEditor_->setCurrentSampleFromId(-1);
		sampleBriefView_->setSample(0);
		dataSourcesEditor_->setCurrentScan(0);
		conditionsTableView_->setFromInfoList(0);

		// Only the 2D scan view currently uses a current scan.  But only pass it if it exists.
		if (scanView2D_)
			scanView2D_->setCurrentScan(0);

		scanTitle_->setText("Scan Editor");
	}
}


// called when the close buttons in the list of scans are clicked
void AMGenericScanEditor::onScanModelCloseClicked(const QModelIndex& index) {
	// Just a quick check to make sure that this is a valid scan index (ie: parent is a top-level index, index.row() is the scan index)
	if(index.parent() == QModelIndex() && index.isValid())
		removeScanWithModifiedCheck(scanSetModel_->scanAt(index.row()));

}


// Remove a scan, but ask the user for confirmation if it's been modified.
bool AMGenericScanEditor::removeScanWithModifiedCheck(AMScan* scan) {

#ifndef ACQUAMAN_NO_ACQUISITION
	// Potential problem 1: Is the scan acquiring?
	if(scan->scanController()) {	// look for a valid scanController().
		shouldStopAcquiringScan(scan);
		return false;
	}
#endif


	// Potential problem 2: Is the scan modified? Do they want to save their changes?
	if(scan->modified()) {
		int result = shouldSaveModifiedScan(scan);

		if(result == QMessageBox::Discard) {
			removeScan(scan);
			scan->deleteLater();
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
				scan->deleteLater();
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
	scan->deleteLater();
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
		/// \todo With the new AMScan memory management model, we could actually open the existing AMScan* instance in multiple editors if desired... But propagation of changes under editing might be a problem, all editors currently assuming they are the only ones modifying the scan.
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
	QModelIndexList scanIndexes = scanListView_->selectionModel()->selectedIndexes();
	foreach(QModelIndex i, scanIndexes) {
		removeScanWithModifiedCheck(scanSetModel_->scanAt(i.row()));
	}
}

void AMGenericScanEditor::onSaveScanButtonClicked() {
	// this function is ready for when multiple scan selection is enabled
	QModelIndexList scanIndexes = scanListView_->selectionModel()->selectedIndexes();
	foreach(QModelIndex i, scanIndexes) {
		AMScan* scan = scanSetModel_->scanAt(i.row());
		if(scan->database())
			scan->storeToDb(scan->database());
		else
			scan->storeToDb(AMDatabase::database("user"));
	}
}

void AMGenericScanEditor::onOpenScanButtonClicked()
{
	QStringList databaseNames = AMDatabase::registeredDatabases();
	databaseNames.removeOne("actions");
	databaseNames.removeOne("scanActions");
	QList<AMDatabase *> databases;

	foreach (QString databaseName, databaseNames)
		databases << AMDatabase::database(databaseName);

	chooseScanDialog_ = new AMChooseScanDialog(databases, "Add an existing scan", "Choose one or more existing scans to open in this editor.", this);
	connect(chooseScanDialog_, SIGNAL(accepted()), this, SLOT(onChooseScanDialogAccepted()));
	connect(chooseScanDialog_, SIGNAL(rejected()), this, SLOT(onChooseScanDialogRejected()));

	chooseScanDialog_->show();
}

void AMGenericScanEditor::removeScan(AMScan *scan) {
	scanSetModel_->removeScan(scan);
	refreshWindowTitle();
}

void AMGenericScanEditor::onChooseScanDialogAccepted()
{
	dropScanURLs(chooseScanDialog_->getSelectedScans());
	chooseScanDialog_->deleteLater();
	chooseScanDialog_ = 0;
}

void AMGenericScanEditor::onChooseScanDialogRejected()
{
	chooseScanDialog_->deleteLater();
	chooseScanDialog_ = 0;
}

void AMGenericScanEditor::closeEvent(QCloseEvent* e)
{

	e->setAccepted(canCloseEditor());

	if(e->isAccepted()) {
		// remove all scans
		while(scanSetModel_->scanCount()) {
			AMScan* s = scanSetModel_->scanAt(scanSetModel_->scanCount()-1);
			scanSetModel_->removeScan(s);
			s->deleteLater();
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



bool AMGenericScanEditor::canCloseEditor(bool promptToSave)
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
		}
	}
#endif
	// Check for any modified scans?
	for(int i=0; i<scanCount(); i++) {
		AMScan* scan = scanAt(i);
		if(scan->modified() && !promptToSave)
			return false;
		else if(scan->modified()) {
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
			scanDuration_->setText("Acquiring " % AMDateTimeUtils::prettyDuration(currentScan_->dateTime(), QDateTime::currentDateTime(), true));
		else {
			// this is a cheap easy way to find out if we've switched from acquiring to not acquiring, and therefore should put in the final duration
			if(scanDuration_->text().startsWith("Acqu")) {
				scanDuration_->setText(AMDateTimeUtils::prettyDuration(currentScan_->dateTime(), currentScan_->endDateTime()));
			}
		}
	}

}

void AMGenericScanEditor::onScanSavedToDatabase()
{
	scanId_->setText(QString("%1").arg(currentScan_->id()));
	scanNumber_->setValue(currentScan_->number());
}

void AMGenericScanEditor::onScanDetailsChanged()
{
	refreshWindowTitle();
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
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.exec();

	QString fileName;
	if(!dialog.selectedFiles().isEmpty())
			fileName = dialog.selectedFiles().first();

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

void AMGenericScanEditor::setupUi()
{
	if (objectName().isEmpty())
		setObjectName(QString::fromUtf8("AMGenericScanEditor"));
	resize(801, 574);
	verticalLayout3_ = new QVBoxLayout();
	verticalLayout3_->setSpacing(0);
	verticalLayout3_->setContentsMargins(0, 0, 0, 0);
	verticalLayout3_->setObjectName(QString::fromUtf8("verticalLayout_3"));
	scanTitleFrame_ = new QFrame();
	scanTitleFrame_->setObjectName(QString::fromUtf8("scanTitleFrame_"));
	scanTitleFrame_->setMinimumSize(QSize(0, 60));
	scanTitleFrame_->setStyleSheet(QString::fromUtf8("background-color: rgb(236, 236, 236);"));
	horizontalLayout3_ = new QHBoxLayout();
	horizontalLayout3_->setObjectName(QString::fromUtf8("horizontalLayout_3"));
	scanTitleFrame_->setLayout(horizontalLayout3_);
	scanTitleIcon_ = new QLabel();
	scanTitleIcon_->setObjectName(QString::fromUtf8("scanTitleIcon_"));
	scanTitleIcon_->setMaximumSize(QSize(36, 36));
	scanTitleIcon_->setPixmap(QPixmap(QString::fromUtf8(":/system-software-update.png")));
	scanTitleIcon_->setScaledContents(true);

	horizontalLayout3_->addWidget(scanTitleIcon_);

	scanTitle_ = new QLabel();
	scanTitle_->setObjectName(QString::fromUtf8("scanTitle_"));
	scanTitle_->setStyleSheet(QString::fromUtf8("font: 16pt \"Lucida Grande\";\n"
"color: rgb(79, 79, 79);"));

	horizontalLayout3_->addWidget(scanTitle_);

	scanTitleHorizontalSpacer_ = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	horizontalLayout3_->addItem(scanTitleHorizontalSpacer_);


	verticalLayout3_->addWidget(scanTitleFrame_);

	statusFrame_ = new QFrame();
	statusFrame_->setObjectName(QString::fromUtf8("statusFrame_"));
	statusFrame_->setMinimumSize(QSize(0, 30));
	statusFrame_->setMaximumSize(QSize(16777215, 30));
	statusFrame_->setStyleSheet(QString::fromUtf8("QFrame#statusFrame {\n"
"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(81, 81, 81, 255), stop:0.494444 rgba(81, 81, 81, 255), stop:0.5 rgba(64, 64, 64, 255), stop:1 rgba(64, 64, 64, 255));\n"
"border-bottom: 1px solid black;\n"
"}"));
	horizontalLayout4_ = new QHBoxLayout();
	horizontalLayout4_->setObjectName(QString::fromUtf8("horizontalLayout_4"));
	horizontalLayout4_->setContentsMargins(-1, 6, -1, 6);
	statusFrameHorizontalSpacer_ = new QSpacerItem(478, 9, QSizePolicy::Expanding, QSizePolicy::Minimum);

	horizontalLayout4_->addItem(statusFrameHorizontalSpacer_);

	statusTextLabel_ = new QLabel();
	statusTextLabel_->setObjectName(QString::fromUtf8("statusTextLabel_"));
	statusTextLabel_->setStyleSheet(QString::fromUtf8("color: white;\n"
"font: bold 10pt \"Lucida Grande\""));

	horizontalLayout4_->addWidget(statusTextLabel_);
	statusFrame_->setLayout(horizontalLayout4_);


	verticalLayout3_->addWidget(statusFrame_);

	horizontalLayout_ = new QHBoxLayout();
	horizontalLayout_->setSpacing(0);
	horizontalLayout_->setObjectName(QString::fromUtf8("horizontalLayout_"));
	leftVerticalLayout_ = new QVBoxLayout();
	leftVerticalLayout_->setContentsMargins(12, 12, 12, 12);
	leftVerticalLayout_->setObjectName(QString::fromUtf8("leftVerticalLayout_"));
	leftHorizontalLayout_ = new QHBoxLayout();
	leftHorizontalLayout_->setSpacing(12);
	leftHorizontalLayout_->setObjectName(QString::fromUtf8("leftHorizontalLayout_"));
	openScansLayout_ = new QVBoxLayout();
	openScansLayout_->setSpacing(0);
	openScansLayout_->setObjectName(QString::fromUtf8("openScansLayout_"));
	openScansLayout_->setContentsMargins(-1, -1, 0, -1);
	openScansTitleButton_ = new QToolButton();
	openScansTitleButton_->setObjectName(QString::fromUtf8("openScansTitleButton_"));
	QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(openScansTitleButton_->sizePolicy().hasHeightForWidth());
	openScansTitleButton_->setSizePolicy(sizePolicy);
	openScansTitleButton_->setMaximumSize(QSize(16777215, 16));
	openScansTitleButton_->setStyleSheet(QString::fromUtf8("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(233, 233, 233, 255), stop:0.502513 rgba(199, 198, 198, 255), stop:1 rgba(163, 162, 162, 255)); border: 1px solid black; border-top-color: rgb(180, 180, 180); border-bottom-color: rgb(121, 121, 121); border-left-color: rgb(180, 180, 180); border-right-color: rgb(180, 180, 180);"));

	openScansLayout_->addWidget(openScansTitleButton_);

	scanListView_ = new QListView();
	scanListView_->setObjectName(QString::fromUtf8("scanListView_"));
	scanListView_->setMinimumSize(QSize(200, 0));

	openScansLayout_->addWidget(scanListView_);

	scanButtonLayout_ = new QHBoxLayout();
	scanButtonLayout_->setObjectName(QString::fromUtf8("scanButtonLayout_"));
	scanButtonLayout_->setContentsMargins(-1, 6, -1, -1);
	openScanButton_ = new QPushButton();
	openScanButton_->setObjectName(QString::fromUtf8("openScanButton_"));

	scanButtonLayout_->addWidget(openScanButton_);

	saveScanButton_ = new QPushButton();
	saveScanButton_->setObjectName(QString::fromUtf8("saveScanButton_"));

	scanButtonLayout_->addWidget(saveScanButton_);

	closeScanButton_ = new QPushButton();
	closeScanButton_->setObjectName(QString::fromUtf8("closeScanButton_"));

	scanButtonLayout_->addWidget(closeScanButton_);


	openScansLayout_->addLayout(scanButtonLayout_);


	leftHorizontalLayout_->addLayout(openScansLayout_);

	scanInformationLayout_ = new QVBoxLayout();
	scanInformationLayout_->setSpacing(0);
	scanInformationLayout_->setObjectName(QString::fromUtf8("scanInformationLayout_"));
	scanInformationLayout_->setContentsMargins(-1, -1, 0, -1);
	scanInformationTitleButton_ = new QToolButton();
	scanInformationTitleButton_->setObjectName(QString::fromUtf8("scanInformationTitleButton_"));
	QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(scanInformationTitleButton_->sizePolicy().hasHeightForWidth());
	scanInformationTitleButton_->setSizePolicy(sizePolicy1);
	scanInformationTitleButton_->setMaximumSize(QSize(16777215, 16));
	scanInformationTitleButton_->setStyleSheet(QString::fromUtf8("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(233, 233, 233, 255), stop:0.502513 rgba(199, 198, 198, 255), stop:1 rgba(163, 162, 162, 255)); border: 1px solid black; border-top-color: rgb(180, 180, 180); border-bottom-color: rgb(121, 121, 121); border-left-color: rgb(180, 180, 180); border-right-color: rgb(180, 180, 180);"));

	scanInformationLayout_->addWidget(scanInformationTitleButton_);

	scanInformationFrame_ = new QFrame();
	scanInformationFrame_->setObjectName(QString::fromUtf8("scanInformationFrame_"));
	scanInformationFrame_->setStyleSheet(QString::fromUtf8("background-color: white;\n"));
	scanInformationFrame_->setFrameShape(QFrame::StyledPanel);
	scanInformationFrame_->setFrameShadow(QFrame::Raised);
	scanInfoLayout = new QVBoxLayout();
	scanInfoLayout->setSpacing(6);
	scanInfoLayout->setObjectName(QString::fromUtf8("scanInfoLayout"));
	scanInfoLayout->setContentsMargins(-1, 6, -1, -1);
	scanInformationFrame_->setLayout(scanInfoLayout);
	scanInformationGridLayout_ = new QGridLayout();
	scanInformationGridLayout_->setObjectName(QString::fromUtf8("scanInformationGridLayout_"));
	scanInformationGridLayout_->setVerticalSpacing(4);
	scanInformationGridLayout_->setContentsMargins(12, -1, -1, -1);
	scanDate_ = new QLabel();
	scanDate_->setObjectName(QString::fromUtf8("scanDate_"));

	scanInformationGridLayout_->addWidget(scanDate_, 3, 1, 1, 1);

	scanTimeLabel_ = new QLabel();
	scanTimeLabel_->setObjectName(QString::fromUtf8("scanTimeLabel_"));
	scanTimeLabel_->setStyleSheet(QString::fromUtf8("color: rgb(121, 121, 121);\n"
"font-weight: bold;\n"
"font-family: \"Lucida Grande\";"));

	scanInformationGridLayout_->addWidget(scanTimeLabel_, 4, 0, 1, 1);

	scanDurationLabel_ = new QLabel();
	scanDurationLabel_->setObjectName(QString::fromUtf8("scanDurationLabel_"));
	scanDurationLabel_->setStyleSheet(QString::fromUtf8("color: rgb(121, 121, 121);\n"
"font-weight: bold;\n"
"font-family: \"Lucida Grande\";"));

	scanInformationGridLayout_->addWidget(scanDurationLabel_, 6, 0, 1, 1);

	scanRunLabel_ = new QLabel();
	scanRunLabel_->setObjectName(QString::fromUtf8("scanRunLabel_"));
	scanRunLabel_->setStyleSheet(QString::fromUtf8("color: rgb(121, 121, 121);\n"
"font-weight: bold;\n"
"font-family: \"Lucida Grande\";"));

	scanInformationGridLayout_->addWidget(scanRunLabel_, 7, 0, 1, 1);

	scanDateLabel_ = new QLabel();
	scanDateLabel_->setObjectName(QString::fromUtf8("scanDateLabel_"));
	scanDateLabel_->setStyleSheet(QString::fromUtf8("color: rgb(121, 121, 121);\n"
"font-weight: bold;\n"
"font-family: \"Lucida Grande\";"));

	scanInformationGridLayout_->addWidget(scanDateLabel_, 3, 0, 1, 1);

	scanDuration_ = new QLabel();
	scanDuration_->setObjectName(QString::fromUtf8("scanDuration_"));

	scanInformationGridLayout_->addWidget(scanDuration_, 6, 1, 1, 1);

	scanNameLabel_ = new QLabel();
	scanNameLabel_->setObjectName(QString::fromUtf8("scanNameLabel_"));
	scanNameLabel_->setStyleSheet(QString::fromUtf8("color: rgb(121, 121, 121);\n"
"font-weight: bold;\n"
"font-family: \"Lucida Grande\";"));

	scanInformationGridLayout_->addWidget(scanNameLabel_, 1, 0, 1, 1);

	scanNumberLabel_ = new QLabel();
	scanNumberLabel_->setObjectName(QString::fromUtf8("scanNumberLabel_"));
	scanNumberLabel_->setStyleSheet(QString::fromUtf8("color: rgb(121, 121, 121);\n"
"font-weight: bold;\n"
"font-family: \"Lucida Grande\";"));

	scanInformationGridLayout_->addWidget(scanNumberLabel_, 2, 0, 1, 1);

	scanNumber_ = new QSpinBox();
	scanNumber_->setObjectName(QString::fromUtf8("scanNumber_"));
	scanNumber_->setMaximumSize(QSize(16777215, 18));
	scanNumber_->setFrame(false);
	scanNumber_->setButtonSymbols(QAbstractSpinBox::NoButtons);
	scanNumber_->setMaximum(9999);

	scanInformationGridLayout_->addWidget(scanNumber_, 2, 1, 1, 1);

	scanIdLabel_ = new QLabel();
	scanIdLabel_->setObjectName(QString::fromUtf8("scanIdLabel_"));
	QPalette palette;
	QBrush brush(QColor(121, 121, 121, 255));
	brush.setStyle(Qt::SolidPattern);
	palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
	palette.setBrush(QPalette::Active, QPalette::Text, brush);
	palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
	palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
	palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
	palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
	QBrush brush1(QColor(159, 158, 158, 255));
	brush1.setStyle(Qt::SolidPattern);
	palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
	palette.setBrush(QPalette::Disabled, QPalette::Text, brush1);
	palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush1);
	scanIdLabel_->setPalette(palette);
	QFont font;
	font.setBold(true);
	font.setWeight(75);
	scanIdLabel_->setFont(font);

	scanInformationGridLayout_->addWidget(scanIdLabel_, 0, 0, 1, 1);

	scanId_ = new QLineEdit();
	scanId_->setObjectName(QString::fromUtf8("scanId_"));
	scanId_->setEnabled(false);
	scanId_->setFrame(false);

	scanInformationGridLayout_->addWidget(scanId_, 0, 1, 1, 1);

	scanTime_ = new QLabel();
	scanTime_->setObjectName(QString::fromUtf8("scanTime_"));

	scanInformationGridLayout_->addWidget(scanTime_, 4, 1, 1, 1);

	scanEnd_ = new QLabel();
	scanEnd_->setObjectName(QString::fromUtf8("scanEnd_"));

	scanInformationGridLayout_->addWidget(scanEnd_, 5, 1, 1, 1);

	scanEndLabel_ = new QLabel();
	scanEndLabel_->setObjectName(QString::fromUtf8("scanEndLabel_"));
	QPalette palette1;
	palette1.setBrush(QPalette::Active, QPalette::WindowText, brush);
	palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
	QBrush brush2(QColor(119, 119, 119, 255));
	brush2.setStyle(Qt::SolidPattern);
	palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
	scanEndLabel_->setPalette(palette1);
	QFont font1;
	font1.setFamily(QString::fromUtf8("Lucida"));
	font1.setPointSize(9);
	font1.setBold(true);
	font1.setWeight(75);
	scanEndLabel_->setFont(font1);

	scanInformationGridLayout_->addWidget(scanEndLabel_, 5, 0, 1, 1);


	scanInfoLayout->addLayout(scanInformationGridLayout_);

	scanInformationSpacer_ = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	scanInfoLayout->addItem(scanInformationSpacer_);


	scanInformationLayout_->addWidget(scanInformationFrame_);


	leftHorizontalLayout_->addLayout(scanInformationLayout_);

	notesLayout_ = new QVBoxLayout();
	notesLayout_->setSpacing(0);
	notesLayout_->setObjectName(QString::fromUtf8("notesLayout_"));
	notesLayout_->setContentsMargins(-1, -1, 0, -1);
	notesTitleButton_ = new QToolButton();
	notesTitleButton_->setObjectName(QString::fromUtf8("notesTitleButton_"));
	sizePolicy.setHeightForWidth(notesTitleButton_->sizePolicy().hasHeightForWidth());
	notesTitleButton_->setSizePolicy(sizePolicy);
	notesTitleButton_->setMaximumSize(QSize(16777215, 16));
	notesTitleButton_->setStyleSheet(QString::fromUtf8("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(233, 233, 233, 255), stop:0.502513 rgba(199, 198, 198, 255), stop:1 rgba(163, 162, 162, 255)); border: 1px solid black; border-top-color: rgb(180, 180, 180); border-bottom-color: rgb(121, 121, 121); border-left-color: rgb(180, 180, 180); border-right-color: rgb(180, 180, 180);\n"
""));

	notesLayout_->addWidget(notesTitleButton_);

	notesEdit_ = new QPlainTextEdit();
	notesEdit_->setObjectName(QString::fromUtf8("notesEdit_"));
	QSizePolicy sizePolicy2(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
	sizePolicy2.setHorizontalStretch(4);
	sizePolicy2.setVerticalStretch(0);
	sizePolicy2.setHeightForWidth(notesEdit_->sizePolicy().hasHeightForWidth());
	notesEdit_->setSizePolicy(sizePolicy2);
	notesEdit_->setStyleSheet(QString::fromUtf8("background-image: url(:/notepadBackground.png);\nfont: bold 15px \" Marker Felt\""));
	notesEdit_->setLineWidth(1);
	notesEdit_->setBackgroundVisible(false);

	notesLayout_->addWidget(notesEdit_);


	leftHorizontalLayout_->addLayout(notesLayout_);


	leftVerticalLayout_->addLayout(leftHorizontalLayout_);

	leftVerticaFrameSpacer_ = new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);

	leftVerticalLayout_->addItem(leftVerticaFrameSpacer_);


	horizontalLayout_->addLayout(leftVerticalLayout_);

	rightVerticalLayout_ = new QVBoxLayout();
	rightVerticalLayout_->setContentsMargins(12, 12, 12, 12);
	rightVerticalLayout_->setObjectName(QString::fromUtf8("rightVerticalLayout_"));

	horizontalLayout_->addLayout(rightVerticalLayout_);

	verticalLayout3_->addLayout(horizontalLayout_);

	setWindowTitle(QApplication::translate("AMGenericScanEditor", "Form", 0, QApplication::UnicodeUTF8));
	scanTitleIcon_->setText(QString());
	scanTitle_->setText(QApplication::translate("AMGenericScanEditor", "Scan Editor", 0, QApplication::UnicodeUTF8));
	statusTextLabel_->setText(QString());
	openScansTitleButton_->setText(QApplication::translate("AMGenericScanEditor", "Open Scans", 0, QApplication::UnicodeUTF8));
	openScanButton_->setText(QApplication::translate("AMGenericScanEditor", "Open...", 0, QApplication::UnicodeUTF8));
	saveScanButton_->setText(QApplication::translate("AMGenericScanEditor", "Save", 0, QApplication::UnicodeUTF8));
	closeScanButton_->setText(QApplication::translate("AMGenericScanEditor", "Close", 0, QApplication::UnicodeUTF8));
	scanInformationTitleButton_->setText(QApplication::translate("AMGenericScanEditor", "Scan Information", 0, QApplication::UnicodeUTF8));
	scanDate_->setText(QString());
	scanTimeLabel_->setText(QApplication::translate("AMGenericScanEditor", "start", 0, QApplication::UnicodeUTF8));
	scanDurationLabel_->setText(QApplication::translate("AMGenericScanEditor", "duration", 0, QApplication::UnicodeUTF8));
	scanRunLabel_->setText(QApplication::translate("AMGenericScanEditor", "run", 0, QApplication::UnicodeUTF8));
	scanDateLabel_->setText(QApplication::translate("AMGenericScanEditor", "date", 0, QApplication::UnicodeUTF8));
	scanDuration_->setText(QString());
	scanNameLabel_->setText(QApplication::translate("AMGenericScanEditor", "name", 0, QApplication::UnicodeUTF8));
	scanNumberLabel_->setText(QApplication::translate("AMGenericScanEditor", "number", 0, QApplication::UnicodeUTF8));
	scanIdLabel_->setText(QApplication::translate("AMGenericScanEditor", "serial #", 0, QApplication::UnicodeUTF8));
	scanTime_->setText(QString());
	scanEnd_->setText(QString());
	scanEndLabel_->setText(QApplication::translate("AMGenericScanEditor", "end", 0, QApplication::UnicodeUTF8));
	notesTitleButton_->setText(QApplication::translate("AMGenericScanEditor", "Notes", 0, QApplication::UnicodeUTF8));
	notesEdit_->setPlainText(QString());

	setLayout(verticalLayout3_);
}

void AMGenericScanEditor::refreshScanInfo()
{
	updateEditor(currentScan_);
}
