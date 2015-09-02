
#include "REIXSAppBottomPanel.h"

class REIXSActionRunnerBottomBarCurrentView3;

 REIXSAppBottomPanel::~REIXSAppBottomPanel(){}
REIXSAppBottomPanel::REIXSAppBottomPanel(AMActionRunner3 *actionRunner, QWidget *parent)
	: AMDatamanAppBottomPanel(parent)
{
	workflowView_ = new REIXSActionRunnerBottomBarCurrentView3(actionRunner);
	layout_->insertStretch(2);
	layout_->insertWidget(3, workflowView_);
}
