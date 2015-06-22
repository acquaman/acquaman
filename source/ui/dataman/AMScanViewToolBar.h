#ifndef AMSCANVIEWTOOLBAR_H
#define AMSCANVIEWTOOLBAR_H

#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QSignalMapper>
#include <QToolBar>
#include <QAction>
#include <QLayout>
#include <QToolButton>
#include <QButtonGroup>

class MPlot;
class MPlotAbstractTool;
class MPlotDataPositionTool;
class AMScanViewPlotToolOptions;
class AMScanViewToolBarDataPositionView;

class AMScanViewToolBar : public QGroupBox
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMScanViewToolBar(AMScanViewPlotToolOptions *toolOptions, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMScanViewToolBar();

signals:
	/// Notifier that the tool options have changed.
	void toolOptionsChanged(AMScanViewPlotToolOptions *newOptions);

public slots:
	/// Sets the tool options.
	void setToolOptions(AMScanViewPlotToolOptions *newOptions);

protected slots:
	/// Updates the tool buttons' check state, based on the tool options' selected tools.
	void updateToolButtonsChecked();
	/// Updates the tool views visibility, based on the tool options selected tools.
	void updateToolViewsVisibility();

	/// Handles updates to the tool options as a result of a tool button click.
	void onToolButtonClicked(int buttonIndex);
	/// Handles updates to the view as a result of the tool options tools changing.
	void onToolOptionsToolsChanged();
	/// Handles updates to the view as a result of the tool options selected tools changing.
	void onToolOptionsSelectedToolsChanged();

protected:
	/// Returns a QToolButton suitable to represent the given tool.
	QToolButton* createToolButton(MPlotAbstractTool *tool);
	/// Returns a QWidget tool view suitable to view the given tool.
	QWidget* createToolView(MPlotAbstractTool *tool);

	/// Returns the tool button that corresponds to the given tool, zero if none found.
	QAbstractButton* toolToButton(MPlotAbstractTool *tool);
	/// Returns the tool that corresponds to the given button, zero if none found.
	MPlotAbstractTool* buttonToTool(QAbstractButton *button);
	/// Returns the icon that corresponds to the given tool, null icon if none found.
	QIcon toolToIcon(MPlotAbstractTool *tool);

protected:
	/// The tool bar options being displayed.
	AMScanViewPlotToolOptions *toolOptions_;

	/// Tool button grouping.
	QButtonGroup *toolButtons_;

	/// The view layout for the tool buttons.
	QHBoxLayout *toolButtonsLayout_;
	/// The view layout for the tool views.
	QHBoxLayout *toolViewsLayout_;
	/// List of the tool-specific views, corresponding to the tool options tools.
	QList<QWidget*> toolViews_;
	/// Displays information for the data position plot tool.
	AMScanViewToolBarDataPositionView *dataPositionView_;
};



class AMScanViewPlotToolOptions: public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMScanViewPlotToolOptions(QList<MPlotAbstractTool*> tools, bool exclusiveSelection = false, QObject *parent = 0);
	/// Destructor.
	virtual ~AMScanViewPlotToolOptions();
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

	/// Returns the list of values to display.
	QList<double> values() const { return values_; }
	/// Returns the list of units to display.
	QStringList units() const { return units_; }

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
	/// Notifier that the values to be displayed have changed.
	void valuesChanged(QList<double> newValues);
	/// Notifier that the units to be displayed have changed.
	void unitsChanged(const QStringList &units);

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
	/// Sets the values to be displayed.
	void setValues(const QPointF &newPosition);
	/// Sets the values to be displayed.
	void setValues(QList<double> newValues);
	/// Sets the units to be displayed.
	void setUnits(const QStringList &newUnits);

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

	/// The list of values to be displayed for a scan view mouse click.
	QList<double> values_;
	/// The list of units to be displayed for a scan view mouse click.
	QStringList units_;
};



class AMScanViewToolBarDataPositionView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMScanViewToolBarDataPositionView(AMScanViewPlotToolOptions *toolOptions, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMScanViewToolBarDataPositionView();
	/// Returns the tool options being viewed.
	AMScanViewPlotToolOptions* toolOptions() const { return toolOptions_; }

signals:
	/// Notifier that the tool options being viewed has changed.
	void toolOptionsChanged(AMScanViewPlotToolOptions *newOptions);

public slots:
	/// Sets the tool options being viewed.
	void setToolOptions(AMScanViewPlotToolOptions *newOptions);

	/// Updates the display.
	void updateDisplay();

protected:
	/// Returns a string representation of the given values and units.
	QString valuesToString(QList<double> values, const QStringList &units);

protected:
	/// The tool options being viewed.
	AMScanViewPlotToolOptions *toolOptions_;

	/// The label containing the current value.
	QLabel *value_;
	/// The box containing all value UI elements.
	QGroupBox *valueDisplay_;
};

#endif // AMSCANVIEWTOOLBAR_H
