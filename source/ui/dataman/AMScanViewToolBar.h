#ifndef AMSCANVIEWTOOLBAR_H
#define AMSCANVIEWTOOLBAR_H

#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QSignalMapper>
#include <QToolBar>
#include <QAction>
#include <QLayout>

class MPlot;
class MPlotAbstractTool;
class MPlotDataPositionTool;
class AMScanViewToolBarOptions;
class AMScanViewToolBarDataPositionView;

class AMScanViewToolBar : public QGroupBox
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMScanViewToolBar(AMScanViewToolBarOptions *toolOptions, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMScanViewToolBar();

signals:
	/// Notifier that the tool options have changed.
	void toolOptionsChanged(AMScanViewToolBarOptions *newOptions);

public slots:
	/// Sets the tool options.
	void setToolOptions(AMScanViewToolBarOptions *newOptions);

protected slots:
	/// Updates the tool actions check state, based on the tool options selected tools.
	void updateToolActionsChecked();
	/// Updates the tool views visibility, based on the tool options selected tools.
	void updateToolViewsVisibility();

	/// Handles updates to the tool options as a result of the tool bar selected tools changing.
	void onToolBarToolSelected(QAction *action);
	/// Handles updates to the view as a result of the tool options tools changing.
	void onToolOptionsToolsChanged();
	/// Handles updates to the view as a result of the tool options selected tools changing.
	void onToolOptionsSelectedToolsChanged();

protected:
	/// Returns a QAction suitable to represent the given tool.
	QAction* createToolAction(MPlotAbstractTool *tool);
	/// Returns a QWidget tool view suitable to view the given tool.
	QWidget* createToolView(MPlotAbstractTool *tool);

	/// Returns the tool action from the toolbar that corresponds to the given tool, zero if none found.
	QAction* toolToAction(MPlotAbstractTool *tool);
	/// Returns the tool that corresponds to the tiven tool action, zero if none found.
	MPlotAbstractTool* actionToTool(QAction *toolAction);

protected:
	/// The tool bar options being displayed.
	AMScanViewToolBarOptions *toolOptions_;

	/// Toolbar displaying the available plot tool options.
	QToolBar *toolBar_;
	/// The view layout for the tool-specific views.
	QHBoxLayout *toolViewsLayout_;
	/// List of the tool-specific views, corresponding to the tool options tools.
	QList<QWidget*> toolViews_;
	/// Displays information for the data position plot tool.
	AMScanViewToolBarDataPositionView *dataPositionView_;
};



class AMScanSetModel;
class AMScanViewToolBarOptions : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMScanViewToolBarOptions(QList<MPlotAbstractTool*> tools, bool exclusiveSelection = false, QObject *parent = 0);
	/// Destructor.
	virtual ~AMScanViewToolBarOptions();
	/// Returns the list of tool options.
	QList<MPlotAbstractTool*> tools() const { return tools_; }
	/// Returns the list of selected/active tool options.
	QList<MPlotAbstractTool*> selectedTools() const { return selectedTools_; }
	/// Returns the flag indicating whether or not exclusive tool selection is enabled.
	bool exclusiveSelectionEnabled() const { return exclusiveSelection_; }
	/// Returns the latest selected data position.
	QPointF dataPosition() const { return dataPosition_; }
	/// Returns the x units.
	QString xUnits() const { return xUnits_; }
	/// Returns the y units.
	QString yUnits() const { return yUnits_; }
	/// Returns the latest selected value.
	double value() const { return value_; }
	/// Returns the value units.
	QString valueUnits() const { return valueUnits_; }

	/// Returns true if the given tool is selected, false otherwise.
	bool isSelectedTool(MPlotAbstractTool *tool);
	/// Returns true if the given tool name corresponds to a selected tool, false otherwise.
	bool isSelectedTool(const QString &toolName);

