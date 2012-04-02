#include "AM2DScanConfigurationViewHolder.h"

#include "actions/AMBeamline2DScanAction.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/AMWorkflowManagerView.h"

#include <QRadioButton>

AM2DScanConfigurationViewHolder::AM2DScanConfigurationViewHolder(AMWorkflowManagerView *workflow, AMScanConfigurationView *view, QWidget *parent)
	:AMScanConfigurationViewHolder(workflow, view, parent)
{
}

void AM2DScanConfigurationViewHolder::onAddToQueueRequested()
{
	if(!view_ || !view_->configuration())
		return;

	AMScanConfiguration *config = view_->configuration()->createCopy();
	/*
	config->setUserScanName(scanNameDictionaryLineEdit_->text());
	config->setUserExportNmae(exportNameDictionaryLineEdit_->text());
	*/
	AMBeamline2DScanAction* action = new AMBeamline2DScanAction(config);
	workflow_->insertBeamlineAction(-1, action);

	if(goToWorkflowOption_->isChecked())
		emit showWorkflowRequested();
}
