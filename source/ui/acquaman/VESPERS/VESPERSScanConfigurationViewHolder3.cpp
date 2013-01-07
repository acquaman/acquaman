#include "VESPERSScanConfigurationViewHolder3.h"
#include "actions3/AMLoopAction3.h"
#include "actions3/actions/AMScanAction.h"

VESPERSScanConfigurationViewHolder3::VESPERSScanConfigurationViewHolder3(VESPERSEXAFSScanConfigurationView *view, QWidget *parent)
	: AMScanConfigurationViewHolder3(view, parent)
{
}

AMAction3 *VESPERSScanConfigurationViewHolder3::createAction()
{
	if (view_){

		const VESPERSEXAFSScanConfiguration *config = qobject_cast<const VESPERSEXAFSScanConfiguration *>(view_->configuration());

		if (config){

			AMLoopAction3 *loop = new AMLoopAction3(config->numberOfScans());
			loop->addSubAction(new AMScanAction(new AMScanActionInfo(config->createCopy())));
			return loop;
		}
	}

	return 0;
}
