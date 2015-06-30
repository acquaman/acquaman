#ifndef AMDATAPOSITIONTOOL_H
#define AMDATAPOSITIONTOOL_H

#include "MPlot/MPlotTools.h"

class AMDataPositionTool : public MPlotDataPositionTool
{
	Q_OBJECT

public:
	/// Constructor.
	AMDataPositionTool(bool cursorVisible = false, bool useSelectionRect = true);
	/// Destructor.
	virtual ~AMDataPositionTool();

	/// Returns the cursor's visibility.
	bool cursorVisibility() const { return cursorVisibility_; }
	/// Returns the cursor's coordinates.
	QPointF cursorCoordinates() const { return cursorCoordinates_; }
	/// Returns the cursor's color.
	QColor cursorColor() const { return cursorColor_; }
	/// Returns the units associated with the indicator's current position.
	QStringList units() const { return units_; }

signals:
	/// Notifier that the cursor visibility has changed.
	void cursorVisibilityChanged(bool isVisible);
	/// Notifier that the cursor coordinates have changed.
	void cursorCoordinatesChanged(const QPointF &newCoordinates);
	/// Notifier that the cursor color has changed.
	void cursorColorChanged(const QColor &newColor);
	/// Notifier that the associated with the indicator position have changed.
	void unitsChanged(const QStringList &units);

public slots:
	/// Sets the visibility for the cursor.
	void setCursorVisibility(bool isVisible);
	/// Sets the cursor's coordinates.
	void setCursorCoordinates(const QPointF &coordinates);
	/// Sets the cursor's color.
	void setCursorColor(const QColor &newColor);
	/// Sets the units associated with the indicator position.
	void setUnits(const QStringList &newUnits);

	/// Applies the cached cursor visibility.
	void updateCursorVisibility();
	/// Applies the cached cursor coordinates.
	void updateCursorCoordinates();
	/// Applies the cached cursor color.
	void updateCursorColor();

protected slots:
	/// Applies the given cursor visibility.
	void applyCursorVisibility(bool isVisible);
	/// Applies the given cursor coordinates.
	void applyCursorCoordinates(const QPointF &newCoordinates);
	/// Applies the given cursor color.
	void applyCursorColor(const QColor &newColor);

protected:
	/// The cursor's visibility.
	bool cursorVisibility_;
	/// The cursor's coordinates.
	QPointF cursorCoordinates_;
	/// The cursor's color.
	QColor cursorColor_;
	/// The units associated with the indicator position.
	QStringList units_;
};

#endif // AMDATAPOSITIONTOOL_H
