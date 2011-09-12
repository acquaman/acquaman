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

#include "dataman/AMDatabase.h"
#include "dataman/AMFirstTimeController.h"
#include "dataman/AMImportController.h"

#include "dataman/AMExportController.h"
#include "dataman/AMExporterGeneralAscii.h"

#include "ui/AMMainWindow.h"
#include "ui/AMWorkflowManagerView.h"
#include "ui/BottomBar.h"
#include "ui/AMDataViewWithActionButtons.h"
#include "ui/AMRunExperimentInsert.h"
#include "ui/AMGenericScanEditor.h"

#include "util/AMErrorMonitor.h"

#include <QMenuBar>
#include <QMessageBox>

AMDatamanAppController::AMDatamanAppController(QObject *parent) :
	QObject(parent)
{
	isStarting_ = true;
	isShuttingDown_ = false;
}

bool AMDatamanAppController::startup() {

	AMErrorMon::enableDebugNotifications(true);

	// Load settings from disk:
	AMSettings::load();
	AMUserSettings::load();
	AMPVNames::load();

	// ensure user data folder and database are ready for use, if this is the first time the program is ever run.
	if(!AMFirstTimeController::firstTimeCheck())
		return false;

	//Create the main tab window:
	mw_ = new AMMainWindow();
	mw_->setWindowTitle("Acquaman");
	connect(mw_, SIGNAL(itemCloseButtonClicked(QModelIndex)), this, SLOT(onWindowPaneCloseButtonClicked(QModelIndex)));

	bottomBar_ = new BottomBar();
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
	runExperimentInsert_ = new AMRunExperimentInsert(AMDatabase::userdb(), runsParentItem_, experimentsParentItem_, this);
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
	AMDatabase::releaseUserDb();


}


void AMDatamanAppController::onActionImport() {

	new AMImportController();

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
	e.storeToDb(AMDatabase::userdb());
}

void AMDatamanAppController::onProgressUpdated(double elapsed, double total){
	bottomBar_->updateScanProgress(elapsed, total);
}

#include "dataman/AMScanEditorModelItem.h"

void AMDatamanAppController::onDataViewItemsActivated(const QList<QUrl>& itemUrls) {

	if(itemUrls.count() == 0)
		return;

	AMGenericScanEditor* editor = new AMGenericScanEditor();
	scanEditorsParentItem_->appendRow(new AMScanEditorModelItem(editor, ":/applications-science.png"));
	mw_->setCurrentPane(editor);

	editor->dropScanURLs(itemUrls);
}

void AMDatamanAppController::onDataViewItemsActivatedSeparateWindows(const QList<QUrl> &itemUrls)
{
	AMGenericScanEditor* editor;

	for(int i=0; i<itemUrls.count(); i++) {
		QList<QUrl> singleUrl;
		editor = new AMGenericScanEditor();
		scanEditorsParentItem_->appendRow(new AMScanEditorModelItem(editor, ":/applications-science.png"));

		singleUrl << itemUrls.at(i);
		editor->dropScanURLs(singleUrl);
	}

	if(itemUrls.count())
		mw_->setCurrentPane(editor);
}

#include "dataman/AMRunExperimentItems.h"

void AMDatamanAppController::onWindowPaneCloseButtonClicked(const QModelIndex& index) {

	// is this a scan editor to be deleted?
	/////////////////////////////////
	if(mw_->windowPaneModel()->itemFromIndex(index.parent()) == scanEditorsParentItem_) {
		AMGenericScanEditor* editor = qobject_cast<AMGenericScanEditor*>(index.data(AM::WidgetRole).value<QWidget*>());

		if(!editor)
			return;

		// delete all scans in the editor, and ask the user for confirmation. If they 'cancel' on any, give up here and don't close the window.
		while(editor->scanCount()) {
			if(!editor->deleteScanWithModifiedCheck(editor->scanAt(editor->scanCount()-1)))
				return;
		}
		mw_->deletePane(editor);
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

#include "dataman/AMExportController.h"
#include "ui/AMExportWizard.h"

void AMDatamanAppController::onDataViewItemsExported(const QList<QUrl> &itemUrls)
{
	// will delete itself when finished
	AMExportController* exportController = new AMExportController(itemUrls);
	AMExportWizard* wizard = new AMExportWizard(exportController);
	wizard->show();

	Q_UNUSED(exportController)
}
