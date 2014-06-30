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


#ifndef AMDETAILEDITEMDELEGATE_H
#define AMDETAILEDITEMDELEGATE_H

#include "ui/AMCloseItemDelegate.h"
#include <QStyleOptionViewItemV4>
#include <QPainter>
#include <QPixmap>

#include "util/AMFontSizes.h"
#include "util/AMDateTimeUtils.h"

class AMDetailedItemDelegate : public AMCloseItemDelegate
{
Q_OBJECT
public:

	explicit AMDetailedItemDelegate(QObject *parent = 0);
	virtual ~AMDetailedItemDelegate();

	/// Re-implemented from QStyledItemDelegate to request the height requested in setItemHeight().
	virtual QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const {
		QSize rv = AMCloseItemDelegate::sizeHint(option, index);
		rv.setHeight(height_);
		return rv;
	}

	/// Re-implemented from QStyledItemDelegate to draw two lines of text, the pixmap/decoration, and a close button if closeButtonEnabled().
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;


	/// Spacing between decoration and text
	static int horizontalSpacing() { return 6; }
	/// Horizontal margin before the decoration on the left side
	static int horizontalMargin() { return 6; }
	/// Vertical margin before above the decoration and text
	static int verticalMargin() { return 3; }

	/// Set the font used for the text
	void setFont(const QFont& font= QFont("Lucida Grande"));
	/// Set the text color used for the top and bottom lines.
	void setTextColor(const QColor &topLineColor= QColor(47,79,79), const QColor &bottomLineColor = QColor(96,123,139));
	/// Set the font size used for the top and bottom lines
	void setFontSize(int size1 = AM_FONT_REGULAR,int size2 = AM_FONT_SMALL);
	/// Request a specific \c height for each item
	void setItemHeight(int height=40);


protected:
	mutable QFont font_, fontItalic_;
	QColor color1_;
	QColor color2_;
	int size1_;
	int size2_;
	int height_;
};

#endif // AMDETAILEDITEMDELEGATE_H
