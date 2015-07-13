#include "AMScanViewPlotToolsView.h"
#include <QLabel>

AMScanViewPlotToolsView::AMScanViewPlotToolsView(AMScanViewPlotTools *tools, QWidget *parent) :
	QWidget(parent)
{
	// Initialize member variables.

	tools_ = 0;

	// Create UI elements.

	buttonView_ = new AMScanViewPlotToolsButtonView(0);
	selectedToolsView_ = new AMScanViewPlotSelectedToolsView(0);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(buttonView_);
	layout->addWidget(selectedToolsView_);
	layout->addStretch();

	setLayout(layout);

	// Current settings.

	setTools(tools);
}

AMScanViewPlotToolsView::~AMScanViewPlotToolsView()
{

}

void AMScanViewPlotToolsView::setTools(AMScanViewPlotTools *newTools)
{
	if (tools_ != newTools) {

		if (tools_) {
			buttonView_->setTools(0);
			selectedToolsView_->setTools(0);
		}

		tools_ = newTools;

		if (tools_) {
			buttonView_->setTools(tools_);
			selectedToolsView_->setTools(tools_);
		}

		emit toolsChanged(tools_);
	}
}

void AMScanViewPlotToolsView::clear()
{
	buttonView_->clear();
	selectedToolsView_->clear();
}

void AMScanViewPlotToolsView::update()
{
	buttonView_->update();
	selectedToolsView_->update();
}

void AMScanViewPlotToolsView::refresh()
{
	buttonView_->refresh();
	selectedToolsView_->refresh();
}
