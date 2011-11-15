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


#include "AMDatamanAppController.h"

#include "util/AMSettings.h"
#include "acquaman.h"
#include "beamline/AMPVNames.h"

#include "dataman/database/AMDatabase.h"
#include "dataman/AMFirstTimeController.h"
#include "dataman/AMImportController.h"

#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterGeneralAscii.h"

#include "ui/AMMainWindow.h"
#include "ui/AMWorkflowManagerView.h"
#include "ui/AMBottomBar.h"
#include "ui/dataman/AMDataViewWithActionButtons.h"
#include "ui/dataman/AMRunExperimentInsert.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/util/AMSettingsView.h"

#include "application/AMPluginsManager.h"

#include "util/AMErrorMonitor.h"

#include <QMenuBar>
#include <QMessageBox>

#include "util/AMSettings.h"
#include "dataman/AMScan.h"

AMDatamanAppController::AMDatamanAppController(QObject *parent) :
	QObject(parent)
{
	isStarting_ = true;
	isShuttingDown_ = false;

	// shutdown is called automatically from the destructor if necessary, but Qt recommends that clean-up be handled in the aboutToQuit() signal. MS Windows doesn't always let the main function finish during logouts.
	// HOWEVER, we're not doing this for now, since this change could cause some existing applications to crash on shutdown, because they're not ready for events to be delivered during their shutdown process.
	// connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(shutdown()));
}

bool AMDatamanAppController::startup() {

	AMErrorMon::enableDebugNotifications(true);

	// Load settings from disk:
	AMSettings::s()->load();
	AMUserSettings::load();
	AMPVNames::load();
	// Load plugins:
	AMPluginsManager::s()->loadApplicationPlugins();

	// ensure user data folder and database are ready for use, if this is the first time the program is ever run.
	if(!AMFirstTimeController::firstTimeCheck())
		return false;
	//Create the main tab window:
	mw_ = new AMMainWindow();
	mw_->setWindowTitle("Acquaman");
	connect(mw_, SIGNAL(itemCloseButtonClicked(QModelIndex)), this, SLOT(onWindowPaneCloseButtonClicked(QModelIndex)));
	mw_->installEventFilter(this);

	bottomBar_ = new AMBottomBar();
	mw_->addBottomWidget(bottomBar_);
	connect(bottomBar_, SIGNAL(addButtonClicked()), this, SLOT(onAddButtonClicked()));


	// Create panes in the main window:
	////////////////////////////////////

	// A heading for the scan editors
	scanEditorsParentItem_ = mw_->windowPaneModel()->headingItem("Open Scans");


	// Make a dataview widget and add it under two links/headings: "Runs" and "Experiments". See AMMainWindowModel for more information.
	////////////////////////////////////
	dataView_ = new AMDataViewWithActionButtons();
	dataView_->setWindowTitle("Data");

	QStandardItem* dataViewItem = new QStandardItem();
	dataViewItem->setData(qVariantFromValue((QWidget*)dataView_), AM::WidgetRole);
	dataViewItem->setFlags(Qt::ItemIsEnabled);	// enabled, but should not be selectable
	QFont font = QFont("Lucida Grande", 10, QFont::Bold);
	font.setCapitalization(QFont::AllUppercase);
	dataViewItem->setFont(font);
	dataViewItem->setData(QBrush(QColor::fromRgb(100, 109, 125)), Qt::ForegroundRole);
	dataViewItem->setData(true, AMWindowPaneModel::DockStateRole);

	mw_->windowPaneModel()->appendRow(dataViewItem);

	runsParentItem_ = new QStandardItem(QIcon(":/22x22/view_calendar_upcoming_days.png"), "Runs");
	mw_->windowPaneModel()->initAliasItem(runsParentItem_, dataViewItem, "Runs", -1);
	dataViewItem->appendRow(runsParentItem_);
	experimentsParentItem_ = new QStandardItem(QIcon(":/applications-science.png"), "Experiments");
	mw_->windowPaneModel()->initAliasItem(experimentsParentItem_, dataViewItem, "Experiments", -1);
	dataViewItem->appendRow(experimentsParentItem_);

	// Hook into the sidebar and add Run and Experiment links below these headings.
	runExperimentInsert_ = new AMRunExperimentInsert(AMDatabase::database("user"), runsParentItem_, experimentsParentItem_, this);
	connect(runExperimentInsert_, SIGNAL(newExperimentAdded(QModelIndex)), this, SLOT(onNewExperimentAdded(QModelIndex)));

	// connect the activated signal from the dataview to our own slot
	connect(dataView_, SIGNAL(selectionActivated(QList<QUrl>)), this, SLOT(onDataViewItemsActivated(QList<QUrl>)));
	connect(dataView_, SIGNAL(selectionActivatedSeparateWindows(QList<QUrl>)), this, SLOT(onDataViewItemsActivatedSeparateWindows(QList<QUrl>)));
	connect(dataView_, SIGNAL(selectionExported(QList<QUrl>)), this, SLOT(onDataViewItemsExported(QList<QUrl>)));

	// When 'alias' links are clicked in the main window sidebar, we might need to notify some widgets of the details
	connect(mw_, SIGNAL(aliasItemActivated(QWidget*,QString,QVariant)), this, SLOT(onMainWindowAliasItemActivated(QWidget*,QString,QVariant)));
	/////////////////////////

	// Make connections:
	//////////////////////////////

	connect(mw_, SIGNAL(currentPaneChanged(QWidget*)), this, SLOT(onCurrentPaneChanged(QWidget*)));



	// Install exporters
	////////////////////////////////

	AMExportController::registerExporter<AMExporterGeneralAscii>();



	// make/install actions:
	/////////////////////////////////
	QAction* importAction = new QAction("Import to Library...", mw_);
	importAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_I));
	importAction->setStatusTip("Import outside data files into the library");
	connect(importAction, SIGNAL(triggered()), this, SLOT(onActionImport()));

	QAction* amSettingsAction = new QAction("Acquaman Settings", mw_);
	amSettingsAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_S));
	amSettingsAction->setStatusTip("View or Change Settings");
	connect(amSettingsAction, SIGNAL(triggered()), this, SLOT(onActionSettings()));

	settingsMasterView_ = 0; //NULL

	//install menu bar, and add actions
	//////////////////////////////////////
