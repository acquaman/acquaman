#include "AMFreeRunScanConfigurationViewHolder.h"

#include <QRadioButton>
#include <QPushButton>
#include <QLabel>

AMFreeRunScanConfigurationViewHolder::AMFreeRunScanConfigurationViewHolder(AMWorkflowManagerView *workflow, AMScanConfigurationView *view, QWidget *parent)
	: AMScanConfigurationViewHolder(workflow, view, parent)
{
	whenDoneLabel_->hide();
	startScanButton_->hide();
	addToQueueButton_->hide();
	goToWorkflowOption_->hide();
	setupAnotherScanOption_->hide();

	setupAnotherScanOption_->setChecked(true);
}

void AMFreeRunScanConfigurationViewHolder::onFreeRunStartRequested()
{
	onStartScanRequested();
}
