#include "AMDataPositionTool.h"
#include "MPlot/MPlot.h"

AMDataPositionTool::AMDataPositionTool(bool cursorVisible, bool useSelectionRect) :
	MPlotDataPositionTool(useSelectionRect)
{
	// Initialize member variables.

	cursorVisibility_ = false;
	cursorCoordinates_ = QPointF();
	cursorColor_ = QColor(Qt::black);

	// Current settings.

	setCursorVisibility(cursorVisible);
}

AMDataPositionTool::~AMDataPositionTool()
{

}

void AMDataPositionTool::setCursorVisibility(bool isVisible)
{
	if (cursorVisibility_ != isVisible) {
		cursorVisibility_ = isVisible;

		updateCursorVisibility();

		emit cursorVisibilityChanged(cursorVisibility_);
	}
}

void AMDataPositionTool::setCursorCoordinates(const QPointF &coordinates)
{
	if (cursorCoordinates_ != coordinates) {
		cursorCoordinates_ = coordinates;

		updateCursorCoordinates();

		emit cursorCoordinatesChanged(cursorCoordinates_);
	}
}

void AMDataPositionTool::setCursorColor(const QColor &newColor)
{
	if (cursorColor_ != newColor) {
		cursorColor_ = newColor;

		updateCursorColor();

		emit cursorColorChanged(cursorColor_);
	}
}

void AMDataPositionTool::setUnits(const QStringList &newUnits)
{
	if (units_ != newUnits) {
		units_ = newUnits;
		emit unitsChanged(units_);
	}
}

void AMDataPositionTool::updateCursorVisibility()
{
	applyCursorVisibility(cursorVisibility_);
}

void AMDataPositionTool::updateCursorCoordinates()
{
	applyCursorCoordinates(cursorCoordinates_);
}

void AMDataPositionTool::updateCursorColor()
{
	applyCursorColor(cursorColor_);
}

void AMDataPositionTool::applyCursorVisibility(bool isVisible)
{
	if (plot()) {
		if (isVisible)
			setDataPositionIndicator(plot()->axisScaleBottom(), plot()->axisScaleLeft());
		else
			removeDataPositionIndicator();
	}
}

void AMDataPositionTool::applyCursorCoordinates(const QPointF &newCoordinates)
{
	if (indicator_)
		indicator_->setPos(newCoordinates);
}

void AMDataPositionTool::applyCursorColor(const QColor &newColor)
{
	if (indicator_ && indicator_->markerValid()) {
		indicator_->marker()->setPen(QPen(newColor));
		indicator_->marker()->setBrush(QBrush(newColor));
	}
}

