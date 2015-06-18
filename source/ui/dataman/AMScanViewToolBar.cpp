#include "AMScanViewToolBar.h"
#include "MPlot/MPlot.h"
#include "MPlot/MPlotAbstractTool.h"
#include "MPlot/MPlotTools.h"
#include "dataman/AMScanSetModel.h"

AMScanViewToolBar::AMScanViewToolBar(AMScanViewToolBarOptions *toolOptions, QWidget *parent) :
	QWidget(parent)
{
	// Set member variables.

	toolOptions_ = 0;

	// Create UI elements.

	toolBar_ = new QToolBar("Plot");

	dataPositionView_ = new AMScanViewToolBarDataPositionView();

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(toolBar_);
	layout->addWidget(dataPositionView_);
	layout->addStretch();

	setLayout(layout);

	// Make connections.

	connect( toolBar_, SIGNAL(actionTriggered(QAction*)), this, SLOT(onToolSelected(QAction*)) );

	// Current settings.

	setToolOptions(toolOptions);
}

AMScanViewToolBar::~AMScanViewToolBar()
{

}

void AMScanViewToolBar::setToolOptions(AMScanViewToolBarOptions *newOptions)
{
	if (toolOptions_ != newOptions) {

		if (toolOptions_) {
			toolBar_->clear();
			toolOptions_->disconnect();
		}

		toolOptions_ = newOptions;

		if (toolOptions_) {

			// Create actions for each available tool and add to tool bar.

			foreach (MPlotAbstractTool *tool, toolOptions_->tools()) {
				QAction *toolAction = new QAction(QIcon(":/22x22/view-list-details-symbolic.png"), tool->name(), this);
				toolAction->setCheckable(true);
				toolAction->setStatusTip(tool->description());

				toolBar_->addAction(toolAction);
			}

			connect( toolOptions_, SIGNAL(xUnitsChanged(QString)), this, SLOT(updateXUnits()) );
			connect( toolOptions_, SIGNAL(yUnitsChanged(QString)), this, SLOT(updateYUnits()) );
			connect( toolOptions_, SIGNAL(valueUnitsChanged(QString)), this, SLOT(updateValueUnits()) );
		}

//		emit toolOptionsChanged(toolOptions_);
	}
}

void AMScanViewToolBar::onToolSelected(QAction *action)
{
	if (toolOptions_ && action) {

		// Identify the tool associated with the given action.

		MPlotAbstractTool *selectedTool = 0;
		int toolIndex = 0;
		int toolCount = toolOptions_->tools().count();

		while (!selectedTool && toolIndex < toolCount) {
			MPlotAbstractTool *tool = toolOptions_->tools().at(toolIndex);
			if (tool->name() == action->text())
				selectedTool = tool;
		}

		// If the action is checked, we are applying this tool to the plot. Removing it otherwise.

		if (selectedTool) {
			if (action->isChecked()) {
				toolOptions_->addSelectedTool(selectedTool);

			} else {
				toolOptions_->removeSelectedTool(selectedTool);
			}
		}
	}
}

void AMScanViewToolBar::updateXUnits()
{
	if (toolOptions_) {
		dataPositionView_->setXUn
	}
}








AMScanViewToolBarOptions::AMScanViewToolBarOptions(QList<MPlotAbstractTool *> tools, bool exclusiveSelection, QObject *parent) :
	QObject(parent)
{
	// Initialize member variables.

	exclusiveSelection_ = false;
	xUnits_ = "";
	yUnits_ = "";
	valueUnits_ = "";

	// Current settings.

	setTools(tools);
	setExclusiveSelectionEnabled(exclusiveSelection);
}

AMScanViewToolBarOptions::~AMScanViewToolBarOptions()
{

}

void AMScanViewToolBarOptions::setTools(QList<MPlotAbstractTool *> newTools)
{
	if (tools_ != newTools) {

		// Clear any lists of previously selected tools.

		if (!tools_.isEmpty())
			selectedTools_.clear();

		tools_ = newTools;

		dragZoomerToolCheck();

		emit toolsChanged(tools_);
	}
}

void AMScanViewToolBarOptions::addTool(MPlotAbstractTool *newTool)
{
	if (!tools_.contains(newTool)) {
		tools_.append(newTool);
		dragZoomerToolCheck();
		emit toolsChanged(tools_);
	}
}

void AMScanViewToolBarOptions::removeTool(MPlotAbstractTool *tool)
{
	if (tools_.contains(tool)) {

		if (selectedTools_.contains(tool))
			removeSelectedTool(tool);

		tools_.removeOne(tool);

		emit toolsChanged(tools_);
	}
}

