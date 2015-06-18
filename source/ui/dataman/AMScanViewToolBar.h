#ifndef AMSCANVIEWTOOLBAR_H
#define AMSCANVIEWTOOLBAR_H

#include <QWidget>
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

class AMScanViewToolBar : public QWidget
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
	/// Updates the tool options selection.
	void onToolSelected(QAction *action);

	/// Uses the current tool options' x units to update the view.
	void updateXUnits();
	/// Uses the current tool options' y units to update the view.
	void updateYUnits();
	/// Uses the current tool options' value units to update the view.
	void updateValueUnits();

protected:
	/// The tool bar options being displayed.
	AMScanViewToolBarOptions *toolOptions_;

	/// Toolbar displaying the available plot tool options.
	QToolBar *toolBar_;
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

signals:
	/// Notifier that the list of tool options has changed.
	void toolsChanged(QList<MPlotAbstractTool*> newTools);
	/// Notifier that the list of selected/active tool options have changed.
	void selectedToolsChanged(QList<MPlotAbstractTool*> newTools);
	/// Notifier that the flag indicating whether or not exclusive tool selection is enabled has changed.
	void exclusiveSelectionEnabledChanged(bool isEnabled);
	/// Notifier that the x-axis units have been changed.
	void xUnitsChanged(const QString &newUnits);
	/// Notifier that the y-axis units have been changed.
	void yUnitsChanged(const QString &newUnits);
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
	/// Sets the x-axis units.
	void setXUnits(const QString &newUnits);
	/// Sets the y-axis units.
	void setYUnits(const QString &newUnits);
	/// Sets the value units.
	void setValueUnits(const QString &newUnits);

protected:
	/// Iterates through the tools list and swaps the order of tools s.t. instances of drag zoomer are always at the front of the list--they should always be added first to a plot.
	void dragZoomerToolCheck();

protected:
	/// The list of tool options.
	QList<MPlotAbstractTool*> tools_;
	/// The list of selected/active tool options.
	QList<MPlotAbstractTool*> selectedTools_;
	/// Flag indicating whether or not exclusive tool selection is enabled.
	bool exclusiveSelection_;
	/// The x-axis units to display.
	QString xUnits_;
	/// The y-units units to display.
	QString yUnits_;
	/// The value units to display.
	QString valueUnits_;
};



class AMScanViewToolBarDataPositionView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMScanViewToolBarDataPositionView(MPlotDataPositionTool *tool, bool enableValue = false, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMScanViewToolBarDataPositionView();
	/// Returns the tool being viewed.
	MPlotDataPositionTool* tool() const { return tool_; }
	/// Returns whether the value display is enabled.
	bool valueEnabled() const { return valueEnabled_; }

signals:
	/// Notifier that the tool being viewed has changed.
	void toolChanged(MPlotDataPositionTool *newTool);
	/// Notifier that the value display has been en/disabled.
	void valueEnabledChanged(bool isEnabled);

public slots:
	/// Sets the tool being viewed.
	void setTool(MPlotDataPositionTool *newTool);
	/// Sets whether the value is displayed.
	void setValueEnabled(bool isEnabled);
	/// Sets the data position to be displayed.
	void setDataPosition(const QPointF &newPoint);
	/// Sets the value to be displayed.
	void setValue(double newValue);

protected:
	/// Returns a string representation of the given data position.
	QString dataPositionToString(const QPointF &newPoint, const QString &xUnits, const QString &yUnits) const;
	/// Returns a string representation of the given value.
	QString valueToString(double value, const QString &units) const;

protected:
	/// The tool being viewed.
	MPlotDataPositionTool *tool_;
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
