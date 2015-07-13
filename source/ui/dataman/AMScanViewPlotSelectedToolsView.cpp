#include "AMScanViewPlotSelectedToolsView.h"
#include "ui/dataman/AMScanViewPlotToolView.h"
#include "MPlot/MPlotTools.h"

AMScanViewPlotSelectedToolsView::AMScanViewPlotSelectedToolsView(AMScanViewPlotTools *tools, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	tools_ = 0;

	// Create and set layouts.

	layout_ = new QHBoxLayout();
	layout_->setMargin(0);

	setLayout(layout_);

	// Current settings.

	setTools(tools);
}

AMScanViewPlotSelectedToolsView::~AMScanViewPlotSelectedToolsView()
{

}

void AMScanViewPlotSelectedToolsView::setTools(AMScanViewPlotTools *newTools)
{
	if (tools_ != newTools) {

		if (tools_) {
			disconnect( tools_, 0, this, 0 );
		}

		tools_ = newTools;

		if (tools_) {
			connect( tools_, SIGNAL(toolsChanged(QList<MPlotAbstractTool*>)), this, SLOT(refresh()) );
			connect( tools_, SIGNAL(selectedToolsChanged(QList<MPlotAbstractTool*>)), this, SLOT(update()) );
		}

		refresh();

		emit toolsChanged(tools_);
	}
}

void AMScanViewPlotSelectedToolsView::clear()
{
	foreach (QWidget *view, toolViews_) {
		toolViews_.removeOne(view);
		layout_->removeWidget(view);
		view->deleteLater();
	}
}

void AMScanViewPlotSelectedToolsView::update()
{
	if (tools_) {

		// Iterate through all tool views and show the views for selected tools.
		// Hide the views of unselected tools.

		int viewCount = toolViews_.count();

		for (int viewIndex = 0; viewIndex < viewCount; viewIndex++) {
			if (viewIndex < tools_->tools().count()) {
				MPlotAbstractTool *tool = tools_->tools().at(viewIndex);
				QWidget *toolView = toolViews_.at(viewIndex);

				if (tool && toolView) {
					if (tools_->isSelectedTool(tool))
						toolView->show();
					else
						toolView->hide();
				}
			}
		}
	}
}

void AMScanViewPlotSelectedToolsView::refresh()
{
	// Clear the view.

	clear();

	// If we have valid tools, create the view.

	if (tools_) {

		// Create tool view for each tool and add to list of tool views.

		int toolCount = tools_->tools().count();

		for (int toolIndex = 0; toolIndex < toolCount; toolIndex++) {
			MPlotAbstractTool *tool = tools_->tools().at(toolIndex);

			if (tool) {
				QWidget *toolView = createToolView(tool);
				toolViews_ << toolView;

				if (toolView)
					layout_->addWidget(toolView);
			}
		}
	}

	// Update with most recent settings.

	update();
}

QWidget* AMScanViewPlotSelectedToolsView::createToolView(MPlotAbstractTool *tool)
{
	QWidget *view = 0;

	if (tool) {
		MPlotDataPositionCursorTool *positionCursorTool = qobject_cast<MPlotDataPositionCursorTool*>(tool);
		MPlotDataPositionTool *positionTool = qobject_cast<MPlotDataPositionTool*>(tool);

		if (positionCursorTool)
			view = new AMDataPositionCursorToolView(positionCursorTool, this);
		else if (positionTool)
			view = new AMDataPositionToolView(positionTool, this);
	}

	return view;
}

