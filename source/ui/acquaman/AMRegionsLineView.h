/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


class AMRegionItem : public QGraphicsItem
{
public:
 	virtual ~AMRegionItem();
	AMRegionItem(double start, double delta, double end, double min, double max, int pixRange, const QString &units = "");

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
	QString units_;
};

class AMEnergyIndexItem : public QGraphicsItem
{
public:
 	virtual ~AMEnergyIndexItem();
	AMEnergyIndexItem(double energy, double min, double max, int pixRange, const QString &units = "");

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
	QString units_;
};

class AMRegionsLineView : public QWidget
{
	Q_OBJECT
public:
 	virtual ~AMRegionsLineView();
	AMRegionsLineView(AMRegionsList *regions, QWidget *parent = 0);

protected slots:
	void handleDataChanged(QModelIndex topLeft, QModelIndex bottomRight);
	void handleRowsInsert(const QModelIndex &parent, int start, int end);
	void handleRowsRemoved(const QModelIndex &parent, int start, int end);
	virtual void redrawRegionsLine();

protected:
	AMRegionsList *regions_;
	QGraphicsScene *scene;
	QGraphicsView *view;
};

/// This class extends the AMRegionsLineView to allow for proper drawing of EXAFS regions where there is a mixture of energy space and k-space values.
class AMEXAFSLineView : public AMRegionsLineView
{
	Q_OBJECT

public:
	/// Constructor.
 	virtual ~AMEXAFSLineView();
	AMEXAFSLineView(AMEXAFSRegionsList *regions, QWidget *parent = 0);

protected slots:
	/// Function that handles drawing all of the line view.
	virtual void redrawRegionsLine();
};

#endif // AMREGIONSLINEVIEW_H
