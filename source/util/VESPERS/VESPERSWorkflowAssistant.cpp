#include "VESPERSWorkflowAssistant.h"

#include "actions/AMBeamlineScanAction.h"

VESPERSWorkflowAssistant::VESPERSWorkflowAssistant(AMWorkflowManagerView *workflow, QObject *parent)
	: QObject(parent)
{
	workflow_ = workflow;
    currentScan_ = -1;
    config_ = 0;

	connect(workflow_, SIGNAL(actionItemCountChanged(int)), this, SLOT(listen()));
    connect(workflow_, SIGNAL(runningChanged(bool)), this, SIGNAL(workflowRunningChanged(bool)));
}

void VESPERSWorkflowAssistant::listen()
{

	AMBeamlineScanAction *scanAction = qobject_cast<AMBeamlineScanAction *>(workflow_->headOfQueue());

    if (!scanAction){

        config_ = 0;
		return;
    }

    config_ = qobject_cast<VESPERSEXAFSScanConfiguration *>(scanAction->cfg());

    if (!config_)
		return;

    qDebug() << config_->name() << config_->numberOfScans();
}