#ifdef Q_WS_MAC
	menuBar_ = new QMenuBar(0);
#else
	menuBar_ = new QMenuBar();
	mw_->addTopWidget(menuBar_);
#endif

	fileMenu_ = menuBar_->addMenu("File");
	fileMenu_->addAction(importAction);
	fileMenu_->addAction(amSettingsAction);

	// show main window
	mw_->show();

	isStarting_ = false;
	return true;

}


/// Program shutdown:
AMDatamanAppController::~AMDatamanAppController() {

	if(isRunning())
		shutdown();
}

void AMDatamanAppController::shutdown() {

	isShuttingDown_ = true;

	// destroy the main window. This will delete everything else within it.
	delete mw_;

	// Close down connection to the user Database
	AMDatabase::deleteDatabase("user");


}


void AMDatamanAppController::onActionImport() {

	new AMImportController();

}

void AMDatamanAppController::onActionSettings(){
	if(!settingsMasterView_)
		settingsMasterView_ = new AMSettingsMasterView();
	settingsMasterView_->show();
}

void AMDatamanAppController::onCurrentPaneChanged(QWidget *pane) {
	Q_UNUSED(pane)
}

void AMDatamanAppController::onMainWindowAliasItemActivated(QWidget *target, const QString &key, const QVariant &value) {

	if(target == dataView_) {
		if(key == "Runs")
			dataView_->dataView()->showRun(value.toInt());
		if(key == "Experiments")
			dataView_->dataView()->showExperiment(value.toInt());
	}
}

void AMDatamanAppController::onNewExperimentAdded(const QModelIndex &index) {
	mw_->sidebar()->expand(index.parent());
	mw_->sidebar()->setCurrentIndex(index);
	mw_->sidebar()->edit(index);
}



