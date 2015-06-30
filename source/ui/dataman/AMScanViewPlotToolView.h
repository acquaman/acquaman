#ifndef AMSCANVIEWPLOTTOOLVIEW_H
#define AMSCANVIEWPLOTTOOLVIEW_H

#include <QWidget>

class AMScanViewPlotToolView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMScanViewPlotToolView(QWidget *parent = 0);
	/// Destructor.
	virtual ~AMScanViewPlotToolView();

public slots:
	/// Clears the view.
	virtual void clear() { return; }
	/// Updates the view.
	virtual void update() { return; }
	/// Clears and then updates the view.
	virtual void refresh() { return; }
};









#include <QGroupBox>
#include <QLayout>
#include <QLabel>
#include <QCheckBox>

#include "dataman/AMDataPositionTool.h"

#include "ui/dataman/AMColorPickerButton.h"

class AMDataPositionToolView : public AMScanViewPlotToolView
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMDataPositionToolView(AMDataPositionTool *tool, QWidget *parent = 0, bool valueWidgetVisible = true, bool cursorWidgetVisible = true);
	/// Destructor.
	virtual ~AMDataPositionToolView();

signals:
	/// Notifier that the tool has changed.
	void toolChanged(AMDataPositionTool *newTool);
	/// Notifier that the visibility of the value-related UI elements has changed.
	void valueWidgetVisibilityChanged(bool isVisible);
	/// Notifier that the visibility of the cursor has changed.
	void cursorWidgetVisibilityChanged(bool isVisible);

public slots:
	/// Sets the tool.
	void setTool(AMDataPositionTool *newTool);

	/// Clears the display.
	virtual void clear();
	/// Updates the display.
	virtual void update();
	/// Clears and then updates the display.
	virtual void refresh();

	/// Sets the visibility of the value-related elements.
	void setValueWidgetVisibility(bool isVisible);
	/// Sets the visibility of the cursor-related elements.
	void setCursorWidgetVisibility(bool isVisible);

	/// Updates the value to display the tool's current value.
	void updateValues();
	/// Updates the cursor visibility to display the tool's current cursor visibility.
	void updateCursorVisibility();
	/// Updates the cursor color to display the tool's current cursor color.
	void updateCursorColor();

	/// Updates the value widget's current visibility.
	void updateValueWidgetVisibility();
	/// Updates the cursor widget's current visibility.
	void updateCursorWidgetVisibility();

protected slots:
	/// Handles updating the tool options when the cursor color button reports a new color available.
	void onCursorColorChanged();
	/// Handles updating the tool options when the cursor checkbox is clicked.
	void onCursorVisibilityClicked();

protected:
	/// Returns a string representation of the given values and units.
	QString valuesToString(const QPointF &values, const QStringList &units);

protected:
	/// The tool being viewed.
	AMDataPositionTool *tool_;
	/// Flag indicating whether the tool value information should be shown.
	bool valueWidgetShown_;
	/// Flag indicating whether the tool cursor information should be shown.
	bool cursorWidgetShown_;

	/// The label containing the value prompt.
	QLabel *valuePrompt_;
	/// The label containing the current value.
	QLabel *value_;
	/// The box containing all value-related UI elements.
	QGroupBox *valueBox_;

	/// The checkbox displaying the cursor visibility.
	QCheckBox *cursorVisibility_;
	/// The button that displays and selects the cursor color.
	AMColorPickerButton *cursorColor_;
	/// The box containing all cursor-related UI elements.
	QGroupBox *cursorBox_;
};

#endif // AMSCANVIEWPLOTTOOLVIEW_H
