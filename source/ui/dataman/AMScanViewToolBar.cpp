#include "AMScanViewToolBar.h"
#include "MPlot/MPlot.h"
#include "MPlot/MPlotAbstractTool.h"
#include "MPlot/MPlotTools.h"
#include "dataman/AMScanSetModel.h"

#include <QDebug>

AMScanViewToolBar::AMScanViewToolBar(AMScanViewToolBarOptions *toolOptions, QWidget *parent) :
	QGroupBox(parent)
{
	// Initialize member variables.

	toolOptions_ = 0;

	// Create UI elements.

	toolBar_ = new QToolBar("Plot");

	// Create and set layouts.

	toolViewsLayout_ = new QHBoxLayout();

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(toolBar_);
	layout->addLayout(toolViewsLayout_);
	layout->addStretch();

	setLayout(layout);

	// Make connections.

	connect( toolBar_, SIGNAL(actionTriggered(QAction*)), this, SLOT(onToolBarToolSelected(QAction*)) );

	// Current settings.

	setToolOptions(toolOptions);

	qDebug() << "\n";
}

AMScanViewToolBar::~AMScanViewToolBar()
{

}

void AMScanViewToolBar::setToolOptions(AMScanViewToolBarOptions *newOptions)
{
	if (toolOptions_ != newOptions) {

		qDebug() << "Setting tool bar tool options...";

		if (toolOptions_)
			toolOptions_->disconnect();

		toolOptions_ = newOptions;

		if (toolOptions_) {
			connect( toolOptions_, SIGNAL(toolsChanged(QList<MPlotAbstractTool*>)), this, SLOT(onToolOptionsToolsChanged()) );
			connect( toolOptions_, SIGNAL(selectedToolsChanged(QList<MPlotAbstractTool*>)), this, SLOT(onToolOptionsSelectedToolsChanged()) );
		}

		onToolOptionsToolsChanged();
		onToolOptionsSelectedToolsChanged();

		qDebug() << "Tool bar tool options set successfully.";

		emit toolOptionsChanged(toolOptions_);
	}
}

void AMScanViewToolBar::updateToolActionsChecked()
{
	if (toolOptions_) {

		toolBar_->blockSignals(true);

		qDebug() << "Updating tool actions checked.";

		foreach (QAction *toolAction, toolBar_->actions()) {
			if (toolAction && toolOptions_->isSelectedTool(toolAction->text()))
				toolAction->setChecked(true);
			else
				toolAction->setChecked(false);
		}

		toolBar_->blockSignals(false);
	}
}

void AMScanViewToolBar::updateToolViewsVisibility()
{
	if (toolOptions_) {

		qDebug() << "Updating tool view visibility.";

		for (int i = 0; i < toolOptions_->tools().count(); i++) {
			MPlotAbstractTool *tool = toolOptions_->tools().at(i);
			QWidget *toolView = toolViews_.at(i);

			if (toolView) {
				if (toolOptions_->isSelectedTool(tool)) {
					qDebug() << "Showing tool view.";
					toolView->show();
				} else {
					qDebug() << "Hiding tool view.";
					toolView->hide();
				}
			}
		}
	}
}

void AMScanViewToolBar::onToolBarToolSelected(QAction *action)
{
	if (toolOptions_ && action) {

		qDebug() << "Toolbar tool selected.";

		// Identify the tool associated with the given action.

		MPlotAbstractTool *tool = actionToTool(action);

		// If the action is checked, we are applying this tool, removing it otherwise.

		if (tool) {
			if (action->isChecked()) {
				toolOptions_->addSelectedTool(tool);

			} else {
				toolOptions_->removeSelectedTool(tool);
			}
		}
	}
}