#include "dataman/AMExperiment.h"
void AMDatamanAppController::onAddButtonClicked() {

	// For now, we simply create a new experiment. Later on, this could pop up a menu to create a new experiment, run, sample plate, whatever...
	AMExperiment e("New Experiment");
	e.storeToDb(AMDatabase::database("user"));
}

void AMDatamanAppController::onProgressUpdated(double elapsed, double total){
	bottomBar_->updateScanProgress(elapsed, total);
}

#include "dataman/AMScanEditorModelItem.h"

void AMDatamanAppController::onDataViewItemsActivated(const QList<QUrl>& itemUrls) {

	dropScanURLs(itemUrls);
}

void AMDatamanAppController::onDataViewItemsActivatedSeparateWindows(const QList<QUrl> &itemUrls)
{
	dropScanURLs(itemUrls, 0, true);

	//	AMGenericScanEditor* editor;

	//	for(int i=0; i<itemUrls.count(); i++) {
	//		QList<QUrl> singleUrl;
	//		editor = createNewScanEditor();

	//		singleUrl << itemUrls.at(i);
	//		editor->dropScanURLs(singleUrl);
	//	}

	//	if(itemUrls.count())
	//		mw_->setCurrentPane(editor);
}

#include "dataman/AMRunExperimentItems.h"

void AMDatamanAppController::onWindowPaneCloseButtonClicked(const QModelIndex& index) {

	// is this a scan editor to be deleted?
	/////////////////////////////////
	if(mw_->windowPaneModel()->itemFromIndex(index.parent()) == scanEditorsParentItem_) {
		AMGenericScanEditor* editor = qobject_cast<AMGenericScanEditor*>(index.data(AM::WidgetRole).value<QWidget*>());
		closeScanEditor(editor);
	}

	// is this an experiment asking to be deleted?
	/// \todo bad code; improve this with better architecture and functionality in expItem.  Don't like trusting dynamic_cast; there's no guarantee that someone didn't put a non-AMExperimentModelItem into the model under experimentsParentItem_.
	else if(mw_->windowPaneModel()->itemFromIndex(index.parent()) == experimentsParentItem_) {

		AMExperimentModelItem* expItem = dynamic_cast<AMExperimentModelItem*>(mw_->windowPaneModel()->itemFromIndex(index));
		if(!expItem)
			return;

		AMExperiment experiment(expItem->id(), expItem->database());

		QMessageBox deleteConfirmation(mw_);
		deleteConfirmation.setText(QString("Are you sure you want to delete the experiment '%1'?").arg(experiment.name()));
		deleteConfirmation.setInformativeText("The scans in this experiment will remain, but they will no longer be associated with the experiment. You can't undo this action.");
		deleteConfirmation.setIcon(QMessageBox::Question);
		deleteConfirmation.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		deleteConfirmation.setDefaultButton(QMessageBox::Ok);
		deleteConfirmation.setEscapeButton(QMessageBox::Cancel);

		if(deleteConfirmation.exec() == QMessageBox::Ok) {
			AMExperiment::deleteExperiment(experiment.id(), expItem->database());
		}
	}
}

#include "dataman/export/AMExportController.h"
#include "ui/dataman/AMExportWizard.h"

void AMDatamanAppController::onDataViewItemsExported(const QList<QUrl> &itemUrls)
{
	// will delete itself when finished
	AMExportController* exportController = new AMExportController(itemUrls);
	AMExportWizard* wizard = new AMExportWizard(exportController);
	wizard->show();

	Q_UNUSED(exportController)
}

int AMDatamanAppController::scanEditorCount() const
{
	return scanEditorsParentItem_->rowCount();
}

AMGenericScanEditor * AMDatamanAppController::scanEditorAt(int index) const
{
	if(index<0 || index>=scanEditorCount())
		return 0;

	return qobject_cast<AMGenericScanEditor*>(scanEditorsParentItem_->child(index)->data(AM::WidgetRole).value<QWidget*>()); // seriously...
}

bool AMDatamanAppController::closeScanEditor(int index)
{
	return closeScanEditor(scanEditorAt(index));
}

