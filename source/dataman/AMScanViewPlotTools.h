#ifndef AMSCANVIEWPLOTOPTIONS_H
#define AMSCANVIEWPLOTOPTIONS_H

#include <QObject>

#include "MPlot/MPlotAbstractTool.h"

class AMScanViewPlotTools : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMScanViewPlotTools(const QList<MPlotAbstractTool*> &tools, bool exclusiveSelection = false, QObject *parent = 0);
	/// Destructor.
	virtual ~AMScanViewPlotTools();

	/// Returns the list of tools, in the order in which they were given.
	QList<MPlotAbstractTool*> tools() const { return tools_; }
	/// Returns the list of selected/active tools, in the order in which they should be applied to a plot.
	QList<MPlotAbstractTool*> selectedTools() const { return selectedTools_; }
	/// Returns the flag indicating whether or not exclusive tool selection is enabled.
	bool exclusiveSelectionEnabled() const { return exclusiveSelection_; }

	/// Returns true if the given tool is selected, false otherwise.
	bool isSelectedTool(MPlotAbstractTool *tool);
	/// Returns true if the given tool name corresponds to a selected tool, false otherwise.
	bool isSelectedTool(const QString &toolName);

signals:
	/// Notifier that the list of tools has changed.
	void toolsChanged(const QList<MPlotAbstractTool*> &newTools);
	/// Notifier that the list of selected/active tool options have changed, in the order in which they should be applied to a plot.
	void selectedToolsChanged(const QList<MPlotAbstractTool*> &newTools);
	/// Notifier that the flag indicating whether or not exclusive tool selection is enabled has changed.
	void exclusiveSelectionEnabledChanged(bool isEnabled);

public slots:
	/// Sets the list of tool options.
	void setTools(const QList<MPlotAbstractTool*> &newTools);
	/// Adds the given tool to the tool list.
	void addTool(MPlotAbstractTool *newTool);
	/// Removes the given tool from the tool list.
	void removeTool(MPlotAbstractTool *tool);
	/// Clears the tool list.
	void clearTools();

	/// Sets the list of selected/active tool options, applying rules as needed.
	void setSelectedTools(QList<MPlotAbstractTool*> &newTools);
	/// Adds the given tool to the list of selected tools.
	void addSelectedTool(MPlotAbstractTool *newTool);
	/// Removes the given tool from the list of selected tools.
	void removeSelectedTool(MPlotAbstractTool *tool);
	/// Clears the selected tool list.
	void clearSelectedTools();

	/// Sets the flag indicating whether or not exclusive tool selection is enabled, applying rules as needed.
	void setExclusiveSelectionEnabled(bool isEnabled);

protected slots:
	/// Sets the tools list.
	void setToolsList(const QList<MPlotAbstractTool*> &newTools);
	/// Sets the selected tools list.
	void setSelectedToolsList(const QList<MPlotAbstractTool*> &newSelection);

protected:
	/// Iterates through the given tools list and swaps the order of tools s.t. instances of drag zoomer are always at the front of the list--they should always be added first to a plot.
	QList<MPlotAbstractTool*> applyDragZoomerRule(QList<MPlotAbstractTool*> &toolList);
	/// Applies the exclusive tool setting to the given list--returning a new list containing just the last item from the given list.
	QList<MPlotAbstractTool*> applyExclusiveToolRule(QList<MPlotAbstractTool*> &toolList);
	/// Iterates through the given tools list to find instances of MPlotPlotSelectorTool. If at least one is found, returns new list containing just the latest MPlotPlotSelectorTool instance. Otherwise, returns given list.
	QList<MPlotAbstractTool*> applyPlotSelectorRule(QList<MPlotAbstractTool*> &toolList);
	/// Iterates through the given tools list to find instances of MPlotDragZoomerTool and MPlotDataPositionTool. The two are exclusive--returns list where only the latest addition between the two is contained. Otherwise, returns original list.
	QList<MPlotAbstractTool*> applyDragZoomerDataPositionRule(QList<MPlotAbstractTool*> &toolList);

protected:
	/// The list of tools.
	QList<MPlotAbstractTool*> tools_;
	/// The list of selected/active tools.
	QList<MPlotAbstractTool*> selectedTools_;
	/// Flag indicating whether or not exclusive tool selection is enabled.
	bool exclusiveSelection_;
};

#endif // AMSCANVIEWPLOTOPTIONS_H
