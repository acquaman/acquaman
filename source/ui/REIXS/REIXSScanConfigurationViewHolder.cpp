#include "REIXSScanConfigurationViewHolder.h"

#include "ui/REIXS/REIXSXESScanConfigurationView.h"
#include "actions2/actions/REIXS/REIXSXESScanAction.h"

REIXSScanConfigurationViewHolder::REIXSScanConfigurationViewHolder(AMScanConfigurationView *view, QWidget *parent)
	: AMActionRunnerAddActionBar("Scan", parent)
{
	view_ = view;
	addWidget(view);
}


AMAction * REIXSScanConfigurationViewHolder::createAction()
{
	const REIXSXESScanConfiguration* xesConfig = qobject_cast<const REIXSXESScanConfiguration*>(view_->configuration());

	if(xesConfig)
		return new REIXSXESScanAction(qobject_cast<REIXSXESScanConfiguration*>(xesConfig->createCopy()));

	return 0;
}

