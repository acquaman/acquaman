#include "AMScanThumbnailGridViewItemDelegate.h"
#include "util/AMDateTimeUtils.h"

AMScanThumbnailGridViewItemDelegate::AMScanThumbnailGridViewItemDelegate(QObject *parent) :
	QItemDelegate(parent)
{
}

void AMScanThumbnailGridViewItemDelegate::paint(QPainter *painter,
												const QStyleOptionViewItem &option,
												const QModelIndex &index) const
{

    if(option.rect.isEmpty())
		return;

	painter->save();

    if(!index.isValid()) {
        paintItemBackground(painter, option);
    }

	if(!index.parent().isValid()) {
		// Is a scan

		switch (index.column()) {
		case 0:
            paintText(painter, option, index.data(Qt::DisplayRole).toString());
			break;
		case 1:
			paintText(painter, option, index.data(Qt::DisplayRole).toString());
			break;
		case 3:
			paintText(painter, option,
					  AMDateTimeUtils::prettyDateTime(index.data(Qt::DisplayRole).toDateTime()));
			break;
		case 4:
			paintText(painter, option, index.data(Qt::DisplayRole).toString());
			break;
		default:
			break;
		}

	} else {
		// Is a thumbnail
		switch (index.column()) {
		case 1:
			paintThumbnail(painter, option, index.data(Qt::DecorationRole).value<QPixmap>());
			break;
		case 2:
			paintText(painter, option, index.data(Qt::DisplayRole).toString());
			break;
		default:
			break;
		}
	}

	painter->restore();
}

void AMScanThumbnailGridViewItemDelegate::paintThumbnail(QPainter *painter,
															  const QStyleOptionViewItem &option,
															  const QPixmap &pixmap) const
{
	painter->drawPixmap(option.rect, pixmap);
}

void AMScanThumbnailGridViewItemDelegate::paintText(QPainter *painter,
													const QStyleOptionViewItem &option,
													const QString &string) const
{

	QString textToPaint = option.fontMetrics.elidedText(
				string, Qt::ElideRight, option.rect.width());

	if(option.state & QStyle::State_Selected) {
		painter->setPen(option.palette.color(QPalette::Normal, QPalette::HighlightedText));
	} else {
		painter->setPen(option.palette.color(QPalette::Normal, QPalette::Text));
	}

	painter->drawText(option.rect, textToPaint);
}

void AMScanThumbnailGridViewItemDelegate::paintItemBackground(QPainter *painter, const QStyleOptionViewItem &option) const
{

	if(option.state & QStyle::State_Selected) {
		painter->setBrush(option.palette.brush(QPalette::Normal, QPalette::Highlight));
	} else {
		painter->setBrush(QBrush(Qt::white));
	}

	painter->setPen(QPen(QBrush(QColor(150,160,168)), 2));
	painter->drawRoundedRect(option.rect, 5, 5);
}
