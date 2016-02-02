#include "AMScanViewPlotTools.h"
#include "MPlot/MPlotTools.h"

AMScanViewPlotTools::AMScanViewPlotTools(const QList<MPlotAbstractTool *> &tools, bool exclusiveSelection, QObject *parent) :
	QObject(parent)
{
	// Initialize member variables.

	exclusiveSelection_ = false;

	// Current settings.

	setTools(tools);
	setExclusiveSelectionEnabled(exclusiveSelection);
}

AMScanViewPlotTools::~AMScanViewPlotTools()
{

}

bool AMScanViewPlotTools::isSelectedTool(MPlotAbstractTool *tool)
{
	return selectedTools_.contains(tool);
}

bool AMScanViewPlotTools::isSelectedTool(const QString &toolName)
{
	int toolIndex = 0;
	int toolCount = selectedTools_.count();
	bool toolFound = false;

	// An assumption is made here that the tool names are unique.

	while (!toolFound && toolIndex < toolCount) {
		MPlotAbstractTool *tool = selectedTools_.at(toolIndex);

		if (tool->name() == toolName)
			toolFound = true;

		toolIndex++;
	}

	return toolFound;
}

void AMScanViewPlotTools::setTools(const QList<MPlotAbstractTool *> &newTools)
{
	// Clear selected tools.
	clearSelectedTools();

	// Set new tools
	setToolsList(newTools);
}

void AMScanViewPlotTools::addTool(MPlotAbstractTool *newTool)
{
	if (!tools_.contains(newTool)) {

		// We can preserve the selected tools.

		QList<MPlotAbstractTool*> selection = selectedTools_;

		// Add new tool.

		QList<MPlotAbstractTool*> tools = tools_;
		tools << newTool;
		setTools(tools);

		// Restore selection.

		setSelectedTools(selection);
	}
}

void AMScanViewPlotTools::removeTool(MPlotAbstractTool *tool)
{
	if (tools_.contains(tool)) {

		// Remove the tool from selection, if it's selected.

		if (selectedTools_.contains(tool))
			removeSelectedTool(tool);

		// We can preserve the current selection.

		QList<MPlotAbstractTool*> selection = selectedTools_;

		// Remove tool.

		QList<MPlotAbstractTool*> tools = tools_;
		tools.removeOne(tool);

		// Set new list of tools.

		setTools(tools);

		// Restore selection.

		setSelectedTools(selection);
	}
}

void AMScanViewPlotTools::clearTools()
{
	setTools(QList<MPlotAbstractTool*>());
}

void AMScanViewPlotTools::setSelectedTools(QList<MPlotAbstractTool *> &newTools)
{
	if (selectedTools_ != newTools) {

		QList<MPlotAbstractTool*> newSelection;

		// If we have an exclusive selection set, only interested in the most recent selection.

		if (exclusiveSelection_)
			newTools = applyExclusiveToolRule(newTools);

		// Check that each of the new tools exists in the master tool options list before adding them to selection.

		foreach (MPlotAbstractTool *tool, newTools) {
			if (tools_.contains(tool))
				newSelection.append(tool);
		}

		// Check that any instances of 'drag zoomer' tools appear first in the selection.

		newSelection = applyDragZoomerRule(newSelection);

		// Set new selection list.

		setSelectedToolsList(newSelection);
	}
}

void AMScanViewPlotTools::addSelectedTool(MPlotAbstractTool *newTool)
{
	if (newTool && tools_.contains(newTool) && !selectedTools_.contains(newTool)) {
		QList<MPlotAbstractTool*> newSelection = selectedTools_;
		newSelection << newTool;
		setSelectedTools(newSelection);
	}
}

void AMScanViewPlotTools::removeSelectedTool(MPlotAbstractTool *tool)
{
	if (tool && selectedTools_.contains(tool)) {
		QList<MPlotAbstractTool*> newSelection = selectedTools_;
		newSelection.removeOne(tool);
		setSelectedTools(newSelection);
	}
}

