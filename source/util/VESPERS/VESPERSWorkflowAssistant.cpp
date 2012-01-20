#include "VESPERSWorkflowAssistant.h"

#include "actions/AMBeamlineScanAction.h"
#include "acquaman/VESPERS/VESPERSEXAFSScanConfiguration.h"

VESPERSWorkflowAssistant::VESPERSWorkflowAssistant(AMWorkflowManagerView *workflow, QObject *parent)
	: QObject(parent)
{
	workflow_ = workflow;

	connect(workflow_, SIGNAL(actionItemCountChanged(int)), this, SLOT(listen()));
}

void VESPERSWorkflowAssistant::listen()
{
	AMBeamlineScanAction *scanAction = qobject_cast<AMBeamlineScanAction *>(workflow_->headOfQueue());

	if (!scanAction)
		return;

	VESPERSEXAFSScanConfiguration *config = qobject_cast<VESPERSEXAFSScanConfiguration *>(scanAction->cfg());

	if (!config)
		return;

	qDebug() << config->name();
}
