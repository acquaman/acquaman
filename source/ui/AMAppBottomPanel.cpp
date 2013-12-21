#include "AMAppBottomPanel.h"

 AMAppBottomPanel::~AMAppBottomPanel(){}
AMAppBottomPanel::AMAppBottomPanel(AMActionRunner3 *actionRunner, QWidget *parent)
	: AMDatamanAppBottomPanel(parent)
{
	workflowView_ = new AMActionRunnerBottomBarCurrentView3(actionRunner);
	layout_->insertStretch(2);
	layout_->insertWidget(3, workflowView_);
}
