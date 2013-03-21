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


#include <QStringBuilder>

#include "AMAppController.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "ui/AMWorkflowManagerView.h"
#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "dataman/export/AMExporter.h"
#include "dataman/export/AMExporterOption.h"
#include "ui/AMStartScreen.h"

#include "ui/actions3/AMWorkflowView3.h"
#include "ui/AMAppBottomPanel.h"
#include "actions3/AMActionRunner3.h"
#include "actions3/AMActionRegistry3.h"
#include "actions3/AMLoopAction3.h"
#include "actions3/editors/AMLoopActionEditor3.h"
#include "actions3/editors/AMListActionEditor3.h"
#include "actions3/actions/AMNumberChangeAction.h"
#include "actions3/editors/AMNumberChangeActionEditor.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/editors/AMControlMoveActionEditor3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/actions/AMScanActionInfo.h"
#include "actions3/editors/AMScanActionEditor.h"
#include "actions3/actions/AMSamplePlateMoveAction.h"
#include "actions3/actions/AMSamplePlateMoveActionInfo.h"
#include "actions3/editors/AMSamplePlateMoveActionEditor.h"

AMAppController::AMAppController(QObject *parent)
	: AMDatamanAppControllerForActions3(parent)
{
}

bool AMAppController::startup(){

	/* Commented out, put it back in to play with the change number action
	AMNumberChangeActionSupport::appendNumber(12);
	AMNumberChangeActionSupport::appendNumber(27);
	AMNumberChangeActionSupport::appendNumber(100);
	AMNumberChangeActionSupport::appendNumber(1000);
	AMNumberChangeActionSupport::appendNumber(0);
	AMNumberChangeActionSupport::appendNumber(15);
	AMNumberChangeActionSupport::appendNumber(8888);
	AMNumberChangeActionSupport::appendNumber(42);
	AMNumberChangeActionSupport::appendNumber(99);
	AMNumberChangeActionSupport::appendNumber(1);
	*/

	if(AMDatamanAppControllerForActions3::startup()){
		bool success = true;
		/* Commented out, put it back in to play with the change number action
		success &= AMActionRegistry3::s()->registerInfoAndAction<AMNumberChangeActionInfo, AMNumberChangeAction>("Number Change", "Changes a number in the list", ":/system-run.png");
		success &= AMActionRegistry3::s()->registerInfoAndEditor<AMNumberChangeActionInfo, AMNumberChangeActionEditor>();
		*/
		success &= AMActionRegistry3::s()->registerInfoAndAction<AMLoopActionInfo3, AMLoopAction3>("Loop", "This action repeats a set of sub-actions a specific number of times.\n\nAfter adding it, you can drag-and-drop other actions inside it.", ":/32x32/media-playlist-repeat.png");
		success &= AMActionRegistry3::s()->registerInfoAndEditor<AMLoopActionInfo3, AMLoopActionEditor3>();

		success &= AMActionRegistry3::s()->registerInfoAndAction<AMSequentialListActionInfo3, AMSequentialListAction3>("Sequential\nList", "This action runs a sequential list of other actions", ":/22x22/viewListInv-22x22.png");
		success &= AMActionRegistry3::s()->registerInfoAndAction<AMParallelListActionInfo3, AMParallelListAction3>("Parallel\nList", "This action runs a parallel list of other actions.", ":/22x22/viewDetaillInv-22x22.png");
		success &= AMActionRegistry3::s()->registerInfoAndEditor<AMListActionInfo3, AMListActionEditor3>();
		success &= AMActionRegistry3::s()->registerInfoAndAction<AMControlMoveActionInfo3, AMControlMoveAction3>("Control Move", "Moves a control to an absolute position or a relative position from its current state.", ":system-run.png");
		success &= AMActionRegistry3::s()->registerInfoAndEditor<AMControlMoveActionInfo3, AMControlMoveActionEditor3>();

		success &= AMActionRegistry3::s()->registerInfoAndAction<AMScanActionInfo, AMScanAction>("Scan Action", "Runs a scan.", ":/spectrum.png", false);
		success &= AMActionRegistry3::s()->registerInfoAndEditor<AMScanActionInfo, AMScanActionEditor>();

		success &= AMActionRegistry3::s()->registerInfoAndAction<AMSamplePlateMoveActionInfo, AMSamplePlateMoveAction>("Move Sample Position", "Move to a different marked sample position", ":system-run.png");
		success &= AMActionRegistry3::s()->registerInfoAndEditor<AMSamplePlateMoveActionInfo, AMSamplePlateMoveActionEditor>();

		return success;
	}
	else
		return false;
}