bool AMDatamanAppController::closeScanEditor(AMGenericScanEditor* editor)
{
	if(editor && editor->canCloseEditor()) {
		mw_->deletePane(editor);
		return true;
	}
	else {
		return false;
	}
}

AMGenericScanEditor * AMDatamanAppController::createNewScanEditor()
{
	AMGenericScanEditor* editor = new AMGenericScanEditor();
	scanEditorsParentItem_->appendRow(new AMScanEditorModelItem(editor, this, ":/applications-science.png"));
	return editor;
}

bool AMDatamanAppController::canCloseScanEditors() const
{
	//	bool canCloseEditors = true;
	//	for(int i=0, count = scanEditorCount(); i<count; i++) {
	//		AMGenericScanEditor* editor = scanEditorAt(i);
	//		if(editor) canCloseEditors &= editor->canCloseEditor();
	//	}
	//	return canCloseEditors;

	// Do we need to check all, or is it okay to stop as soon as we find one that doesn't allow closing?
	for(int i=0, count = scanEditorCount(); i<count; i++) {
		AMGenericScanEditor* editor = scanEditorAt(i);
		if(editor) {
			mw_->setCurrentPane(editor);
			if(!editor->canCloseEditor())
				return false;
		}
	}
	return true;
}

//void AMDatamanAppController::processEventsFor(int ms)
//{
//	QTime t;
//	t.start();
//	while(t.elapsed() <ms) {
//		qApp->sendPostedEvents();
//		qApp->processEvents();
//	}
//}

bool AMDatamanAppController::eventFilter(QObject* o, QEvent* e)
{
	if(o == mw_ && e->type() == QEvent::Close) {
		if(!canCloseScanEditors()) {
			e->ignore();
			return true;
		}
		else {
			// They got away with closing the main window. We should quit the application
			qApp->quit();	//note that this might already be in progress, if an application quit was what triggered this close event.  No harm in asking twice...
		}
	}
	// anything else, allow unfiltered
	return QObject::eventFilter(o,e);
}

AMGenericScanEditor * AMDatamanAppController::isScanOpenForEditing(int id, AMDatabase *db)
{
	for(int i=0, editorCount=scanEditorCount(); i<editorCount; i++) {
		AMGenericScanEditor* editor = scanEditorAt(i);
		if(editor) {
			for(int j=0, scanCount=editor->scanCount(); j<scanCount; j++) {
				AMScan* scan = editor->scanAt(j);
				if(scan->id() == id && scan->database() == db)
					return editor;
			}
		}
	}
	return 0;
}

bool AMDatamanAppController::dropScanURLs(const QList<QUrl> &urls, AMGenericScanEditor *editor, bool openInIndividualEditors)
{
	if(	!urls.count() )
		return false;

	bool accepted = false;
	bool madeNewEditor = false;

	// where are they going?
	if(openInIndividualEditors) {
		editor = 0;	// create new one for each
	}
	else {
		if(!editor) {
			editor = createNewScanEditor();
			madeNewEditor = true;
		}
	}

	// loop through all URLs
	foreach(QUrl url, urls) {
		if(dropScanURL(url, editor))
			accepted = true;
	}

	if(madeNewEditor) {	// if we made a single new editor for these...
		if(accepted) {
			mw_->setCurrentPane(editor);
		}
		else {
			mw_->deletePane(editor);	// no need for this guy; he's empty.
		}
	}

	return accepted;
}

#include "dataman/database/AMDbObjectSupport.h"

