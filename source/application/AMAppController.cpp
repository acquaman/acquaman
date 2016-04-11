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

#include "AMAppController.h"

#include <QStringBuilder>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>

#include "application/AMAppControllerSupport.h"

#include "acquaman/AMScanConfiguration.h"
#include "acquaman/AMScanController.h"
#include "acquaman/AMDetectorTriggerSourceScanOptimizer.h"
#include "acquaman/AMDetectorDwellTimeSourceScanOptimizer.h"
#include "acquaman/AMListActionScanOptimizer.h"
#include "acquaman/AMNestedAxisTypeValidator.h"
#include "acquaman/AMAgnosticDataAPI.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/AMAction3.h"
#include "actions3/actions/AMScanAction.h"
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
#include "actions3/actions/AMSamplePlatePre2013MoveAction.h"
#include "actions3/actions/AMSamplePlatePre2013MoveActionInfo.h"
#include "actions3/editors/AMSamplePlatePre2013MoveActionEditor.h"
#include "actions3/actions/AMSampleMoveAction.h"
#include "actions3/actions/AMSampleMoveActionInfo.h"
#include "actions3/editors/AMSampleMoveActionEditor.h"
#include "actions3/actions/AMControlWaitAction.h"
#include "actions3/actions/AMControlWaitActionInfo.h"
#include "actions3/actions/AMWaitAction.h"
#include "actions3/editors/AMWaitActionEditor.h"
#include "actions3/actions/AMChangeToleranceAction.h"

#include "beamline/AMBeamline.h"
#include "beamline/AMStorageRing.h"
#include "beamline/AMProcessVariablePrivate.h"

#include "dataman/AMRun.h"
#include "dataman/AMScan.h"
#include "dataman/AMScanEditorModelItem.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/database/AMDatabase.h"
#include "dataman/export/AMExporter.h"
#include "dataman/export/AMExporterOption.h"

#include "ui/AMMainWindow.h"
#include "ui/AMStartScreen.h"
#include "ui/actions3/AMWorkflowView3.h"
#include "ui/AMAppBottomPanel.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/dataman/AMGenericScanEditor.h"


AMAppController::AMAppController(QObject *parent)
	: AMDatamanAppControllerForActions3(parent)
{
	automaticBringScanEditorToFrontWithRunningScans_ = true;
	automaticLaunchScanEditor_ = true;
}

AMAppController::~AMAppController()
{
}

