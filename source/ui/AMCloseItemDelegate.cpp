#include "AMCloseItemDelegate.h"
#include "acquaman.h"

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

	/// Display close button?
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

	if(!closeButtonEnabled_ || !index.data(AM::CanCloseRole).toBool())
		return QStyledItemDelegate::editorEvent(event, model, option, index);


	if(event->type() == QEvent::MouseButtonRelease) {
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

				// handle anything special the view might need to do on mouse-release?
				// QStyledItemDelegate::editorEvent(event, model, option, index);

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
