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
	explicit AMDataPositionToolView(MPlotDataPositionTool *tool, QWidget *parent = 0, bool view2D = false);
	/// Destructor.
	virtual ~AMDataPositionToolView();
	/// Returns the tool being viewed.
	MPlotDataPositionTool* tool() const { return tool_; }
	/// Returns flag for whether the displayed position is being formatted for 2D viewing.
	bool view2D() const { return view2D_; }

signals:
	/// Notifier that the tool being viewed has changed.
	void toolChanged(MPlotDataPositionTool *newTool);
	/// Notifier that the flag for whether the displayed position is being formatted for 2D viewing has changed.
	void view2DChanged(bool use2D);

public slots:
	/// Sets the tool.
	void setTool(MPlotDataPositionTool *newTool);
	/// Sets the flag for whether the displayed position should be formatted for 2D viewing.
	void setView2D(bool use2D);

	/// Clears the display.
	virtual void clear();
	/// Updates the display.
	virtual void update();
	/// Clears and then updates the display.
	virtual void refresh();

protected:
	/// Returns a string representation of the given 1D position.
	static QString positionToString(double value, const QString &units);
	/// Returns a string representation of the given 2D position.
	static QString positionToString(const QPointF &values, const QStringList &units);

protected:
	/// The tool being viewed.
	MPlotDataPositionTool *tool_;
	/// Flag containing whether the displayed position should be formatted for multidimensions.
	bool view2D_;

	/// The label containing the value prompt.
	QLabel *valuePrompt_;
	/// The label containing the current value.
	QLabel *value_;
};




#include <QCheckBox>
#include <QDoubleSpinBox>
#include "ui/dataman/AMColorPickerButton.h"
#include "ui/AMPlotMarkerComboBox.h"

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
	/// Handles updating the tool when the cursor position spinbox reports a new position.
	void onPositionChanged();
	/// Handles updating the tool when the cursor marker combobox reports a new marker.
	void onMarkerChanged();
	/// Handles updating the tool when the cursor color button reports a new color available.
	void onColorChanged();
	/// Handles updating the tool when the cursor checkbox is clicked.
	void onVisibilityChanged();

	/// Handles updating the view to match the tool's value and units.
	void updatePositionLabel();
	/// Handles updating the view to match the tool's value and units.
	void updatePositionBoxes();
	/// Handles updating the view to match the tool's marker.
	void updateMarkerComboBox();
	/// Handles updating the view to match the tool's color.
	void updateColorButton();
	/// Handles updating the view to match the tool's visibility.
	void updateVisibility();

protected:
	/// Returns a string representation of the given 1D position.
	QString positionToString(double value, const QString &units) const;
	/// Returns a string representation of the given 2D position.
	QString positionToString(const QPointF &values, const QStringList &units) const;

protected:
	/// The tool being viewed.
	MPlotDataPositionCursorTool *tool_;

	/// The position label.
	QLabel *positionLabel_;
	/// The spinbox displaying the cursor x position.
	QDoubleSpinBox *xPositionBox_;
	/// The spinbox displaying the cursor y position.
	QDoubleSpinBox *yPositionBox_;
	/// The plot marker combo box.
	AMPlotMarkerComboBox *markerComboBox_;
	/// The button that displays and selects the cursor color.
	AMColorPickerButton *colorButton_;
	/// The checkbox that displays cursor visibility.
	QCheckBox *visibilityCheckBox_;
};

#endif // AMSCANVIEWPLOTTOOLVIEW_H
