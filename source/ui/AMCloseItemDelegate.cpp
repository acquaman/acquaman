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


#include "AMCloseItemDelegate.h"
#include "acquaman.h"

AMCloseItemDelegate::~AMCloseItemDelegate(){}

AMCloseItemDelegate::AMCloseItemDelegate(QObject *parent) :
		QStyledItemDelegate(parent)
{
	closeButtonEnabled_ = false;
	closeButtonAction_ = SignalOnly;
}

#include <QStyleOptionViewItemV4>
#include <QApplication>
void AMCloseItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

	if(closeButtonEnabled_ && index.data(AM::CanCloseRole).toBool()) {
		// trick the base class paint version into thinking we have less width than we do.
		QStyleOptionViewItemV4 opt(option);
		initStyleOption(&opt, index);
		opt.rect.setWidth(option.rect.width() - 20);
		QStyledItemDelegate::paint(painter, opt, index);

		// finish drawing the background in the remaining width
		QStyle* sty = opt.widget ? opt.widget->style() : QApplication::style();
		opt.rect = option.rect;
		opt.rect.setLeft(option.rect.right() - 20);
		sty->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter);

	}
	else
		QStyledItemDelegate::paint(painter, option, index);

	drawCloseButton(painter, option, index);
}

QSize AMCloseItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
	QSize rv = QStyledItemDelegate::sizeHint(option, index);
	if(closeButtonEnabled_ && index.data(AM::CanCloseRole).toBool())
		rv.setWidth(rv.width() + 20);
	return rv;
}
#include <QPainter>
void AMCloseItemDelegate::drawCloseButton(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

	Q_UNUSED(index)

	// Display close button?
	if(closeButtonEnabled_ && index.data(AM::CanCloseRole).toBool()) {
		// "local" rectangle of the close button.
		closeButtonRect_ = QRect(
				option.rect.width() - 16,
				(option.rect.height() - 13)/2,
				12,
				13);
		painter->drawPixmap(option.rect.left() + closeButtonRect_.left(),
							option.rect.top() + closeButtonRect_.top(),
							closeButton_);
	}
}


#include <QEvent>
#include <QMouseEvent>
#include <QApplication>

bool AMCloseItemDelegate::editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index ) {
	if(!closeButtonEnabled_ || !index.data(AM::CanCloseRole).toBool()) {
		return QStyledItemDelegate::editorEvent(event, model, option, index);
	}


	if(event->type() == QEvent::MouseButtonPress) {
		mouseDownPosition_ = static_cast<QMouseEvent*>(event)->pos();
		QPoint localPos = mouseDownPosition_ - option.rect.topLeft();

		if(closeButtonRect_.contains(localPos)) {
			// On mac, qlistviews in comboboxes don't report mouse-button release events. So we have to handle it here.
#ifdef Q_WS_MAC
			emit closeButtonClicked(index);

			if(closeButtonAction_ == RemoveFromModel) {
				model->removeRow(index.row(), index.parent());
			}
#endif
			// given that we've acted on this, we want this event.
			return true;
		}
	}

#ifndef Q_WS_MAC
	else if(event->type() == QEvent::MouseButtonRelease) {
		QMouseEvent* e = static_cast<QMouseEvent*>(event);
		// Normally, a "click" has to be released nearly in the same position as where it was pushed down. I should actually check that both the down and the up are within the confines of the same widget/button, but we don't really have buttons here now.
		// This is our criteria for a valid click. Now is it on a close button?
		if( (mouseDownPosition_ - e->pos()).manhattanLength() < QApplication::startDragDistance() ) {
			QPoint localPos = e->pos() - option.rect.topLeft();

			// we're on the button!
			if( closeButtonRect_.contains(localPos) ) {
				emit closeButtonClicked(index);

				if(closeButtonAction_ == RemoveFromModel) {
					model->removeRow(index.row(), index.parent());
				}
				// given that we've acted on this, we want this event.
				return true;
			}
		}
	}
#endif

	return QStyledItemDelegate::editorEvent(event, model, option, index);
}
