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


#include "AMAppController.h"

#include "ui/AMWorkflowManagerView.h"
#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "dataman/export/AMExporter.h"
#include "dataman/export/AMExporterOption.h"
#include "ui/AMStartScreen.h"

AMAppController::AMAppController(QObject *parent)
	: AMDatamanAppController(parent)
{
}

bool AMAppController::startupCreateUserInterface() {

	if (AMDatamanAppController::startupCreateUserInterface()){
		// a heading for the workflow manager...
		workflowManagerView_ = new AMWorkflowManagerView();
		mw_->insertHeading("Experiment Tools", 1);
		mw_->addPane(workflowManagerView_, "Experiment Tools", "Workflow", ":/user-away.png");

		AMStartScreen* chooseRunDialog = new AMStartScreen(true, mw_);
		chooseRunDialog->show();

		return true;
	}

	return false;
}




void AMAppController::goToWorkflow() {
	mw_->setCurrentPane(workflowManagerView_);
}

#include "dataman/AMScan.h"

void AMAppController::openScanInEditorAndTakeOwnership(AMScan *scan, bool bringEditorToFront, bool openInExistingEditor)
{
	AMGenericScanEditor* editor;

	if(openInExistingEditor && scanEditorCount()) {
		editor = scanEditorAt(scanEditorCount()-1);
	}
	else {

		if (scan->scanRank() == 2)
			editor = createNewScanEditor(true);
		else
			editor = createNewScanEditor();
	}

	editor->addScan(scan);

	if(bringEditorToFront)
		mw_->setCurrentPane(editor);
}

#include "acquaman/AMScanConfiguration.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/acquaman/AMScanConfigurationViewHolder.h"
#include "dataman/database/AMDatabase.h"
#include "dataman/database/AMDbObjectSupport.h"

void AMAppController::launchScanConfigurationFromDb(const QUrl &url)
{
	// scheme correct?
	if (url.scheme() != "amd")
		return;

	// Scan configurations only come from the user databases currently.
	AMDatabase *db = AMDatabase::database("user");
	if (!db)
		return;

	QStringList path = url.path().split('/', QString::SkipEmptyParts);
	if(path.count() != 2)
		return;

	QString tableName = path.at(0);
	bool idOkay;
	int id = path.at(1).toInt(&idOkay);
	if(!idOkay || id < 1)
		return;

	// Only open scans for now (ie: things in the scans table)
	if(tableName != AMDbObjectSupport::s()->tableNameForClass<AMScan>())
		return;

	// turn off automatic raw-day loading for scans... This will make loading the scan to access it's config much faster.
	bool scanAutoLoadingOn = AMScan::autoLoadData();
	AMScan::setAutoLoadData(false);
	// Dynamically create and load a detailed subclass of AMDbObject from the database... whatever type it is.
	AMDbObject* dbo = AMDbObjectSupport::s()->createAndLoadObjectAt(db, tableName, id);
	if(!dbo)
		return;
	// restore AMScan's auto-loading of data to whatever it was before.
	AMScan::setAutoLoadData(scanAutoLoadingOn);

	// Is it a scan?
	AMScan* scan = qobject_cast<AMScan*>( dbo );
	if(!scan) {
		delete dbo;
		return;
	}

	// need to check that this scan actually has a valid config. This hasn't always been guaranteed, especially when scans move between beamlines.
	AMScanConfiguration* config = scan->scanConfiguration();
	if(!config) {
		scan->release();
		return;
	}
	// need to create a copy of the config so we can delete the scan (and hence the config instance owned by the scan). The view will take ownership of the copy.
	config = config->createCopy();
	scan->release();
	if(!config)
		return;

	AMScanConfigurationView *view = config->createView();
	if(!view) {
		delete config;
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -401, "Unable to create view from the scan configuration loaded from the database.  Contact Acquaman developers."));
		return;
	}

	AMScanConfigurationViewHolder *viewHolder = new AMScanConfigurationViewHolder( workflowManagerView_, view);
	viewHolder->setAttribute(Qt::WA_DeleteOnClose, true);
	viewHolder->show();
}


bool AMAppController::eventFilter(QObject* o, QEvent* e)
{
	if(o == mw_ && e->type() == QEvent::Close) {
		if(!canCloseScanEditors()) {
			e->ignore();
			return true;
		}
		if(!canCloseActionRunner()) {
			e->ignore();
			return true;
		}
		// They got away with closing the main window. We should quit the application
		qApp->quit();	//note that this might already be in progress, if an application quit was what triggered this close event.  No harm in asking twice...

	}
	// anything else, allow unfiltered
	return QObject::eventFilter(o,e);
}

#include <QMessageBox>
#include "actions2/AMActionRunner.h"
#include "actions2/AMAction.h"
bool AMAppController::canCloseActionRunner()
{
	AMActionRunner::s()->setQueuePaused(true);

	// is there an action running in the primary queue?
	if(AMActionRunner::s()->actionRunning()) {

		int doCancel = QMessageBox::question(mw_,
											  "An action is still running. Are you sure you want to quit?",
											  QString("There is an action (%1) still running.  Do you want to stop it?").arg(AMActionRunner::s()->currentAction()->info()->shortDescription()),
											  QMessageBox::Yes, QMessageBox::No);

		// Careful: it might have finished while the user was answering the question.
		if(AMActionRunner::s()->actionRunning()) {
			if(doCancel == QMessageBox::Yes)
				AMActionRunner::s()->cancelCurrentAction();
			return false;
		}
	}

	// any actions running in the background in immediate mode?
	if(AMActionRunner::s()->immediateActionsCount()) {
		QStringList descriptions;
		for(int i=0, cc=AMActionRunner::s()->immediateActionsCount(); i<cc; i++) {
			descriptions << AMActionRunner::s()->immediateActionAt(i)->info()->shortDescription();
		}
		int doCancel = QMessageBox::question(mw_,
											  "Actions are still running in the background. Are you sure you want to quit?",
											  QString("There are still actions running in the background (%1). Do you want to stop them?").arg(descriptions.join(", ")),
											  QMessageBox::Yes, QMessageBox::No);

		// careful: they might have finished while the user was answering.
		if(AMActionRunner::s()->immediateActionsCount()) {
			if(doCancel == QMessageBox::Yes)
				AMActionRunner::s()->cancelImmediateActions();
			return false;
		}
	}


	// No objections. Can quit.
	return true;
}

void AMAppController::showChooseRunDialog()
{
	AMStartScreen* d = new AMStartScreen(false, mw_);
	d->show();
}

#include <QMenu>
bool AMAppController::startupInstallActions()
{
	if(AMDatamanAppController::startupInstallActions()) {

		QAction* changeRunAction = new QAction("Change Run...", mw_);
		// changeRunAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_B));
		changeRunAction->setStatusTip("Change the current run, or create a new one");
		connect(changeRunAction, SIGNAL(triggered()), this, SLOT(showChooseRunDialog()));

		fileMenu_->addAction(changeRunAction);
		return true;
	}
	else
		return false;
}

