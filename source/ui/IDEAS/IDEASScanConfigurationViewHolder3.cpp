#include "IDEASScanConfigurationViewHolder3.h"
#include "actions3/AMLoopAction3.h"
#include "actions3/actions/AMScanAction.h"

IDEASScanConfigurationViewHolder3::~IDEASScanConfigurationViewHolder3(){}

IDEASScanConfigurationViewHolder3::IDEASScanConfigurationViewHolder3(IDEASXASScanConfigurationView *view, QWidget *parent)
	: AMScanConfigurationViewHolder3(view, parent)
{
}

AMAction3 *IDEASScanConfigurationViewHolder3::createAction()
{
	if (view_){

		const IDEASXASScanConfiguration *config = qobject_cast<const IDEASXASScanConfiguration *>(view_->configuration());

		if (config){

			AMLoopAction3 *loop = new AMLoopAction3(new AMLoopActionInfo3(config->numberOfScans(), config->name(), config->description()));
			loop->addSubAction(new AMScanAction(new AMScanActionInfo(config->createCopy())));
			return loop;
		}
	}

	return 0;
}
