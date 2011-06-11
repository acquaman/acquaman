#include "ROIPlotMarker.h"
#include <QPainter>

ROIPlotMarker::ROIPlotMarker(QString name, double center, double low, double high)
	: MPlotItem(), pen_(Qt::gray), brush_(QColor(0, 150, 0, 100))
{
	name_ = name;
	center_ = center;
	low_ = low;
	high_ = high;
	setHighlighted(true);
}

void ROIPlotMarker::setHighlighted(bool highlight)
{
	if (highlight)
		brush_.setColor(QColor(138, 43, 226, 100));  // BlueViolet
	else
		brush_.setColor(QColor(0, 150, 0, 100)); // Light green

	isHighlighted_ = highlight;
	updatePlot();
}

QRectF ROIPlotMarker::boundingRect() const
{
	QRectF br = MPlotItem::boundingRect();

	// expand by the selection line width (in pixels...)
	QRectF hs = QRectF(0, 0, MPLOT_SELECTION_LINEWIDTH, MPLOT_SELECTION_LINEWIDTH);

	// expand by pen border size
	hs |= QRectF(0,0, pen_.widthF(), pen_.widthF());

	// really we just need 1/2 the marker size and 1/2 the selection highlight width. But extra doesn't hurt.
	br.adjust(-hs.width(),-hs.height(),hs.width(), hs.height());

	return br;
}

QRectF ROIPlotMarker::dataRect() const
{
	return QRectF(low_, 0, high_-low_, 0.95);
}

void ROIPlotMarker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	painter->save();
	painter->setPen(pen_);
	painter->setBrush(brush_);
	painter->drawRect(MPlotItem::boundingRect());
	painter->drawLine(QLineF(xAxisTarget()->mapDataToDrawing(center_), MPlotItem::boundingRect().top(), xAxisTarget()->mapDataToDrawing(center_), MPlotItem::boundingRect().bottom()));
	painter->restore();
}