bool AMAppController::startup(){

	if(AMDatamanAppControllerForActions3::startup()){
		bool success = true;
		success &= AMActionRegistry3::s()->registerInfoAndAction<AMLoopActionInfo3, AMLoopAction3>("Loop", "This action repeats a set of sub-actions a specific number of times.\n\nAfter adding it, you can drag-and-drop other actions inside it.", ":/32x32/media-playlist-repeat.png");
		success &= AMActionRegistry3::s()->registerInfoAndEditor<AMLoopActionInfo3, AMLoopActionEditor3>();

		success &= AMActionRegistry3::s()->registerInfoAndAction<AMListActionInfo3, AMListAction3>("List", "A sequential or parallel list of other actions", ":/22x22/viewListInv-22x22.png", false);
		success &= AMActionRegistry3::s()->registerInfoAndAction<AMSequentialListActionInfo3, AMSequentialListAction3>("Sequential\nList", "This action runs a sequential list of other actions", ":/22x22/viewListInv-22x22.png");
		success &= AMActionRegistry3::s()->registerInfoAndAction<AMParallelListActionInfo3, AMParallelListAction3>("Parallel\nList", "This action runs a parallel list of other actions.", ":/22x22/viewDetaillInv-22x22.png");
		success &= AMActionRegistry3::s()->registerInfoAndEditor<AMListActionInfo3, AMListActionEditor3>();
		success &= AMActionRegistry3::s()->registerInfoAndAction<AMControlMoveActionInfo3, AMControlMoveAction3>("Control Move", "Moves a control to an absolute position or a relative position from its current state.", ":system-run.png");
		success &= AMActionRegistry3::s()->registerInfoAndEditor<AMControlMoveActionInfo3, AMControlMoveActionEditor3>();

		success &= AMActionRegistry3::s()->registerInfoAndAction<AMScanActionInfo, AMScanAction>("Scan Action", "Runs a scan.", ":/spectrum.png", false);
		success &= AMActionRegistry3::s()->registerInfoAndEditor<AMScanActionInfo, AMScanActionEditor>();

		success &= AMActionRegistry3::s()->registerInfoAndAction<AMWaitActionInfo, AMWaitAction>("Wait Action", "Waits for a predetermined amount of time", ":/user-away.png", true);
		success &= AMActionRegistry3::s()->registerInfoAndEditor<AMWaitActionInfo, AMWaitActionEditor>();

		success &= AMActionRegistry3::s()->registerInfoAndAction<AMControlWaitActionInfo, AMControlWaitAction>("Wait for Control", "Wait for Control", ":system-run.png", false);
		success &= AMActionRegistry3::s()->registerInfoAndAction<AMChangeToleranceActionInfo, AMChangeToleranceAction>("Change Tolerance Action", "Changes the tolerance of a specific control.", ":/user-away.png", false);
		AMAgnosticDataMessageQEventHandler *scanActionMessager = new AMAgnosticDataMessageQEventHandler();
		AMAgnosticDataAPISupport::registerHandler("ScanActions", scanActionMessager);

		AMDetectorTriggerSourceScanOptimizer *triggerOptimizer = new AMDetectorTriggerSourceScanOptimizer();
		AMDetectorDwellTimeSourceScanOptimizer *dwellTimeOptimizer = new AMDetectorDwellTimeSourceScanOptimizer();
		AMEmptyListScanOptimizer *emptyListOptimizer = new AMEmptyListScanOptimizer();
		AMSingleElementListOptimizer *singleElementListOptimizer = new AMSingleElementListOptimizer();
		AMAppControllerSupport::appendPrincipleOptimizer(triggerOptimizer);
		AMAppControllerSupport::appendPrincipleOptimizer(dwellTimeOptimizer);
		AMAppControllerSupport::appendPrincipleOptimizer(emptyListOptimizer);
		AMAppControllerSupport::appendPrincipleOptimizer(singleElementListOptimizer);

		AMNestedAxisTypeValidator *nestedAxisValidator = new AMNestedAxisTypeValidator();
		AMAppControllerSupport::appendPrincipleValidator(nestedAxisValidator);

		return success;
	}
	else
		return false;
}

void AMAppController::shutdown()
{
	AMDatamanAppControllerForActions3::shutdown();
	AMBeamline::releaseBl();
	AMStorageRing::releaseStorageRing();
	AMProcessVariableSupport::shutdownChannelAccess();
}

