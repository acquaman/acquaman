#ifndef AMFREERUNSCANCONFIGURATIONVIEWHOLDER_H
#define AMFREERUNSCANCONFIGURATIONVIEWHOLDER_H

#include "ui/AMScanConfigurationViewHolder.h"

class AMFreeRunScanConfigurationViewHolder : public AMScanConfigurationViewHolder
{
	Q_OBJECT

public:
	/// Default constructor that simply passes on all this information but hides the bottom bar.
	AMFreeRunScanConfigurationViewHolder(AMWorkflowManagerView* workflow, AMScanConfigurationView* view = 0, QWidget *parent = 0);

public slots:
	/// Start the scan and add it to the workflow.  It is public to allow your free run viewing classes the ability to connect to the workflow without having to see it.
	void onFreeRunStartRequested();
};

#endif // AMFREERUNSCANCONFIGURATIONVIEWHOLDER_H