bool AMDatamanAppController::dropScanURL(const QUrl &url, AMGenericScanEditor *editor)
{
	// scheme correct?
	if(url.scheme() != "amd") {
		return false;
	}

	// Can we connect to the database?
	AMDatabase* db = AMDatabase::database(url.host());
	if(!db)
		return false;
	// \bug This does not verify that the incoming scans came from the user database. In fact, it happily accepts scans from other databases. Check if we assume anywhere inside AMGenericScanEditor that we're using the AMDatabase::database("user") database. (If we do, this could cause problems when multiple databases exist.)

	QStringList path = url.path().split('/', QString::SkipEmptyParts);
	if(path.count() != 2)
		return false;

	QString tableName = path.at(0);
	bool idOkay;
	int id = path.at(1).toInt(&idOkay);
	if(!idOkay || id < 1)
		return false;

	// Only open scans for now (ie: things in the scans table)
	if(tableName != AMDbObjectSupport::s()->tableNameForClass<AMScan>())
		return false;

	// Check if this scan is scanning... Use the currentlyScanning column stored in the database.
	QVariant isScanning = db->retrieve(id, tableName, "currentlyScanning");
	if(!isScanning.isValid())
		return false;
	if(isScanning.toBool()) {
		QList<QVariant> nameAndNumber = db->retrieve(id, tableName, QStringList() << "name" << "number");
		QMessageBox stillScanningEnquiry;
		stillScanningEnquiry.setWindowTitle("This scan is still acquiring.");
		stillScanningEnquiry.setText(QString("The scan '%1' (#%2) is currently still acquiring data, so you can't open multiple copies of it yet.")
									 .arg(nameAndNumber.at(0).toString())
									 .arg(nameAndNumber.at(1).toString()));
		stillScanningEnquiry.setIcon(QMessageBox::Question);
		QPushButton* showMeNowButton = stillScanningEnquiry.addButton("Show me the scan", QMessageBox::AcceptRole);
		stillScanningEnquiry.addButton(QMessageBox::Cancel);
		stillScanningEnquiry.setDefaultButton(showMeNowButton);
		stillScanningEnquiry.exec();
		if(stillScanningEnquiry.clickedButton() == showMeNowButton) {
			AMGenericScanEditor* otherEditor = isScanOpenForEditing(id, db);
			if(otherEditor)
				mw_->setCurrentPane(otherEditor);
			/// \todo What if we don't find it? What if a program crashed and left the currentlyScanning column set in the database? Should we have a way to override this?
		}
		return false;
	}

	// Check if this scan is already open
	AMGenericScanEditor* otherEditor = isScanOpenForEditing(id, db);
	if(otherEditor) {
		QList<QVariant> nameAndNumber = db->retrieve(id, tableName, QStringList() << "name" << "number");
		QMessageBox alreadyOpenEnquiry;
		alreadyOpenEnquiry.setWindowTitle("This scan is already open.");
		alreadyOpenEnquiry.setText(QString("A copy of the scan '%1' (#%2) is already open. Are you sure you want to open multiple copies of it?")
								   .arg(nameAndNumber.at(0).toString())
								   .arg(nameAndNumber.at(1).toString()));
		alreadyOpenEnquiry.setInformativeText("If you open multiple copies of a scan, changes that you make in one view will not show up in the other view. Therefore, you should be careful to only save the version you want to keep.");
		alreadyOpenEnquiry.setIcon(QMessageBox::Question);
		QPushButton* showMeNowButton = alreadyOpenEnquiry.addButton("Show me the open scan", QMessageBox::NoRole);
		QPushButton* openAgainButton = alreadyOpenEnquiry.addButton("Open another copy", QMessageBox::YesRole);
		alreadyOpenEnquiry.addButton(QMessageBox::Cancel);
		alreadyOpenEnquiry.setDefaultButton(showMeNowButton);
		alreadyOpenEnquiry.exec();
		if(alreadyOpenEnquiry.clickedButton() == openAgainButton) {
			// do nothing... we'll proceed
		}
		else if(alreadyOpenEnquiry.clickedButton() == showMeNowButton) {
			mw_->setCurrentPane(otherEditor);
			return false;
		}
		else {
			// user chose to cancel.
			return false;
		}
	}

	// Dynamically create and load a detailed subclass of AMDbObject from the database... whatever type it is.
	AMDbObject* dbo = AMDbObjectSupport::s()->createAndLoadObjectAt(db, tableName, id);
	if(!dbo)
		return false;

	// Is it a scan?
	AMScan* scan = qobject_cast<AMScan*>( dbo );
	if(!scan) {
		delete dbo;
		return false;
	}

	// success!
	if(!editor) {
		editor = createNewScanEditor();
	}
	editor->addScan(scan);
	return true;
}






