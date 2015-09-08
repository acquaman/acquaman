
#ifndef REIXSAPPBOTTOMPANEL_H
#define REIXSAPPBOTTOMPANEL_H

#include "ui/AMDatamanAppBottomPanel.h"
#include "actions3/AMActionRunner3.h"
#include "ui/REIXS/REIXSActionRunnerBottomBarCurrentView3.h"


/// This class implements the bottom panel used in the App for REIXS.  It contains a mini-view of the workflow and adds it between the add experiment button, the status view, and is XESScan/MCP aware for scan time modification
class REIXSAppBottomPanel : public AMDatamanAppBottomPanel
{
	Q_OBJECT

public:
	/// Constructor.  Passes in the action runner to appropriately build the mini-workflow view.
	virtual ~REIXSAppBottomPanel();
	REIXSAppBottomPanel(AMActionRunner3 *actionRunner, QWidget *parent = 0);

	/// Returns the bottom bar view that is encapuslated.
	AMActionRunnerBottomBarCurrentView3 *workFlowView() const { return workflowView_; }

protected:
	/// The current workflow view.
	REIXSActionRunnerBottomBarCurrentView3 *workflowView_;
};

#endif // REIXSAPPBOTTOMPANEL_H
