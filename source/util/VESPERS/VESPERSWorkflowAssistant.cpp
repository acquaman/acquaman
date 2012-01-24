#include "VESPERSWorkflowAssistant.h"

#include "actions/AMBeamlineScanAction.h"

VESPERSWorkflowAssistant::VESPERSWorkflowAssistant(AMWorkflowManagerView *workflow, QObject *parent)
	: QObject(parent)
{
	workflow_ = workflow;
	currentScan_ = -1;
	config_ = 0;

	connect(workflow_, SIGNAL(actionItemCountChanged(int)), this, SLOT(listen()));
	connect(workflow_, SIGNAL(runningChanged(bool)), this, SLOT(onWorkflowRunningChanged(bool)));
}

void VESPERSWorkflowAssistant::listen()
{
	AMBeamlineScanAction *scanAction = qobject_cast<AMBeamlineScanAction *>(workflow_->headOfQueue());

	if (!scanAction)
		return;

	VESPERSEXAFSScanConfiguration * config = qobject_cast<VESPERSEXAFSScanConfiguration *>(scanAction->cfg());

	if (!config)
		return;

	if (config_ && config->name() == config_->name()){

		currentScan_++;
		emit currentScanChanged(currentScan_);
	}
	else{

		config_ = config;
		currentScan_ = 1;

		// If the workflow is already running, then automatically add these extra scans to the workflow.
		if (workflow_->isRunning())
			addExtraScanActions(config_->numberOfScans()-1);

		emit configurationChanged();
	}
}

void VESPERSWorkflowAssistant::setTotalScans(int num)
{
	int difference = num - config_->numberOfScans();

	if (difference == 0)
		return;

	else if (difference > 0)
		addExtraScanActions(difference);

	else if (difference < 0)
		removeExtraScanActions(-difference);

	config_->setNumberOfScans(num);
}

void VESPERSWorkflowAssistant::onWorkflowRunningChanged(bool running)
{
	if (running)
		addExtraScanActions(config_->numberOfScans()-1);

	emit workflowRunningChanged(running);
}

void VESPERSWorkflowAssistant::addExtraScanActions(int num)
{
	for (int i = 0; i < num; i++){

		AMBeamlineScanAction* action = new AMBeamlineScanAction(config_->createCopy());
		workflow_->insertBeamlineAction(0, action);
	}
}

void VESPERSWorkflowAssistant::removeExtraScanActions(int num)
{
	for (int i = 0; i < num; i++)
		workflow_->removeBeamlineAction(0);
}
