#ifndef AMSCANVIEWPLOTTOOLVIEW_H
#define AMSCANVIEWPLOTTOOLVIEW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>

#include "MPlot/MPlotTools.h"

class AMDataPositionToolView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMDataPositionToolView(MPlotDataPositionTool *tool, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMDataPositionToolView();
	/// Returns the tool being viewed.
	MPlotDataPositionTool* tool() const { return tool_; }

signals:
	/// Notifier that the tool being viewed has changed.
	void toolChanged(MPlotDataPositionTool *newTool);

public slots:
	/// Sets the tool.
	void setTool(MPlotDataPositionTool *newTool);

	/// Clears the display.
	virtual void clear();
	/// Updates the display.
	virtual void update();
	/// Clears and then updates the display.
	virtual void refresh();

protected slots:
	/// Handles updating the view to match the tool's current position.
	void onToolPositionChanged();

protected:
	/// Returns a string representation of the given position.
	QString positionToString(const QPointF &values, const QStringList &units);

protected:
	/// The tool being viewed.
	MPlotDataPositionTool *tool_;

	/// The label containing the value prompt.
	QLabel *valuePrompt_;
	/// The label containing the current value.
	QLabel *value_;
	/// The box containing all value-related UI elements.
	QWidget *valueBox_;
};




#include <QCheckBox>
#include <QDoubleSpinBox>
#include "ui/dataman/AMColorPickerButton.h"

class AMDataPositionCursorToolView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMDataPositionCursorToolView(MPlotDataPositionCursorTool *tool, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMDataPositionCursorToolView();
	/// Returns the tool being viewed.
	MPlotDataPositionCursorTool* tool() const { return tool_; }

signals:
	/// Notifier that the tool being viewed has changed.
	void toolChanged(MPlotDataPositionTool *newTool);

public slots:
	/// Sets the tool being viewed.
	void setTool(MPlotDataPositionCursorTool *newTool);

	/// Clears the display.
	virtual void clear();
	/// Updates the display.
	virtual void update();
	/// Clears and then updates the display.
	virtual void refresh();

protected slots:
	/// Handles updating the tool when the cursor color button reports a new color available.
	void onColorChanged();
	/// Handles updating the tool when the cursor checkbox is clicked.
	void onVisibilityChanged();
	/// Handles updating the tool when the cursor position spinbox reports a new position.
	void onPositionChanged();

	/// Handles updating the view when the tool indicates the visibility has changed.
	void onToolVisibilityChanged();
	/// Handles updating the view when the tool indicates the color has changed.
	void onToolColorChanged();
	/// Handles updating the view when the tool indicates the position has changed.
	void onToolPositionChanged();

protected:
	/// The tool being viewed.
	MPlotDataPositionCursorTool *tool_;

	/// The position view.
	AMDataPositionToolView *positionView_;
	/// The checkbox that displays cursor visibility.
	QCheckBox *visibilityCheckBox_;
	/// The double spinbox that displays the cursor position.
	QDoubleSpinBox *positionSpinBox_;
	/// The button that displays and selects the cursor color.
	AMColorPickerButton *colorButton_;
};

#endif // AMSCANVIEWPLOTTOOLVIEW_H