bool AMAppController::startupCreateUserInterface() {

	if (AMDatamanAppControllerForActions3::startupCreateUserInterface()){

		// Defaults the auto-open for generic scan editors to true.  All new running scans will have their scan editor brought to the front.
		setAutomaticBringScanEditorToFront(true);

		// add the workflow control UI
		workflowView_ = new AMWorkflowView3(AMActionRunner3::workflow());
		mw_->insertHeading("Experiment Tools", 1);
		mw_->addPane(workflowView_, "Experiment Tools", "Workflow", ":/user-away.png");

		scanActionRunnerView_ = new AMWorkflowView3(AMActionRunner3::scanActionRunner());
		scanActionRunnerView_->hide();

		// get the "open scans" section to be under the workflow
		mw_->windowPaneModel()->removeRow(scanEditorsParentItem_->row());
		scanEditorsParentItem_ = mw_->windowPaneModel()->headingItem("Open Scans", QModelIndex(), mw_->windowPaneModel()->rowCount()-1);

		connect(AMActionRunner3::workflow(), SIGNAL(scanActionStarted(AMScanAction*)), this, SLOT(onCurrentScanActionStarted(AMScanAction*)));
		connect(AMActionRunner3::workflow(), SIGNAL(scanActionFinished(AMScanAction *)), this, SLOT(onCurrentScanActionFinished(AMScanAction*)));

		AMStartScreen* chooseRunDialog = new AMStartScreen(true, mw_);
		chooseRunDialog->show();
		chooseRunDialog->activateWindow();
		chooseRunDialog->raise();

		AMAppControllerSupport::addActionRunnerGroup(AMActionRunner3::workflow()->loggingDatabase()->connectionName(), AMActionRunner3::workflow(), workflowView_->historyView()->model());
		AMAppControllerSupport::addActionRunnerGroup(AMActionRunner3::scanActionRunner()->loggingDatabase()->connectionName(), AMActionRunner3::scanActionRunner(), scanActionRunnerView_->historyView()->model());

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

void AMAppController::goToWorkflow()
{
	mw_->setCurrentPane(workflowView_);
}

void AMAppController::updateScanEditorModelItem()
{
	// Get the action, or if it's in a list, the current running action.
	AMAction3 *currentAction = AMActionRunner3::workflow()->currentAction();
	AMScanAction *action = 0;

	if (currentAction && !currentAction->hasChildren())
		action = qobject_cast<AMScanAction *>(currentAction);

	else if (currentAction && currentAction->hasChildren()){

		AMListAction3 *listAction = qobject_cast<AMListAction3 *>(currentAction);

		if (listAction)
			action = qobject_cast<AMScanAction *>(listAction->currentSubAction());
	}

	// Do something with it if the action is valid.
	if (action && (action->state() == AMAction3::Running || action->inFinalState())){

		AMGenericScanEditor *editor = editorFromScan(action->controller()->scan());

		if (!editor)
			return;

		QString stateString;

		switch(action->state()){

		case AMAction3::Running:
			stateString = "running";
			break;

		case AMAction3::Succeeded:
			stateString = "succeeded";
			break;

		case AMAction3::Failed:
			stateString = "failed";
			break;

		case AMAction3::Cancelled:
			stateString = "cancelled";
			break;

		default:
			stateString = "default";
			break;
		}

		AMScanEditorModelItem *item = (AMScanEditorModelItem *)(mw_->windowPaneModel()->itemFromIndex(mw_->windowPaneModel()->indexForPane(editor)));

		if (item)
			item->scanActionStateChanged(stateString, editor == mw_->currentPane());
	}
}

void AMAppController::onActionAutomaticLaunchScanEditorToggled(bool toggled){
	setAutomaticLaunchScanEditor(toggled);
}

void AMAppController::onCurrentScanActionStarted(AMScanAction *action)
{
	if(automaticLaunchScanEditor_){
		AMScan *scan = action->controller()->scan();
		openScanInEditor(scan, automaticBringScanEditorToFrontWithRunningScans());

		scanEditorScanMapping_.append(qMakePair(scan, scanEditorAt(scanEditorCount()-1)));
		connect(action, SIGNAL(stateChanged(int,int)), this, SLOT(updateScanEditorModelItem()));
		updateScanEditorModelItem();
	}

	onCurrentScanActionStartedImplementation(action);
}

void AMAppController::onCurrentScanActionFinished(AMScanAction *action)
{
	disconnect(action, SIGNAL(stateChanged(int,int)), this, SLOT(updateScanEditorModelItem()));

	// It is possible to cancel a scan just before it starts, so we need to check this to see if there's any controller at all
	if(action->controller() && automaticLaunchScanEditor_)
		updateScanEditorModelItem();

	onCurrentScanActionFinishedImplementation(action);

	if(!automaticLaunchScanEditor_ && action && action->controller() && action->controller()->scan())
		action->controller()->scan()->deleteLater();
}

bool AMAppController::startupPopulateNewUserDBTables(AMDatabase* userDb)
{
	AMFacility newFacility = facility();

	// AMFacility will contain only one facility for each beamline and it will be created for the first time run
	if (userDb->objectsMatching("AMFacility_table", "name", newFacility.name()).isEmpty()) {
		newFacility.storeToDb(userDb);

		// initialize the default AMRun information
		AMRun firstRun(newFacility.name());
		firstRun.storeToDb(userDb);
	}

	return true;
}

void AMAppController::openScanInEditor(AMScan *scan, bool bringEditorToFront, bool openInExistingEditor)
{
	AMGenericScanEditor* editor;

	if(openInExistingEditor && scanEditorCount())
		editor = scanEditorAt(scanEditorCount()-1);
	else{

		bool use2DScanView = (scan->scanRank() == 2 || scan->scanRank() == 3);
		editor = createNewScanEditor(use2DScanView);
	}

	editor->addScan(scan);

	if(bringEditorToFront)
		mw_->setCurrentPane(editor);
}

void AMAppController::launchScanConfigurationFromDb(const QUrl &url)
{
	AMScanConfigurationView *configurationView = createScanConfigurationViewFromDb(url);
	if (configurationView) {
		// This is Actions3 stuff.
		AMScanConfigurationViewHolder3 *configurationViewHolder;
		if (configurationView->configuration()->isXASScan())
			configurationViewHolder = new AMScanConfigurationViewHolder3(configurationView, true);
		else
			configurationViewHolder = new AMScanConfigurationViewHolder3(configurationView);

		configurationViewHolder->setAttribute(Qt::WA_DeleteOnClose, true);
		configurationViewHolder->show();
	}
}


bool AMAppController::eventFilter(QObject* o, QEvent* e)
{
	return AMDatamanAppController::eventFilter(o,e);
}

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

void AMAppController::showScanActionsView(){
	if(scanActionRunnerView_->isHidden())
		scanActionRunnerView_->show();
	scanActionRunnerView_->raise();
}

bool AMAppController::startupInstallActions()
{
	if(AMDatamanAppControllerForActions3::startupInstallActions()) {

		QAction *changeRunAction = new QAction("Change Run...", mw_);
		changeRunAction->setStatusTip("Change the current run, or create a new one");
		connect(changeRunAction, SIGNAL(triggered()), this, SLOT(showChooseRunDialog()));

		QAction *openScanActionsViewAction = new QAction("See Scan Actions...", mw_);
		openScanActionsViewAction->setStatusTip("Open the view to see all actions done by scans");
		connect(openScanActionsViewAction, SIGNAL(triggered()), this, SLOT(showScanActionsView()));

		QAction *automaticLaunchScanEditorAction = new QAction("Launch New Scans in Editors", mw_);
		automaticLaunchScanEditorAction->setStatusTip("Launch all new scans in an new scan editor to view them");
		automaticLaunchScanEditorAction->setCheckable(true);
		automaticLaunchScanEditorAction->setChecked(true);
		connect(automaticLaunchScanEditorAction, SIGNAL(toggled(bool)), this, SLOT(onActionAutomaticLaunchScanEditorToggled(bool)));

		fileMenu_->addSeparator();
		fileMenu_->addAction(changeRunAction);

		viewMenu_->addAction(openScanActionsViewAction);
		viewMenu_->addAction(automaticLaunchScanEditorAction);

		return true;
	}
	else
		return false;
}

void AMAppController::setActionRunnerCancelPromptVisibility(bool showPrompt)
{
	workflowView_->currentView()->setCancelPromptVisibility(showPrompt);
	AMAppBottomPanel *bottom = qobject_cast<AMAppBottomPanel *>(bottomPanel_);

	if (bottom)
		bottom->workFlowView()->setCancelPromptVisibility(showPrompt);
}
