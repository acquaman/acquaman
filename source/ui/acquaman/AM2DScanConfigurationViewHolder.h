#ifndef AM2DSCANCONFIGURATIONVIEWHOLDER_H
#define AM2DSCANCONFIGURATIONVIEWHOLDER_H

#include "ui/acquaman/AMScanConfigurationViewHolder.h"

/// This is a very simple extension to AMScanConfigurationViewHolder that adds 2D scan actions instead of regular ones.
class AM2DScanConfigurationViewHolder : public AMScanConfigurationViewHolder
{
	Q_OBJECT

public:
	/// Constructor.  Builds a view for the holder.  This adds nothing to the view.
	AM2DScanConfigurationViewHolder(AMWorkflowManagerView *workflow, AMScanConfigurationView *view = 0, QWidget *parent = 0);

protected slots:
	/// Reimplemented to add 2D scan actions instead of 1D.
	virtual void onAddToQueueRequested();
};

#endif // AM2DSCANCONFIGURATIONVIEWHOLDER_H
