/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