bool AMAppController::startupCreateUserInterface() {

	if (AMDatamanAppControllerForActions3::startupCreateUserInterface()){
		// a heading for the workflow manager...
//		workflowManagerView_ = new AMWorkflowManagerView();
//		mw_->insertHeading("Experiment Tools", 1);
//		mw_->addPane(workflowManagerView_, "Experiment Tools", "WorkflowOld", ":/user-away.png");

		// add the workflow control UI
		//workflowView_ = new AMWorkflowView3();
		workflowView_ = new AMWorkflowView3(AMActionRunner3::workflow());
		mw_->insertHeading("Experiment Tools", 1);
		mw_->addPane(workflowView_, "Experiment Tools", "Workflow", ":/user-away.png");

		scanActionRunnerView_ = new AMWorkflowView3(AMActionRunner3::scanActionRunner());
		mw_->addPane(scanActionRunnerView_, "Experiment Tools", "ScanActions", ":/user-away.png");

		// remove the old one:
//		mw_->removePane(workflowManagerView_);
//		workflowManagerView_->hide();

		// get the "open scans" section to be under the workflow
		mw_->windowPaneModel()->removeRow(scanEditorsParentItem_->row());
		scanEditorsParentItem_ = mw_->windowPaneModel()->headingItem("Open Scans", QModelIndex(), mw_->windowPaneModel()->rowCount()-1);

		AMStartScreen* chooseRunDialog = new AMStartScreen(true, mw_);
		chooseRunDialog->show();
		chooseRunDialog->activateWindow();
		chooseRunDialog->raise();

		/* Commented out, put it back in to play with the change number action
		QListView *listView = new QListView();
		listView->setModel(AMNumberChangeActionSupport::AMNumberChangeActionModel_);
		listView->show();
		*/

		return true;
	}

	return false;
}

void AMAppController::addBottomPanel()
{
	AMAppBottomPanel *panel = new AMAppBottomPanel(AMActionRunner3::workflow());
	mw_->addBottomWidget(panel);
	connect(panel, SIGNAL(addExperimentButtonClicked()), this, SLOT(onAddButtonClicked()));
	bottomPanel_ = panel;
}

void AMAppController::goToWorkflow() {
	// This check can be removed when all the old workflow stuff is finally removed
	if(mw_->windowPaneModel()->allPanes().contains(workflowManagerView_))
		mw_->setCurrentPane(workflowManagerView_);
	else
		mw_->setCurrentPane(workflowView_);
}

#include "dataman/AMScan.h"

void AMAppController::openScanInEditor(AMScan *scan, bool bringEditorToFront, bool openInExistingEditor)
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
#include "acquaman/AM2DScanConfiguration.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/acquaman/AMScanConfigurationViewHolder.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/acquaman/AM2DScanConfigurationViewHolder.h"
#include "dataman/database/AMDatabase.h"
#include "dataman/database/AMDbObjectSupport.h"

void AMAppController::launchScanConfigurationFromDb(const QUrl &url)
{
	// turn off automatic raw-day loading for scans... This will make loading the scan to access it's config much faster.
	bool scanAutoLoadingOn = AMScan::autoLoadData();
	AMScan::setAutoLoadData(false);

	AMScan* scan = AMScan::createFromDatabaseUrl(url, true);

	// restore AMScan's auto-loading of data to whatever it was before.
	AMScan::setAutoLoadData(scanAutoLoadingOn);

	if(!scan)
		return;


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

	// Check if this is a regular scan configuration or a 2D one.
	bool is2D_ = false;

	if (qobject_cast<AM2DScanConfiguration *>(config))
		is2D_ = true;

	AMScanConfigurationView *view = config->createView();
	if(!view) {
		delete config;
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -401, "Unable to create view from the scan configuration loaded from the database.  Contact Acquaman developers."));
		return;
	}

	// This is Actions3 stuff.
	AMScanConfigurationViewHolder3 *viewHolder = new AMScanConfigurationViewHolder3(view);
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
#include "actions3/AMActionRunner3.h"
#include "actions3/AMAction3.h"
bool AMAppController::canCloseActionRunner()
{
	AMActionRunner3::workflow()->setQueuePaused(true);

	// is there an action running in the primary queue?
	if(AMActionRunner3::workflow()->actionRunning()) {

		int doCancel = QMessageBox::question(mw_,
											  "An action is still running. Are you sure you want to quit?",
											  QString("There is an action (%1) still running.  Do you want to stop it?").arg(AMActionRunner3::workflow()->currentAction()->info()->shortDescription()),
											  QMessageBox::Yes, QMessageBox::No);

		// Careful: it might have finished while the user was answering the question.
		if(AMActionRunner3::workflow()->actionRunning()) {
			if(doCancel == QMessageBox::Yes)
				AMActionRunner3::workflow()->cancelCurrentAction();
			return false;
		}
	}

	// any actions running in the background in immediate mode?
	if(AMActionRunner3::workflow()->immediateActionsCount()) {
		QStringList descriptions;
		for(int i=0, cc=AMActionRunner3::workflow()->immediateActionsCount(); i<cc; i++) {
			descriptions << AMActionRunner3::workflow()->immediateActionAt(i)->info()->shortDescription();
		}
		int doCancel = QMessageBox::question(mw_,
											  "Actions are still running in the background. Are you sure you want to quit?",
											  QString("There are still actions running in the background (%1). Do you want to stop them?").arg(descriptions.join(", ")),
											  QMessageBox::Yes, QMessageBox::No);

		// careful: they might have finished while the user was answering.
		if(AMActionRunner3::workflow()->immediateActionsCount()) {
			if(doCancel == QMessageBox::Yes)
				AMActionRunner3::workflow()->cancelImmediateActions();
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
	if(AMDatamanAppControllerForActions3::startupInstallActions()) {

		QAction* changeRunAction = new QAction("Change Run...", mw_);
		// changeRunAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_B));
		changeRunAction->setStatusTip("Change the current run, or create a new one");
		connect(changeRunAction, SIGNAL(triggered()), this, SLOT(showChooseRunDialog()));

		fileMenu_->addSeparator();
		fileMenu_->addAction(changeRunAction);
		return true;
	}
	else
		return false;
}