void AMScanViewPlotTools::clearSelectedTools()
{
	QList<MPlotAbstractTool*> emptySelection;
	setSelectedTools(emptySelection);
}

void AMScanViewPlotTools::setExclusiveSelectionEnabled(bool isEnabled)
{
	if (exclusiveSelection_ != isEnabled) {
		exclusiveSelection_ = isEnabled;

		if (exclusiveSelection_) {
			QList<MPlotAbstractTool*> newSelection = applyExclusiveToolRule(selectedTools_);
			setSelectedTools(newSelection);
		}

		emit exclusiveSelectionEnabledChanged(exclusiveSelection_);
	}
}

void AMScanViewPlotTools::setToolsList(const QList<MPlotAbstractTool *> &newTools)
{
	if (tools_ != newTools) {
		tools_ = newTools;
		emit toolsChanged(tools_);
	}
}

void AMScanViewPlotTools::setSelectedToolsList(const QList<MPlotAbstractTool *> &newSelection)
{
	if (selectedTools_ != newSelection) {
		selectedTools_ = newSelection;
		emit selectedToolsChanged(selectedTools_);
	}
}

QList<MPlotAbstractTool*> AMScanViewPlotTools::applyDragZoomerRule(QList<MPlotAbstractTool*> &toolList)
{
	// Make sure that any drag zoomer tools in the list appear first.
	// They should always be added to a plot first.

	int toolCount = toolList.count();
	int dragZoomerCount = 0;

	for (int toolIndex = 0; toolIndex < toolCount; toolIndex++) {
		MPlotDragZoomerTool *dragZoomer = qobject_cast<MPlotDragZoomerTool*>(toolList.at(toolIndex));

		if (dragZoomer) {

			if (toolIndex > dragZoomerCount)
				toolList.swap(dragZoomerCount, toolIndex);

			dragZoomerCount++;
		}
	}

	return toolList;
}

QList<MPlotAbstractTool*> AMScanViewPlotTools::applyExclusiveToolRule(QList<MPlotAbstractTool *> &toolList)
{
	if (toolList.count() > 1)
		toolList = QList<MPlotAbstractTool*>() << toolList.last();

	return toolList;
}

QList<MPlotAbstractTool*> AMScanViewPlotTools::applyPlotSelectorRule(QList<MPlotAbstractTool *> &toolList)
{
	// Find instance(s) of the plot selector tool.
	// If at least one is found, return new list containing just the last instance.

	int toolCount = toolList.count();
	int selectorIndex = -1;

	for (int toolIndex = 0; toolIndex < toolCount; toolIndex++) {
		MPlotPlotSelectorTool *selectorTool = qobject_cast<MPlotPlotSelectorTool*>(toolList.at(toolIndex));
		if (selectorTool) {
			selectorIndex = toolIndex;
		}
	}

	if (selectorIndex > -1) {
		toolList = QList<MPlotAbstractTool*>() << toolList.at(selectorIndex);
	}

	return toolList;
}

QList<MPlotAbstractTool*> AMScanViewPlotTools::applyDragZoomerDataPositionRule(QList<MPlotAbstractTool *> &toolList)
{
	// Find latest instance of either the drag zoomer or data position tools.
	// If more than one is found, remove all instances but the last.

	QList<MPlotAbstractTool*> toRemove;
	int toolCount = toolList.count();
	int lastToolIndex = -1;

	for (int toolIndex = 0; toolIndex < toolCount; toolIndex++) {
		MPlotDragZoomerTool *dragZoomerTool = qobject_cast<MPlotDragZoomerTool*>(toolList.at(toolIndex));
		MPlotDataPositionTool *dataPositionTool = qobject_cast<MPlotDataPositionTool*>(toolList.at(toolIndex));

		if (dragZoomerTool || dataPositionTool) {
			if (lastToolIndex >= 0) {
				toRemove << toolList.at(lastToolIndex);
			}

			lastToolIndex = toolIndex;
		}
	}

	foreach (MPlotAbstractTool *tool, toRemove) {
		toolList.removeOne(tool);
	}

	return toolList;
}