void AMScanViewToolBar::onToolOptionsToolsChanged()
{
	// Clear the view.

	toolBar_->clear();

	foreach (QWidget *toolView, toolViews_) {
		toolViewsLayout_->removeWidget(toolView);
		toolViews_.removeOne(toolView);
		toolView->deleteLater();
	}

	// If we have valid tool options, build the view up with the available tools.

	if (toolOptions_) {

		qDebug() << "Setting up toolbar view for new tools...";

		foreach (MPlotAbstractTool *tool, toolOptions_->tools()) {

			// Create actions for each available tool and add to tool bar.

			QAction *toolAction = createToolAction(tool);
			toolBar_->addAction(toolAction);

			// Create tool view for each tool and add to list of tool views.

			QWidget *toolView = createToolView(tool);
			toolViews_ << toolView;
		}

		// Add list of generated tool views to the layout and update their visibility to correspond to tool select state.

		foreach (QWidget *toolView, toolViews_)
			toolViewsLayout_->addWidget(toolView);

		updateToolActionsChecked();
		updateToolViewsVisibility();

		qDebug() << "Toolbar view setup with new tools.";
	}
}

void AMScanViewToolBar::onToolOptionsSelectedToolsChanged()
{
	// Update the check state of each action and visibility of tool views by comparing against the selected tools.

	updateToolActionsChecked();
	updateToolViewsVisibility();
}

QAction* AMScanViewToolBar::createToolAction(MPlotAbstractTool *tool)
{
	QAction *toolAction = 0;

	if (tool) {

		qDebug() << "Creating tool action.";

		toolAction = new QAction(QIcon(":/22x22/view-list-details-symbolic.png"), tool->name(), this);
		toolAction->setCheckable(true);
		toolAction->setStatusTip(tool->description());
	}

	return toolAction;
}

QWidget* AMScanViewToolBar::createToolView(MPlotAbstractTool *tool)
{
	QWidget *view = 0;

	if (tool) {

		qDebug() << "Creating tool view.";

		MPlotDataPositionTool *positionTool = qobject_cast<MPlotDataPositionTool*>(tool);

		if (positionTool) {
			view = new AMScanViewToolBarDataPositionView(toolOptions_, false, this);
			qDebug() << "\tCreating data position tool view.";

		} else {
			view = new QWidget(this);
			qDebug() << "\tCreating other tool view.";
		}
	}

	return view;
}

QAction* AMScanViewToolBar::toolToAction(MPlotAbstractTool *tool)
{
	QAction *result = 0;

	if (toolOptions_ && tool) {

		foreach (QAction *action, toolBar_->actions()) {
			if (action->text() == tool->name()) {
				result = action;
				break;
			}
		}
	}

	return result;
}

MPlotAbstractTool* AMScanViewToolBar::actionToTool(QAction *toolAction)
{
	MPlotAbstractTool *result = 0;

	if (toolOptions_ && toolAction) {
		int toolIndex = 0;
		int toolCount = toolOptions_->tools().count();

		while (!result && toolIndex < toolCount) {
			MPlotAbstractTool *tool = toolOptions_->tools().at(toolIndex);

			if (tool->name() == toolAction->text())
				result = tool;

			toolIndex++;
		}
	}

	return result;
}








AMScanViewToolBarOptions::AMScanViewToolBarOptions(QList<MPlotAbstractTool *> tools, bool exclusiveSelection, QObject *parent) :
	QObject(parent)
{
	// Initialize member variables.

	exclusiveSelection_ = false;
	dataPosition_ = QPointF();
	xUnits_ = "";
	yUnits_ = "";
	value_ = 0;
	valueUnits_ = "";

	// Current settings.

	setTools(tools);
	setExclusiveSelectionEnabled(exclusiveSelection);
}

AMScanViewToolBarOptions::~AMScanViewToolBarOptions()
{

}

bool AMScanViewToolBarOptions::isSelectedTool(MPlotAbstractTool *tool)
{
	return selectedTools_.contains(tool);
}

bool AMScanViewToolBarOptions::isSelectedTool(const QString &toolName)
{
	int toolIndex = 0;
	int toolCount = selectedTools_.count();
	bool toolFound = false;

	while (!toolFound && toolIndex < toolCount) {
		MPlotAbstractTool *tool = selectedTools_.at(toolIndex);

		if (tool->name() == toolName)
			toolFound = true;

		toolIndex++;
	}

	return toolFound;
}