signals:
	/// Notifier that the list of tool options has changed.
	void toolsChanged(QList<MPlotAbstractTool*> newTools);
	/// Notifier that the list of selected/active tool options have changed.
	void selectedToolsChanged(QList<MPlotAbstractTool*> newTools);
	/// Notifier that the flag indicating whether or not exclusive tool selection is enabled has changed.
	void exclusiveSelectionEnabledChanged(bool isEnabled);
	/// Notifier that the latest data position has changed.
	void dataPositionChanged(const QPointF &newPosition);
	/// Notifier that the x-axis units have been changed.
	void xUnitsChanged(const QString &newUnits);
	/// Notifier that the y-axis units have been changed.
	void yUnitsChanged(const QString &newUnits);
	/// Notifier that the latest selected value has changed.
	void valueChanged(double newValue);
	/// Notifier that the value units have been changed.
	void valueUnitsChanged(const QString &newUnits);

public slots:
	/// Sets the list of tool options.
	void setTools(QList<MPlotAbstractTool*> newTools);
	/// Adds the given tool to the list of tool options.
	void addTool(MPlotAbstractTool *newTool);
	/// Removes the given tool from the list of tool options.
	void removeTool(MPlotAbstractTool *tool);
	/// Sets the list of selected/active tool options.
	void setSelectedTools(QList<MPlotAbstractTool*> newTools);
	/// Adds the given tool to the list of selected tools.
	void addSelectedTool(MPlotAbstractTool *newTool);
	/// Removes the given tool from the list of selected tools.
	void removeSelectedTool(MPlotAbstractTool *tool);
	/// Sets the flag indicating whether or not exclusive tool selection is enabled.
	void setExclusiveSelectionEnabled(bool isEnabled);
	/// Sets the latest selected data position.
	void setDataPosition(const QPointF &newPosition);
	/// Sets the x-axis units.
	void setXUnits(const QString &newUnits);
	/// Sets the y-axis units.
	void setYUnits(const QString &newUnits);
	/// Sets the latest selected value.
	void setValue(double newValue);
	/// Sets the value units.
	void setValueUnits(const QString &newUnits);

protected:
	/// Iterates through the given tools list and swaps the order of tools s.t. instances of drag zoomer are always at the front of the list--they should always be added first to a plot.
	void dragZoomerToolCheck(QList<MPlotAbstractTool*> toolList);

protected:
	/// The list of tool options.
	QList<MPlotAbstractTool*> tools_;
	/// The list of selected/active tool options.
	QList<MPlotAbstractTool*> selectedTools_;
	/// Flag indicating whether or not exclusive tool selection is enabled.
	bool exclusiveSelection_;
	/// The latest selected data position.
	QPointF dataPosition_;
	/// The x-axis units to display.
	QString xUnits_;
	/// The y-units units to display.
	QString yUnits_;
	/// The latest selected value.
	double value_;
	/// The value units to display.
	QString valueUnits_;
};



class AMScanViewToolBarDataPositionView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMScanViewToolBarDataPositionView(AMScanViewToolBarOptions *toolOptions, bool enableValue = false, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMScanViewToolBarDataPositionView();
	/// Returns the tool options being viewed.
	AMScanViewToolBarOptions* toolOptions() const { return toolOptions_; }
	/// Returns whether the value display is enabled.
	bool valueEnabled() const { return valueEnabled_; }

signals:
	/// Notifier that the tool options being viewed has changed.
	void toolOptionsChanged(AMScanViewToolBarOptions *newOptions);
	/// Notifier that the value display has been en/disabled.
	void valueEnabledChanged(bool isEnabled);

public slots:
	/// Sets the tool options being viewed.
	void setToolOptions(AMScanViewToolBarOptions *newOptions);
	/// Sets whether the value is displayed.
	void setValueEnabled(bool isEnabled);

	/// Updates the position display.
	void updatePositionDisplay();
	/// Updates the value display.
	void updateValueDisplay();

protected:
	/// Returns a string representation of the given data position.
	QString dataPositionToString(const QPointF &newPoint, const QString &xUnits, const QString &yUnits) const;
	/// Returns a string representation of the given value.
	QString valueToString(double value, const QString &units) const;

protected:
	/// The tool options being viewed.
	AMScanViewToolBarOptions *toolOptions_;
	/// Bool indicating whether the value should be displayed.
	bool valueEnabled_;

	/// The label containing the current position.
	QLabel *position_;
	/// The box containing all position UI elements.
	QGroupBox *positionDisplay_;
	/// The label containing the current value.
	QLabel *value_;
	/// The box containing all value UI elements.
	QGroupBox *valueDisplay_;
};

#endif // AMSCANVIEWTOOLBAR_H
