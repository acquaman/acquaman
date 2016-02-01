#include "AMScanTreeView.h"

AMScanTreeView::AMScanTreeView(QWidget *parent) :
	QTreeView(parent)
{
	setItemDelegate(new AMScanTreeViewDelegate(this));
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setVerticalScrollMode(ScrollPerPixel);
	verticalScrollBar()->setSingleStep(20);
	setExpandsOnDoubleClick(false);
}

void AMScanTreeView::setModel(QAbstractItemModel *model)
{
	QTreeView::setModel(model);
	setColumnWidth(0, 140);
	setColumnWidth(1, 280);
	setColumnWidth(2, 140);
	setColumnWidth(3, 120);
	setColumnWidth(4, 120);

}


AMScanTreeViewDelegate::AMScanTreeViewDelegate(QObject *parent)
	:QItemDelegate(parent)
{

}

QSize AMScanTreeViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if(!index.parent().isValid()) {
		return QSize(option.rect.width(), option.fontMetrics.height());
	} else {
		return QSize(option.rect.width(), 200);
	}
}

void AMScanTreeViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

	QVariant indexDisplayData = index.data(Qt::DisplayRole);

	if(indexDisplayData.type() == QVariant::String) {
		QString indexDisplayStringData = indexDisplayData.toString();

		if(option.fontMetrics.width(indexDisplayStringData) > option.rect.width()) {
			drawDisplay(painter, option, option.rect,
						option.fontMetrics.elidedText(
							indexDisplayStringData.trimmed(), Qt::ElideRight, option.rect.width()));

		} else {
			QItemDelegate::paint(painter, option, index);
		}
	} else {
		QItemDelegate::paint(painter, option, index);
	}
}