void AMScanViewToolBarOptions::setSelectedTools(QList<MPlotAbstractTool *> newTools)
{
	if (selectedTools_ != newTools) {

		// Clear the list of selected tools.

		selectedTools_.clear();

		// Check that each of the new tools exists in the master tool options list before adding them to selection.

		foreach (MPlotAbstractTool *tool, newTools) {
			if (tools_.contains(tool))
				selectedTools_.append(tool);
		}

		emit selectedToolsChanged(selectedTools_);
	}
}

void AMScanViewToolBarOptions::addSelectedTool(MPlotAbstractTool *newTool)
{
	if (newTool && !selectedTools_.contains(newTool)) {

		if (exclusiveSelection_)
			selectedTools_.clear();

		if (!tools_.contains(newTool))
			addTool(newTool);

		selectedTools_.append(newTool);

		emit selectedToolsChanged(selectedTools_);
	}
}

void AMScanViewToolBarOptions::removeSelectedTool(MPlotAbstractTool *tool)
{
	if (tool && selectedTools_.contains(tool)) {
		selectedTools_.removeOne(tool);
		emit selectedToolsChanged(selectedTools_);
	}
}

void AMScanViewToolBarOptions::setExclusiveSelectionEnabled(bool isEnabled)
{
	if (exclusiveSelection_ != isEnabled) {
		exclusiveSelection_ = isEnabled;
		emit exclusiveSelectionEnabledChanged(exclusiveSelection_);
	}
}


void AMScanViewToolBarOptions::setXUnits(const QString &newUnits)
{
	if (xUnits_ != newUnits) {
		xUnits_ = newUnits;
		emit xUnitsChanged(xUnits_);
	}
}

void AMScanViewToolBarOptions::setYUnits(const QString &newUnits)
{
	if (yUnits_ != newUnits) {
		yUnits_ = newUnits;
		emit yUnitsChanged(yUnits_);
	}
}

void AMScanViewToolBarOptions::setValueUnits(const QString &newUnits)
{
	if (valueUnits_ != newUnits) {
		valueUnits_ = newUnits;
		emit valueUnitsChanged(valueUnits_);
	}
}

void AMScanViewToolBarOptions::dragZoomerToolCheck()
{
	// Make sure that any drag zoomer tools in the list appear first.
	// They should always be added to a plot first.

	int toolCount = tools_.size();
	int dragZoomerCount = 0;

	for (int toolIndex = 0; toolIndex < toolCount; toolIndex++) {
		MPlotDragZoomerTool *dragZoomer = qobject_cast<MPlotDragZoomerTool*>(tools_.at(toolIndex));

		if (dragZoomer) {
			if (toolIndex > dragZoomerCount)
				tools_.swap(dragZoomerCount, toolIndex);
			dragZoomerCount++;
		}
	}
}










AMScanViewToolBarDataPositionView::AMScanViewToolBarDataPositionView(MPlotDataPositionTool *tool, bool enableValue, QWidget *parent) :
	QWidget(parent)
{
	// Initialize member variables.

	tool_ = 0;
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

	setTool(tool);
	setValueEnabled(enableValue);
}

AMScanViewToolBarDataPositionView::~AMScanViewToolBarDataPositionView()
{

}

void AMScanViewToolBarDataPositionView::setTool(MPlotDataPositionTool *newTool)
{
	if (tool_ != newTool) {

		if (tool_) {
			tool_->disconnect();
		}

		tool_ = newTool;

		if (tool_) {
			connect( tool_->plot()->signalSource(), SIGNAL(dataPositionChanged(QPointF)), this, SLOT(setDataPosition(QPointF)) );
		}

		emit toolChanged(tool_);
	}
}

void AMScanViewToolBarDataPositionView::setValueEnabled(bool isEnabled)
{
	if (valueEnabled_ != isEnabled) {
		valueEnabled_ = isEnabled;
		valueDisplay_->setVisible(valueEnabled_);
		emit valueEnabledChanged(valueEnabled_);
	}
}

void AMScanViewToolBarDataPositionView::setDataPosition(const QPointF &newPoint)
{
	position_->setText(dataPositionToString(newPoint));
}

void AMScanViewToolBarDataPositionView::setValue(double newValue)
{
	value_->setText(valueToString(newValue));
}

QString AMScanViewToolBarDataPositionView::dataPositionToString(const QPointF &newPoint, const QString &xUnits, const QString &yUnits) const
{
	QString text = "(" + QString::number(newPoint.x(), 'f', 3) + " " + xUnits + ", " + QString::number(newPoint.y(), 'f', 3) + " " + yUnits + ")";
	return text;
}

QString AMScanViewToolBarDataPositionView::valueToString(double value, const QString &units) const
{
	QString text = QString::number(value, 'f', 3) + " " + units;
	return text;
}
