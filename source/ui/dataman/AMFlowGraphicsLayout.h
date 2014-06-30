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


#ifndef AMFLOWGRAPHICSLAYOUT_H
#define AMFLOWGRAPHICSLAYOUT_H

#include <QGraphicsLayout>

#include <QtGui/qgraphicslayout.h>

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

/// This class implements a "flow"-style layout for a QGraphicsWidget. It is taken from the Qt Flow Layout example (graphicsview/flowlayout/flowlayout.h).
 class AMFlowGraphicsLayout : public QGraphicsLayout
 {
 public:
	 AMFlowGraphicsLayout();

	 /// Added by mark.boots@usask.ca to fix crashes when a layout is deleted before the layout items it contains.
	 virtual ~AMFlowGraphicsLayout();

	 /// Added to manually allow setting a width constraint, which will be used inside sizeHint() whenever the supplied width constraint is < 0:
	 void setDefaultWidth(double widthConstraint) { defaultWidth_ = widthConstraint; updateGeometry();  }

	 /// Set the size constraint used for the individual items in the layout
	 void setItemSizeConstraint(const QSizeF& itemSizeConstraint) {
		 itemSizeConstraint_ = itemSizeConstraint;
		 updateGeometry();
	 }

	 inline void addItem(QGraphicsLayoutItem *item);
	 void insertItem(int index, QGraphicsLayoutItem *item);
	 void setSpacing(Qt::Orientations o, qreal spacing);
	 qreal spacing(Qt::Orientation o) const;

	 // inherited functions
	 void setGeometry(const QRectF &geom);

	 int count() const;
	 QGraphicsLayoutItem *itemAt(int index) const;
	 void removeAt(int index);

	 /// If you know the items are all the same size as the first one, this can be used to optimize the layout performance
	 void setUniformItemSizes(bool itemsAreUniform) { uniformItemSizes_ = itemsAreUniform; }

 protected:


	 QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;

 private:
	 qreal doLayout(const QRectF &geom, bool applyNewGeometry) const;
	 /// calculate the height required, if the whole width available is \c width, and all the items have the same size. (This is a shortcut optimization when uniformItemSizes_ is true.)
	qreal heightForWidth(qreal width) const;


	 QList<QGraphicsLayoutItem*> m_items;
	 qreal m_spacing[2];

	 /// used as the width assumed when calculating the sizeHint, if no width constraint is provided.
	double defaultWidth_;
	 bool uniformItemSizes_;

	 QSizeF itemSizeConstraint_;
 };

 inline void AMFlowGraphicsLayout::addItem(QGraphicsLayoutItem *item)
 {
	 insertItem(-1, item);
 }

#endif // AMFLOWGRAPHICSLAYOUT_H
