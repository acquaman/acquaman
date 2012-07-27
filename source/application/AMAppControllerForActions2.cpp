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

#include "AMAppControllerForActions2.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "dataman/export/AMExporter.h"
#include "dataman/export/AMExporterOption.h"
#include "ui/AMStartScreen.h"

#include "ui/actions2/AMWorkflowView.h"
#include "actions2/AMActionRunner.h"
#include "actions2/AMActionRegistry.h"
#include "actions2/AMLoopAction.h"

#include "actions2/actions/AMWaitAction.h"
#include "actions2/actions/AMScanControllerAction.h"
#include "actions2/actions/AMInternalControlMoveAction.h"

#include "actions2/editors/AMWaitActionEditor.h"
#include "actions2/actions/AMChangeRunAction.h"
#include "actions2/editors/AMChangeRunActionEditor.h"
#include "actions2/editors/AMLoopActionEditor.h"


AMAppControllerForActions2::AMAppControllerForActions2(QObject *parent)
	: AMDatamanAppControllerForActions2(parent)
{
}

bool AMAppControllerForActions2::startupCreateUserInterface() {

	if (AMDatamanAppControllerForActions2::startupCreateUserInterface()){
		// add the workflow control UI
		mw_->insertHeading("Experiment Tools", 1);
		workflowView_ = new AMWorkflowView();
		mw_->addPane(workflowView_, "Experiment Tools", "Workflow", ":/user-away.png");

		AMStartScreen* chooseRunDialog = new AMStartScreen(true, mw_);
		chooseRunDialog->show();
		chooseRunDialog->activateWindow();
		chooseRunDialog->raise();

		return true;
	}

	return false;
}




void AMAppControllerForActions2::goToWorkflow() {
	mw_->setCurrentPane(workflowView_);
}

#include "dataman/AMScan.h"

void AMAppControllerForActions2::openScanInEditor(AMScan *scan, bool bringEditorToFront, bool openInExistingEditor)
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

bool AMAppControllerForActions2::eventFilter(QObject* o, QEvent* e)
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
bool AMAppControllerForActions2::canCloseActionRunner()
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

void AMAppControllerForActions2::showChooseRunDialog()
{
	AMStartScreen* d = new AMStartScreen(false, mw_);
	d->show();
}

#include <QMenu>
bool AMAppControllerForActions2::startupInstallActions()
{
	if(AMDatamanAppControllerForActions2::startupInstallActions()) {

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

bool AMAppControllerForActions2::startupAfterEverything()
{
	if(!AMDatamanAppControllerForActions2::startupAfterEverything())
		return false;

	connect(AMActionRunner::s(), SIGNAL(currentActionStateChanged(int,int)), this, SLOT(onCurrentActionStateChanged(int,int)));
	return true;
}

#include "acquaman/AMScanController.h"

void AMAppControllerForActions2::onCurrentActionStateChanged(int newState, int oldState)
{
	Q_UNUSED(oldState)

	if(newState == AMAction::Running) {
		AMAction* action = AMActionRunner::s()->currentAction();

		AMScanControllerAction* scanAction = qobject_cast<AMScanControllerAction*>(action);
		if(scanAction) {
			AMScanController* scanController = scanAction->scanController();
			if(scanController) {
				openScanInEditor(scanController->scan());
			}
		}
	}
}

bool AMAppControllerForActions2::startupRegisterDatabases()
{
	if(!AMDatamanAppControllerForActions2::startupRegisterDatabases())
		return false;

	AMActionRegistry::s()->registerInfoAndAction<AMLoopActionInfo, AMLoopAction>("Loop", "This action repeats a set of sub-actions a specific number of times.\n\nAfter adding it, you can drag-and-drop other actions inside it.", ":/32x32/media-playlist-repeat.png");
	AMActionRegistry::s()->registerInfoAndAction<AMWaitActionInfo, AMWaitAction>("Wait", "This action simply waits for a specified amount of time.", ":/user-away.png");
	AMActionRegistry::s()->registerInfoAndAction<AMChangeRunActionInfo, AMChangeRunAction>("Change Run", "This action changes the current run, or creates a new one.");

	AMActionRegistry::s()->registerInfoAndEditor<AMWaitActionInfo, AMWaitActionEditor>();
	AMActionRegistry::s()->registerInfoAndEditor<AMLoopActionInfo, AMLoopActionEditor>();
	AMActionRegistry::s()->registerInfoAndEditor<AMChangeRunActionInfo, AMChangeRunActionEditor>();

	return true;
}
