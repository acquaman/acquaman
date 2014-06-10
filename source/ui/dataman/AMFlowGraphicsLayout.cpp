/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMFlowGraphicsLayout.h"

// This implementation is taken from the Qt Flow Layout example (graphicsview/AMFlowGraphicsLayout/flowlayout.cpp).
/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "ui/dataman/AMFlowGraphicsLayout.h"
#include <QtGui/qwidget.h>
#include <QtCore/qmath.h>


AMFlowGraphicsLayout::AMFlowGraphicsLayout()
{
	m_spacing[0] = 6;
	m_spacing[1] = 6;
	QSizePolicy sp = sizePolicy();
	sp.setHeightForWidth(true);
	setSizePolicy(sp);

	defaultWidth_ = 400;
	uniformItemSizes_ = false;

}

/*! A common situation is for graphics items to inherit both from QGraphicsItem and QGraphicsLayoutItem. This creates an ambiguity into which "parent" owns the item: the QGraphicsItem* parent, or the parent layout (QGraphicsLayoutItem* parent).  [see QGraphicsLayoutItem::ownedByLayout().]

If QGraphicsLayoutItem::ownedByLayout() is true, we must delete the items this layout contains when it is deleted.

If not, we must remove them from the layout, because they might not be deleted yet, and this layout will soon be.

\note This second scenario happens whenever a parent QGraphicsItem has a layout set on it, and is also the parent of the items within that layout.  Omitting this code causes crashes when the items themselves are deleted; they believe that they have a layout, but the layout is gone.

This functionality probably belongs in ~QGraphicsLayout, but it isn't.
*/
AMFlowGraphicsLayout::~AMFlowGraphicsLayout() {

	for (int i = count() - 1; i >= 0; --i) {

		QGraphicsLayoutItem *item = itemAt(i);
		removeAt(i);

		if (item) {
			item->setParentLayoutItem(0);
			if (item->ownedByLayout())
				delete item;
		}
	}
}

void AMFlowGraphicsLayout::insertItem(int index, QGraphicsLayoutItem *item)
{
	item->setParentLayoutItem(this);
	if (uint(index) > uint(m_items.count()))
		index = m_items.count();
	m_items.insert(index, item);
	invalidate();
}

int AMFlowGraphicsLayout::count() const
{
	return m_items.count();
}

QGraphicsLayoutItem *AMFlowGraphicsLayout::itemAt(int index) const
{
	return m_items.value(index);
}

void AMFlowGraphicsLayout::removeAt(int index)
{
	m_items.removeAt(index);
	invalidate();
}

qreal AMFlowGraphicsLayout::spacing(Qt::Orientation o) const
{
	return m_spacing[int(o) - 1];
}

void AMFlowGraphicsLayout::setSpacing(Qt::Orientations o, qreal spacing)
{
	if (o & Qt::Horizontal)
		m_spacing[0] = spacing;
	if (o & Qt::Vertical)
		m_spacing[1] = spacing;
}

void AMFlowGraphicsLayout::setGeometry(const QRectF &geom)
{
	QGraphicsLayout::setGeometry(geom);
	doLayout(geom, true);
}

qreal AMFlowGraphicsLayout::doLayout(const QRectF &geom, bool applyNewGeometry) const
{
	qreal left, top, right, bottom;
	getContentsMargins(&left, &top, &right, &bottom);
	const qreal maxw = geom.width() - left - right;

	qreal x = 0;
	qreal y = 0;
	qreal maxRowHeight = 0;
	QSizeF pref;
	for (int i = 0; i < m_items.count(); ++i) {
		QGraphicsLayoutItem *item = m_items.at(i);
		pref = item->effectiveSizeHint(Qt::PreferredSize, itemSizeConstraint_);
		maxRowHeight = qMax(maxRowHeight, pref.height());

		qreal next_x;
		next_x = x + pref.width();
		if (next_x > maxw) {
			if (x == 0) {
				pref.setWidth(maxw);
			} else {
				x = 0;
				next_x = pref.width();
				// moved from below into this else statement:
				y += maxRowHeight + spacing(Qt::Vertical);
				maxRowHeight = 0;
			}

		}

		if (applyNewGeometry)
			item->setGeometry(QRectF(QPointF(left + x, top + y), pref));
		x = next_x + spacing(Qt::Horizontal);
	}
	maxRowHeight = qMax(maxRowHeight, pref.height());
	return top + y + maxRowHeight + bottom;
}






QSizeF AMFlowGraphicsLayout::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
	QSizeF sh = constraint;

	if(sh.width() < 0) {
		sh.setWidth(defaultWidth_);
	}

	qreal height;

	switch (which) {
	case Qt::PreferredSize:
	case Qt::MinimumSize:
	case Qt::MaximumSize:

		if(uniformItemSizes_)
			height = heightForWidth(sh.width());
		else
			height = doLayout(QRectF(QPointF(0,0), sh), false);

		sh = QSizeF(constraint.width(), height);
		break;
	default: break;
	}

	return sh;
}

qreal AMFlowGraphicsLayout::heightForWidth(qreal width) const
{
	qreal left, top, right, bottom;
	getContentsMargins(&left, &top, &right, &bottom);


	qreal maxWidth = width - left - right;
	QSizeF itemSize = m_items.count() > 0 ? m_items.first()->effectiveSizeHint(Qt::PreferredSize, itemSizeConstraint_) : QSizeF(10,10);
	qreal horizontalSpacing = spacing(Qt::Horizontal);

	int itemsPerRow = int((maxWidth + horizontalSpacing) / (itemSize.width() + horizontalSpacing));	// algebra from: maxWidth = num*width + (num-1)*horizontalSpacing
	if(itemsPerRow < 1)
		itemsPerRow = 1;


	int rowsRequired = int(ceil(m_items.count() / double(itemsPerRow)));	// round up here

	qreal itemsHeight = rowsRequired*itemSize.height();
	if(rowsRequired > 1)	// add spacing (x the number of spaces, not the number of rows!) if there's more than one row.
		itemsHeight += (rowsRequired-1)*spacing(Qt::Vertical);

	return itemsHeight + top + bottom;

}

