#include "AMScanViewToolBar.h"
#include "MPlot/MPlot.h"
#include "MPlot/MPlotAbstractTool.h"

AMScanViewToolBar::AMScanViewToolBar(MPlot *plot, QList<MPlotAbstractTool*> tools, bool exclusiveSelection, QWidget *parent) :
	QWidget(parent)
{
	// Set member variables.

	plot_ = 0;

	// Create UI elements.

	toolBar_ = new QToolBar("Plot");
	connect( toolBar_, SIGNAL(actionTriggered(QAction*)), this, SLOT(onToolSelected(QAction*)) );

	dataPositionView_ = new AMScanViewToolBarDataPositionView();

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(toolBar_);
	layout->addWidget(dataPositionView_);
	layout->addStretch();

	setLayout(layout);

	// Current settings.

	setPlot(plot);
	setTools(tools);
	setExclusiveSelectionStatus(exclusiveSelection);
}

AMScanViewToolBar::~AMScanViewToolBar()
{

}

void AMScanViewToolBar::setPlot(MPlot *newPlot)
{
	if (plot_ != newPlot) {
		plot_ = newPlot;
		emit plotChanged(plot_);
	}
}

void AMScanViewToolBar::setTools(QList<MPlotAbstractTool*> newTools)
{
	if (tools_ != newTools) {

		if (!tools_.isEmpty()) {

			// Remove all old actions from the tool bar, all old tools from the plot.
			toolBar_->clear();
			plot_->removeTools();
		}

		tools_ = newTools;

		if (!tools_.isEmpty()) {

			// Create actions for each available tool and add to tool bar.
			foreach (MPlotAbstractTool *tool, tools_) {
				QAction *toolAction = new QAction(QIcon(":/22x22/view-list-details-symbolic.png"), tool->name(), this);
				toolAction->setCheckable(true);
				toolAction->setStatusTip(tool->description());

				toolBar_->addAction(toolAction);
			}
		}

		emit toolsChanged(tools_);
	}
}

void AMScanViewToolBar::setExclusiveSelectionStatus(bool isExclusive)
{
	if (exclusiveSelection_ != isExclusive) {
		exclusiveSelection_ = isExclusive;
		emit exclusiveSelectionStatusChanged(exclusiveSelection_);
	}
}

void AMScanViewToolBar::onToolSelected(QAction *action)
{
	if (plot_ && action) {

		// Identify the tool associated with the given action.

		MPlotAbstractTool *selectedTool = 0;
		int toolIndex = 0;
		int toolCount = tools_.count();

		while (!selectedTool && toolIndex < toolCount) {
			MPlotAbstractTool *tool = tools_.at(toolIndex);
			if (tool->name() == action->text())
				selectedTool = tool;
		}

		// If the action is checked, we are applying this tool to the plot. Removing it otherwise.

		if (selectedTool) {
			if (action->isChecked()) {
				if (exclusiveSelection_)
					plot_->removeTools();

				plot_->addTool(selectedTool);

			} else {
				plot_->removeTool(selectedTool);
			}
		}
	}
}










AMScanViewToolBarDataPositionView::AMScanViewToolBarDataPositionView(bool enableValue, QWidget *parent) :
	QWidget(parent)
{
	// Initialize member variables.

	valueEnabled_ = false;

	// Create UI elements.

	QLabel *positionPrompt = new QLabel("Position:");
	position_ = new QLabel();
	positionDisplay_ = new QGroupBox();

	QLabel *valuePrompt = new QLabel("Value:");
	value_ = new QLabel();
	valueDisplay_ = new QGroupBox();

	// Create and set layouts.

	QHBoxLayout *positionDisplayLayout = new QHBoxLayout();
	positionDisplayLayout->addWidget(positionPrompt);
	positionDisplayLayout->addWidget(position_);

	positionDisplay_->setLayout(positionDisplayLayout);

	QHBoxLayout *valueDisplayLayout = new QHBoxLayout();
	valueDisplayLayout->addWidget(valuePrompt);
	valueDisplayLayout->addWidget(value_);

	valueDisplay_->setLayout(valueDisplayLayout);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(positionDisplay_);
	layout->addWidget(valueDisplay_);

	setLayout(layout);

	// Initial settings.

	valueDisplay_->hide();

	// Current settings.

	setValueEnabled(enableValue);
}

AMScanViewToolBarDataPositionView::~AMScanViewToolBarDataPositionView()
{

}

void AMScanViewToolBarDataPositionView::setValueEnabled(bool isEnabled)
{
	if (valueEnabled_ != isEnabled) {
		valueEnabled_ = isEnabled;
		valueDisplay_->setVisible(valueEnabled_);
		emit valueEnabledChanged(valueEnabled_);
	}
}

void AMScanViewToolBarDataPositionView::setPosition(const QString &newPosition)
{
	position_->setText(newPosition);
}

void AMScanViewToolBarDataPositionView::setValue(const QString &newValue)
{
	value_->setText(newValue);
}
