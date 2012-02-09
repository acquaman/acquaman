#ifndef REIXSSCANCONFIGURATIONVIEWHOLDER_H
#define REIXSSCANCONFIGURATIONVIEWHOLDER_H

#include "ui/actions2/AMActionRunnerAddActionBar.h"

class AMScanConfigurationView;

/// This widget holds view widgets for configuring scans on REIXS, and implements AMActionRunnerAddActionBar::createAction() to create the REIXS-specific actions.
class REIXSScanConfigurationViewHolder : public AMActionRunnerAddActionBar
{
    Q_OBJECT
public:
	/// Constructor. We can handle any of the AMScanConfigurationViews used on REIXS.
	REIXSScanConfigurationViewHolder(AMScanConfigurationView* view, QWidget *parent = 0);

protected:
	/// Creates the appropriate action depending on the view type.
	virtual AMAction* createAction();

	AMScanConfigurationView* view_;
};

#endif // REIXSSCANCONFIGURATIONVIEWHOLDER_H
