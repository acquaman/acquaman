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
class AMScanViewToolBarDataPositionView;

class AMScanViewToolBar : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMScanViewToolBar(MPlot *plot, QList<MPlotAbstractTool*> toolOptions, bool exclusiveSelection = true, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMScanViewToolBar();

	/// Returns the plot being edited.
	MPlot* plot() const { return plot_; }
	/// Returns the list of available tool options.
	QList<MPlotAbstractTool*> tools() const { return tools_; }
	/// Returns whether multiple tools can be selected at once.
	bool exclusiveSelectionStatus() const { return exclusiveSelection_; }

signals:
	/// Notifier that the plot being edited has changed.
	void plotChanged(MPlot *newPlot);
	/// Notifier that the list of tool options has changed.
	void toolsChanged(QList<MPlotAbstractTool*> newTools);
	/// Notifier that the exclusive selection status has changed.
	void exclusiveSelectionStatusChanged(bool isExclusive);

public slots:
	/// Sets the plot to be edited.
	void setPlot(MPlot *newPlot);
	/// Sets the list of available tool options.
	void setTools(QList<MPlotAbstractTool*> newTools);
	/// Sets the exclusive selection status.
	void setExclusiveSelectionStatus(bool isExclusive);

protected slots:
	/// Handles applying the given action.
	void onToolSelected(QAction *action);

protected:
	/// Handles updating the plot with the selected tool.
	void onToolBoxItemActivated(const QString &text);

protected:
	/// The plot being edited.
	MPlot *plot_;
	/// The list of available tool options.
	QList<MPlotAbstractTool*> tools_;
	/// Bool indicating whether multiple tools can be selected at once.
	bool exclusiveSelection_;

	/// Toolbar displaying the available plot tool options.
	QToolBar *toolBar_;
	/// Displays information for the data position plot tool.
	AMScanViewToolBarDataPositionView *dataPositionView_;
};



class AMScanViewToolBarDataPositionView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMScanViewToolBarDataPositionView(bool enableValue = false, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMScanViewToolBarDataPositionView();
	/// Returns whether the value display is enabled.
	bool valueEnabled() const { return valueEnabled_; }

signals:
	/// Notifier that the value display has been en/disabled.
	void valueEnabledChanged(bool isEnabled);

public slots:
	/// Sets whether the value is displayed.
	void setValueEnabled(bool isEnabled);
	/// Sets the position to be displayed.
	void setPosition(const QString &newPosition);
	/// Sets the value to be displayed. Sets enableValue to true if
	void setValue(const QString &newValue);

protected:
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