void AMScanViewToolBarOptions::setTools(QList<MPlotAbstractTool *> newTools)
{
	if (tools_ != newTools) {

		// Clear any previously selected tools.

		if (!tools_.isEmpty())
			selectedTools_.clear();

		tools_ = newTools;

		emit toolsChanged(tools_);
	}
}

void AMScanViewToolBarOptions::addTool(MPlotAbstractTool *newTool)
{
	if (!tools_.contains(newTool)) {
		tools_.append(newTool);
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

		dragZoomerToolCheck(selectedTools_);

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

		dragZoomerToolCheck(selectedTools_);

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

		if (exclusiveSelection_ && selectedTools_.count() > 1)
			setSelectedTools(QList<MPlotAbstractTool*>() << selectedTools_.first());

		emit exclusiveSelectionEnabledChanged(exclusiveSelection_);
	}
}

void AMScanViewToolBarOptions::setDataPosition(const QPointF &newPosition)
{
	if (dataPosition_ != newPosition) {
		qDebug() << "Tool bar options position update.";

		dataPosition_ = newPosition;
		emit dataPositionChanged(dataPosition_);
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

void AMScanViewToolBarOptions::setValue(double newValue)
{
	if (value_ != newValue) {
		value_ = newValue;
		emit valueChanged(value_);
	}
}

void AMScanViewToolBarOptions::setValueUnits(const QString &newUnits)
{
	if (valueUnits_ != newUnits) {
		valueUnits_ = newUnits;
		emit valueUnitsChanged(valueUnits_);
	}
}

void AMScanViewToolBarOptions::dragZoomerToolCheck(QList<MPlotAbstractTool*> toolList)
{
	// Make sure that any drag zoomer tools in the list appear first.
	// They should always be added to a plot first.

	int toolCount = toolList.size();
	int dragZoomerCount = 0;

	for (int toolIndex = 0; toolIndex < toolCount; toolIndex++) {
		MPlotDragZoomerTool *dragZoomer = qobject_cast<MPlotDragZoomerTool*>(toolList.at(toolIndex));

		if (dragZoomer) {
			if (toolIndex > dragZoomerCount)
				toolList.swap(dragZoomerCount, toolIndex);
			dragZoomerCount++;
		}
	}
}










AMScanViewToolBarDataPositionView::AMScanViewToolBarDataPositionView(AMScanViewToolBarOptions *toolOptions, bool enableValue, QWidget *parent) :
	QWidget(parent)
{
	// Initialize member variables.

	toolOptions_ = 0;
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

	setToolOptions(toolOptions);
	setValueEnabled(enableValue);
}

AMScanViewToolBarDataPositionView::~AMScanViewToolBarDataPositionView()
{

}

void AMScanViewToolBarDataPositionView::setToolOptions(AMScanViewToolBarOptions *newOptions)
{
	if (toolOptions_ != newOptions) {

		if (toolOptions_)
			toolOptions_->disconnect();

		toolOptions_ = newOptions;

		if (toolOptions_) {
			connect( toolOptions_, SIGNAL(dataPositionChanged(QPointF)), this, SLOT(updatePositionDisplay()) );
			connect( toolOptions_, SIGNAL(xUnitsChanged(QString)), this, SLOT(updatePositionDisplay()) );
			connect( toolOptions_, SIGNAL(yUnitsChanged(QString)), this, SLOT(updatePositionDisplay()) );
			connect( toolOptions_, SIGNAL(valueChanged(double)), this, SLOT(updateValueDisplay()) );
			connect( toolOptions_, SIGNAL(valueUnitsChanged(QString)), this, SLOT(updateValueDisplay()) );
		}

		emit toolOptionsChanged(toolOptions_);
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

void AMScanViewToolBarDataPositionView::updatePositionDisplay()
{
	if (toolOptions_) {
		position_->setText(dataPositionToString(toolOptions_->dataPosition(), toolOptions_->xUnits(), toolOptions_->yUnits()));
	}
}

void AMScanViewToolBarDataPositionView::updateValueDisplay()
{
	if (toolOptions_) {
		value_->setText(valueToString(toolOptions_->value(), toolOptions_->valueUnits()));
	}
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
