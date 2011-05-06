#include "ROIPlotMarker.h"
#include <QPainter>

ROIPlotMarker::ROIPlotMarker(QString name, double center, double low, double high, double height, double sx, double sy)
	: MPlotItem()
{
	name_ = name;
	center_ = center;
	low_ = low;
	high_ = high;
	height_ = height;
	sx_ = sx;
	sy_ = sy;
	markerColor_ = QColor(0, 150, 0, 100);
}

void ROIPlotMarker::setHighlighted(bool highlight)
{
	if (highlight)
		markerColor_ = QColor(138, 43, 226, 100);  // BlueViolet

	else
		markerColor_ = QColor(0, 150, 0, 100); // Light green

	isHighlighted_ = highlight;
}

QRectF ROIPlotMarker::boundingRect() const
{
	return QRectF(lowEnd(), 0, highEnd()-lowEnd(), height());
}

QRectF ROIPlotMarker::dataRect() const
{
	return boundingRect();
}

void ROIPlotMarker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	painter->save();
	painter->setBrush(QBrush(markerColor_));
	painter->setPen(QColor(Qt::gray));
	painter->setBackground(painter->brush());
	painter->drawLine(QLineF(center(), 0, center(), height()));
	painter->drawRect(boundingRect());
	painter->restore();
}
