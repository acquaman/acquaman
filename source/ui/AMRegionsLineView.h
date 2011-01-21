/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#ifndef AMREGIONSLINEVIEW_H
#define AMREGIONSLINEVIEW_H

#include <QtGui>
#include <QGraphicsItem>
#include "acquaman/AMRegionsList.h"


class RegionItem : public QGraphicsItem
{
public:
	RegionItem(double start, double delta, double end, double min, double max, int pixRange) : color(qrand() % 256, qrand() % 256, qrand() % 256)
	{
		setToolTip(QString("QColor(%1, %2, %3)\n%4")
				  .arg(color.red()).arg(color.green()).arg(color.blue())
				  .arg("Click and drag this color onto the robot!"));
		setCursor(Qt::OpenHandCursor);
		start_ = start;
		delta_ = delta;
		end_ = end;
		min_ = min;
		max_ = max;
		pixRange_ = pixRange;
		width_ = (int)floor( (end-start)/((max-min)/pixRange) );
	}

	QRectF boundingRect() const{
		return QRectF(0, -15, width_, 30);
	}
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
		Q_UNUSED(option);
		Q_UNUSED(widget);
		painter->setPen(Qt::NoPen);
		painter->setBrush(Qt::darkGray);
		painter->drawRoundedRect(0, -15, width_, 30, 1, 1, Qt::RelativeSize);
		painter->setPen(QPen(Qt::black, 1));
		painter->setBrush(Qt::gray);
		painter->drawRoundedRect(0, -15, width_, 30, 1, 1, Qt::RelativeSize);
		QString deltaVal;
		deltaVal.setNum(delta_);
		QChar deltaChar(0x0394);
		deltaVal.prepend(" = ");
		deltaVal.prepend(deltaChar);
		QRectF box(0, -15, width_, 30);
		painter->drawText(box, Qt::AlignHCenter, deltaVal, &box);
	}

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event){
		if (event->button() != Qt::LeftButton) {
			event->ignore();
			return;
		}

		setCursor(Qt::ClosedHandCursor);
	}
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event){
		if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length() < QApplication::startDragDistance()) {
			return;
		}

		QDrag *drag = new QDrag(event->widget());
		QMimeData *mime = new QMimeData;
		drag->setMimeData(mime);


		mime->setColorData(color);
		mime->setText(QString("#%1%2%3")
					  .arg(color.red(), 2, 16, QLatin1Char('0'))
					  .arg(color.green(), 2, 16, QLatin1Char('0'))
					  .arg(color.blue(), 2, 16, QLatin1Char('0')));

		QPixmap pixmap(60, 40);
		pixmap.fill(Qt::white);

		QPainter painter(&pixmap);
		painter.translate(26, 16);
		painter.setRenderHint(QPainter::Antialiasing);
		paint(&painter, 0, 0);
		painter.end();
		pixmap.setMask(pixmap.createHeuristicMask());

		drag->setPixmap(pixmap);
		drag->setHotSpot(QPoint(15, 20));


		drag->exec();
		setCursor(Qt::OpenHandCursor);
	}
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
		Q_UNUSED(event)

		setCursor(Qt::OpenHandCursor);
	}

private:
	QColor color;
	double start_;
	double delta_;
	double end_;
	double min_;
	double max_;
	int pixRange_;
	int width_;
};

class EnergyIndexItem : public QGraphicsItem
{
public:
	EnergyIndexItem(double energy, double min, double max, int pixRange) : color(qrand() % 256, qrand() % 256, qrand() % 256)
	{
		setToolTip(QString("QColor(%1, %2, %3)\n%4")
				  .arg(color.red()).arg(color.green()).arg(color.blue())
				  .arg("Click and drag this color onto the robot!"));
		setCursor(Qt::OpenHandCursor);
		energy_ = energy;
		min_ = min;
		max_ = max;
		pixRange_ = pixRange;
		width_ = 33;
		textBox_ = new QRectF(0, -15, width_, 30);
//		width_ = (int)floor( (end-start)/((max-min)/pixRange) );
	}

	QRectF boundingRect() const{
		return QRectF(textBox_->x(), textBox_->y(), textBox_->width(), textBox_->height());
	}
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
		Q_UNUSED(option);
		Q_UNUSED(widget);
		painter->setPen(Qt::NoPen);
		painter->setPen(QPen(Qt::black, 1));
		painter->setBrush(Qt::gray);
		QString energyVal;
		QRectF box(boundingRect());
		painter->drawText(box, Qt::AlignHCenter, energyVal.setNum(energy_), textBox_);
	}

private:
	QColor color;
	QRectF *textBox_;
	double energy_;
	double min_;
	double max_;
	int pixRange_;
	int width_;
};

class AMRegionsLineView : public QWidget
{
	Q_OBJECT
public:
	AMRegionsLineView(AMXASRegionsList *regions, QWidget *parent = 0);

protected slots:
	void handleDataChanged(QModelIndex topLeft, QModelIndex bottomRight);
	void handleRowsInsert(const QModelIndex &parent, int start, int end);
	void handleRowsRemoved(const QModelIndex &parent, int start, int end);
	void redrawRegionsLine();

protected:
	AMXASRegionsList *regions_;
	QGraphicsScene *scene;
	QGraphicsView *view;
};

#endif // AMREGIONSLINEVIEW_H
