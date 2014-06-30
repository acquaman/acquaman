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


#include "AMDetailedItemDelegate.h"
#include <QStyle>
#include <QApplication>
#include <QFontMetrics>
#include <QStyleOptionButton>
#include "acquaman.h"

AMDetailedItemDelegate::~AMDetailedItemDelegate(){}

AMDetailedItemDelegate::AMDetailedItemDelegate(QObject *parent) :
		AMCloseItemDelegate(parent)
{
	// apply defaults:
	setFont();
	setTextColor();
	setFontSize();
	setItemHeight();
}

void AMDetailedItemDelegate::setItemHeight(int height){
	height_=height;
}

void AMDetailedItemDelegate::setFont(const QFont& font){
	font_=font;
	fontItalic_ = font;
	// is the opposite italicization of whatever font you gave us.
	fontItalic_.setItalic(!font.italic());
}

void AMDetailedItemDelegate::setTextColor(const QColor &color1, const QColor &color2){
	color1_ = color1;
	color2_ = color2;
}

void AMDetailedItemDelegate::setFontSize(int size1,int size2){
	size1_=size1;
	size2_=size2;
}


void AMDetailedItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const  {

	QStyleOptionViewItemV4 opt(option);
	initStyleOption(&opt, index);

	QStyle* sty = QApplication::style();

	// Draw the background: (this will handle selection for us. You can also probe selection directly with opt.state & QStyle::State_Selected)
	sty->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter);

	int textStartingPoint = horizontalMargin();
	// Do we have a pixmap available?
	QRect pmap = opt.rect;
	pmap.setLeft( pmap.left() + horizontalMargin() );
	pmap.setTop( pmap.top() + verticalMargin() );
	pmap.setBottom( pmap.bottom() - verticalMargin() );

	if(opt.features & QStyleOptionViewItemV2::HasDecoration) {
		QPixmap p = opt.icon.pixmap(pmap.height());
		sty->drawItemPixmap(painter, pmap, Qt::AlignLeft | Qt::AlignVCenter, p);
		textStartingPoint += horizontalMargin() + p.width() + horizontalSpacing();
	}

	QRect textRect = opt.rect;
	textRect.setLeft( textRect.left() + textStartingPoint);


	if (index.data(AM::ModifiedRole).toBool()) {
		fontItalic_.setPointSize(size1_);
		painter->setFont(fontItalic_);
	}
	else {
		font_.setPointSize(size1_);
		painter->setFont(font_);
	}
	painter->setPen(color1_);

	painter->drawText(textRect, index.data(Qt::DisplayRole).toString());

	QVariant description = index.data(AM::DescriptionRole);
	if(!description.isNull()){
		font_.setPointSize(size2_);
		painter->setFont(font_);
		painter->setPen(color2_);
		QFontMetrics fontMetrics(font_);

		painter->drawText(textRect.translated(QPoint(0,20)), fontMetrics.elidedText(description.toString(), Qt::ElideRight, textRect.width() ));

	}

	// call base class to draw close button:
	drawCloseButton(painter, opt, index);
}



