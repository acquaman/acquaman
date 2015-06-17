#ifndef AMSCANVIEWTOOLBAR_H
#define AMSCANVIEWTOOLBAR_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QLayout>

class MPlot;

class AMScanViewToolBar : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMScanViewToolBar(MPlot *plot, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMScanViewToolBar();

	/// Returns the plot being edited.
	MPlot* plot() const { return plot_; }

signals:
	/// Notifier that the plot being edited has changed.
	void plotChanged(MPlot *newPlot);

public slots:
	/// Sets the plot to be edited.
	void setPlot(MPlot *newPlot);
	/// Sets the list of available tool names.
	void setToolNames(const QStringList &names);

protected:
	/// Handles updating the plot with the selected tool.
	void onToolBoxItemActivated(const QString &text);

protected:
	/// The plot being edited.
	MPlot *plot_;

	/// The combo box displaying the currently active tool, and containing all available tool options.
	QComboBox *toolBox_;
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
