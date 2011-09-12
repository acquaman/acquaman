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


#ifndef AMREGIONSLINEVIEW_H
#define AMREGIONSLINEVIEW_H

#include <QtGui>
#include <QGraphicsItem>
#include "acquaman/AMRegionsList.h"


class RegionItem : public QGraphicsItem
{
public:
	RegionItem(double start, double delta, double end, double min, double max, int pixRange);

	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

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
	EnergyIndexItem(double energy, double min, double max, int pixRange);

	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

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
